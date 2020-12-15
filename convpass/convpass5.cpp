#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/CodeMetrics.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Analysis/DomTreeUpdater.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <vector>
#include <algorithm>

using namespace llvm;




bool mergeBlocksIntoPredecessors5(Loop &L, DominatorTree &DT,
                                        LoopInfo &LI, MemorySSAUpdater *MSSAU) {
  bool Changed = false;
  DomTreeUpdater DTU(DT, DomTreeUpdater::UpdateStrategy::Eager);
  // Copy blocks into a temporary array to avoid iterator invalidation issues
  // as we remove them.
  SmallVector<WeakTrackingVH, 16> Blocks(L.blocks());

  for (auto &Block : Blocks) {
    // Attempt to merge blocks in the trivial case. Don't modify blocks which
    // belong to other loops.
    BasicBlock *Succ = cast_or_null<BasicBlock>(Block);
    if (!Succ)
      continue;
    errs() << Succ->getName() << "\n";
    BasicBlock *Pred = Succ->getSinglePredecessor();
    if (!Pred)
	continue;
    errs() << Succ->getName() << " " << Pred->getName() << " " << LI.getLoopFor(Succ)->getLoopPreheader()->getName() << " " << LI.getLoopFor(Pred)->getLoopPreheader()->getName() << "\n";
    if (!Pred || !Pred->getSingleSuccessor() || LI.getLoopFor(Pred) != &L)
      continue;

    // Merge Succ into Pred and delete it.
    MergeBlockIntoPredecessor(Succ, &DTU, &LI, MSSAU);

    if (MSSAU && VerifyMemorySSA)
      MSSAU->getMemorySSA()->verifyMemorySSA();

    Changed = true;
  }

  return Changed;
}



namespace {
struct ConvPass5 : public FunctionPass {
  static char ID;
  ConvPass5() : FunctionPass(ID) {}

  unsigned UnrollMaxUpperBound = 10;
  Optional<unsigned> ProvidedCount;
  Optional<unsigned> ProvidedThreshold;
  Optional<bool> ProvidedAllowPartial;
  Optional<bool> ProvidedRuntime;
  Optional<bool> ProvidedUpperBound;
  Optional<bool> ProvidedAllowPeeling;
  Optional<bool> ProvidedAllowProfileBasedPeeling;
  Optional<unsigned> ProvidedFullUnrollMaxCount;

  // Specify the list of analysis passes that will be used inside your pass.
  void getAnalysisUsage(AnalysisUsage &AU) const {
    //AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
    //AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
    AU.addRequired<AssumptionCacheTracker>();
    AU.addRequired<TargetTransformInfoWrapperPass>();
  }
  
  
  bool runOnFunction(Function &F) override {
    //BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    //BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    
    // Check to make sure function is standard convolution, with single nested loop
    int topLevelLoopCount = 0;
    for(LoopInfo::iterator i = LI.begin(); i != LI.end(); ++i){
        topLevelLoopCount += 1;
    }
    if(topLevelLoopCount!=1){
        return false;
    }

    
    // Recurse through nested loops, keep track of loops for optimization
    std::vector<Loop*> convLoops;
    Loop *currLoop = *LI.begin();
    while(currLoop){
	convLoops.push_back(currLoop);
        int numSubLoops = currLoop->getSubLoops().size();

        if(numSubLoops>1){
            // More than single sub loop implies non-standard conv function
            return false;
	}
	else if (numSubLoops==1) {
            // Single sub loop implies keep recursing
            currLoop = currLoop->getSubLoops().front();
	}
	else {
	    // No sub loops implies end of nest
            currLoop = NULL;
	}
    }
    
    // If odd number of nested loops, non-standard conv function
    if(convLoops.size()%2==1){
        return false;
    }
    
    // Reverse the list of loops to ensure unfold starts at innermost nest
    std::reverse(std::begin(convLoops), std::end(convLoops));
    
    errs() << "Found a total of " << convLoops.size() << " nested loops" << "\n";
    for(int i=0; i<convLoops.size(); i++) {
        Loop *L = convLoops[i];
        errs() << "Loop Stored: F["
                    << L->getHeader()->getParent()->getName() << "] Loop %"
                    << L->getHeader()->getName() << "\n";
    }

    int OptLevel = 3;
    bool OnlyWhenForced = false;
    bool ForgetAllSCEV = false;
    for(int i=0; i<convLoops.size()/2; i++) {
	Optional<unsigned> KernelCount(5+i);
	
	
	Loop *L = convLoops[i];

        BasicBlock *Headerr = L->getHeader();
        BranchInst *HeaderBI = dyn_cast<BranchInst>(Headerr->getTerminator());	
        errs() << "inf " << Headerr->getName() << " " << (bool)HeaderBI->isUnconditional() << " " << (L->getExitingBlock() != Headerr) << " " << (bool)L->getExitingBlock() << "\n";	
	Function &F = *L->getHeader()->getParent();
 
        auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
        LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
        ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
        const TargetTransformInfo &TTI = getAnalysis<TargetTransformInfoWrapperPass>().getTTI(F);
        auto &AC = getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);
        // For the old PM, we can't use OptimizationRemarkEmitter as an analysis
        // pass.  Function analyses need to be preserved across loop transformations
        // but ORE cannot be preserved (see comment before the pass definition).
        OptimizationRemarkEmitter ORE(&F);
        bool PreserveLCSSA = true;
        
	/*
	LoopUnrollResult Result = tryToUnrollLoop(
            L, DT, LI, SE, TTI, AC, ORE, nullptr, nullptr, PreserveLCSSA, OptLevel,
            OnlyWhenForced, ForgetAllSCEV, KernelCount, ProvidedThreshold,
            ProvidedAllowPartial, ProvidedRuntime, ProvidedUpperBound,
            ProvidedAllowPeeling, ProvidedAllowProfileBasedPeeling,
            ProvidedFullUnrollMaxCount, UnrollMaxUpperBound);
        
        */
        BasicBlock *PreHeader = L->getLoopPreheader();
        BasicBlock *Header = L->getHeader();
	unsigned count = 5;
	unsigned tripCount = 5;
	bool force = false;
	bool allowRuntime = true;
	bool allowExpensiveTripCount = false;
	bool preserveCondBr = false;
	bool preserveOnlyFirst = false;
	unsigned tripMultiple = 1;
	unsigned peelCount = 0;
	bool unrollRemainder = false;
	bool forgetAllSCEV = false;
        MDNode *OrigLoopID = L->getLoopID();
	errs() << L->getNumBlocks() << "\n";
	//{UP.Count, TripCount, UP.Force, UP.Runtime, UP.AllowExpensiveTripCount,
	//
        // UseUpperBound, MaxOrZero, TripMultiple, UP.PeelCount, UP.UnrollRemainder,
        // ForgetAllSCEV},
	// Unroll the loop.
        Loop *RemainderLoop = nullptr;
        LoopUnrollResult UnrollResult = UnrollLoop(
               L,
	       {count, tripCount, force, allowRuntime, allowExpensiveTripCount,
                preserveCondBr, preserveOnlyFirst, tripMultiple, peelCount, 
		unrollRemainder, forgetAllSCEV},
	       LI, &SE, &DT, &AC, &ORE, PreserveLCSSA, &RemainderLoop);
        if (UnrollResult == LoopUnrollResult::Unmodified)
            break; //return LoopUnrollResult::Unmodified;

        if (RemainderLoop) {
            Optional<MDNode *> RemainderLoopID =
                  makeFollowupLoopID(OrigLoopID, {LLVMLoopUnrollFollowupAll,
                                                  LLVMLoopUnrollFollowupRemainder});
            if (RemainderLoopID.hasValue())
                RemainderLoop->setLoopID(RemainderLoopID.getValue());
        }

        if (UnrollResult != LoopUnrollResult::FullyUnrolled) {
            Optional<MDNode *> NewLoopID =
                      makeFollowupLoopID(OrigLoopID, {LLVMLoopUnrollFollowupAll,
                                                      LLVMLoopUnrollFollowupUnrolled});
            if (NewLoopID.hasValue()) {
                L->setLoopID(NewLoopID.getValue());

            // Do not setLoopAlreadyUnrolled if loop attributes have been specified
            // explicitly.
            //return UnrollResult;
            }
	} 

	errs() << "Tried loop " << i << " " << (int)UnrollResult << "\n";
        errs() << PreHeader->getName() << " " << PreHeader->getTerminator()->getNumSuccessors() << "\n";
	BasicBlock *nextBlock = PreHeader;
	Instruction *lastInst = nextBlock->getTerminator();
        unsigned numSuc = lastInst->getNumSuccessors();
        while(numSuc==1) {
            nextBlock = lastInst->getSuccessor(0);
	    lastInst = nextBlock->getTerminator();
            numSuc = lastInst->getNumSuccessors();
	}
	assert(numSuc==2);
	BasicBlock *finalUnroll = lastInst->getSuccessor(0);
	BasicBlock *exitLatch = lastInst->getSuccessor(1);
        errs() << "Stopped at " << lastInst->getOpcodeName() << " " << finalUnroll->getName() << " " << exitLatch->getName() << "\n";
        

	lastInst->eraseFromParent();
	BranchInst *uncondBrUnroll = BranchInst::Create(finalUnroll, nextBlock);
        bool done = false;
	while(!done) {
	    Instruction *unreachInst = finalUnroll->getTerminator();
	    if(unreachInst->getOpcode()==7) {
	      convLoops[i+1]->addBasicBlockToLoop(finalUnroll, *LI);
	      unreachInst->eraseFromParent();
              done = true;
	      break;
	    }
	    convLoops[i+1]->addBasicBlockToLoop(finalUnroll, *LI);
	    finalUnroll = finalUnroll->getTerminator()->getSuccessor(0);
	}

	BranchInst *uncondBrLatch = BranchInst::Create(exitLatch, finalUnroll);
	errs() << "Merging from " << Header->getName() << "\n";
        mergeBlocksIntoPredecessors5(*convLoops[i+1], DT, *LI, nullptr);	
	//errs() << "loopsleft " << convLoopps.size() << "\n";
        	
    }


    
    return false;
  }
}; // end of struct ConvPass
}  // end of anonymous namespace

char ConvPass5::ID = 0;
static RegisterPass<ConvPass5> X("convpass5", "Convolution Pass 5x5 Kernel",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

