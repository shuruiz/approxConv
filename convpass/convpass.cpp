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

#include <vector>
#include <algorithm>

using namespace llvm;





bool computeFullUnrollCount(
    Loop *L, const TargetTransformInfo &TTI, DominatorTree &DT, LoopInfo *LI,
    ScalarEvolution &SE, const SmallPtrSetImpl<const Value *> &EphValues,
    OptimizationRemarkEmitter *ORE, unsigned &TripCount, unsigned MaxTripCount,
    bool MaxOrZero, unsigned &TripMultiple, unsigned LoopSize,
    TargetTransformInfo::UnrollingPreferences &UP, bool &UseUpperBound,
    unsigned UnrollMaxUpperBound) {
  bool ExplicitUnroll = false;
  // 3rd priority is full unroll count.
  // Full unroll makes sense only when TripCount or its upper bound could be
  // statically calculated.
  // Also we need to check if we exceed FullUnrollMaxCount.
  // If using the upper bound to unroll, TripMultiple should be set to 1 because
  // we do not know when loop may exit.

  // We can unroll by the upper bound amount if it's generally allowed or if
  // we know that the loop is executed either the upper bound or zero times.
  // (MaxOrZero unrolling keeps only the first loop test, so the number of
  // loop tests remains the same compared to the non-unrolled version, whereas
  // the generic upper bound unrolling keeps all but the last loop test so the
  // number of loop tests goes up which may end up being worse on targets with
  // constrained branch predictor resources so is controlled by an option.)
  // In addition we only unroll small upper bounds.
  unsigned FullUnrollMaxTripCount = MaxTripCount;
  if (!(UP.UpperBound || MaxOrZero) ||
      FullUnrollMaxTripCount > UnrollMaxUpperBound)
    FullUnrollMaxTripCount = 0;
  errs() << FullUnrollMaxTripCount << "fumtc\n";
  // UnrollByMaxCount and ExactTripCount cannot both be non zero since we only
  // compute the former when the latter is zero.
  unsigned ExactTripCount = TripCount;
  assert((ExactTripCount == 0 || FullUnrollMaxTripCount == 0) &&
         "ExtractTripCount and UnrollByMaxCount cannot both be non zero.");
  errs() << ExactTripCount << "etc\n";
  unsigned FullUnrollTripCount =
      ExactTripCount ? ExactTripCount : FullUnrollMaxTripCount;
  UP.Count = FullUnrollTripCount;
  if (FullUnrollTripCount && FullUnrollTripCount <= UP.FullUnrollMaxCount) {
    // When computing the unrolled size, note that BEInsns are not replicated
    // like the rest of the loop body.
    //if (getUnrolledLoopSize(LoopSize, UP) < UP.Threshold) {
    UseUpperBound = (FullUnrollMaxTripCount == FullUnrollTripCount);
    TripCount = FullUnrollTripCount;
    TripMultiple = UP.UpperBound ? 1 : TripMultiple;
    return ExplicitUnroll;
   /* } else {
      // The loop isn't that small, but we still can fully unroll it if that
      // helps to remove a significant number of instructions.
      // To check that, run additional analysis on the loop.
      if (Optional<EstimatedUnrollCost> Cost = analyzeLoopUnrollCost(
              L, FullUnrollTripCount, DT, SE, EphValues, TTI,
              UP.Threshold * UP.MaxPercentThresholdBoost / 100)) {
        unsigned Boost =
            getFullUnrollBoostingFactor(*Cost, UP.MaxPercentThresholdBoost);
        if (Cost->UnrolledCost < UP.Threshold * Boost / 100) {
          UseUpperBound = (FullUnrollMaxTripCount == FullUnrollTripCount);
          TripCount = FullUnrollTripCount;
          TripMultiple = UP.UpperBound ? 1 : TripMultiple;
          return ExplicitUnroll;
        }
      }
    }*/
  }
  return ExplicitUnroll;
}

static LoopUnrollResult tryToUnrollLoop(
    Loop *L, DominatorTree &DT, LoopInfo *LI, ScalarEvolution &SE,
    const TargetTransformInfo &TTI, AssumptionCache &AC,
    OptimizationRemarkEmitter &ORE, BlockFrequencyInfo *BFI,
    ProfileSummaryInfo *PSI, bool PreserveLCSSA, int OptLevel,
    bool OnlyWhenForced, bool ForgetAllSCEV, Optional<unsigned> ProvidedCount,
    Optional<unsigned> ProvidedThreshold, Optional<bool> ProvidedAllowPartial,
    Optional<bool> ProvidedRuntime, Optional<bool> ProvidedUpperBound,
    Optional<bool> ProvidedAllowPeeling,
    Optional<bool> ProvidedAllowProfileBasedPeeling,
    Optional<unsigned> ProvidedFullUnrollMaxCount, unsigned UnrollMaxUpperBound) {
  errs() << "Loop Unroll: F["
                    << L->getHeader()->getParent()->getName() << "] Loop %"
                    << L->getHeader()->getName() << "\n";
  TransformationMode TM = hasUnrollTransformation(L);
  if (TM & TM_Disable)
    return LoopUnrollResult::Unmodified;
  if (!L->isLoopSimplifyForm()) {
    errs() << "  Not unrolling loop which is not in loop-simplify form.\n";
    return LoopUnrollResult::Unmodified;
  }

  // When automtatic unrolling is disabled, do not unroll unless overridden for
  // this loop.
  if (OnlyWhenForced && !(TM & TM_Enable))
    return LoopUnrollResult::Unmodified;

  bool OptForSize = L->getHeader()->getParent()->hasOptSize();
  unsigned NumInlineCandidates;
  bool NotDuplicatable;
  bool Convergent;
  TargetTransformInfo::UnrollingPreferences UP = gatherUnrollingPreferences(
      L, SE, TTI, BFI, PSI, OptLevel, ProvidedThreshold, ProvidedCount,
      ProvidedAllowPartial, ProvidedRuntime, ProvidedUpperBound,
      ProvidedAllowPeeling, ProvidedAllowProfileBasedPeeling,
      ProvidedFullUnrollMaxCount);

  // Exit early if unrolling is disabled. For OptForSize, we pick the loop size
  // as threshold later on.
  if (UP.Threshold == 0 && (!UP.Partial || UP.PartialThreshold == 0) &&
      !OptForSize)
    return LoopUnrollResult::Unmodified;

  SmallPtrSet<const Value *, 32> EphValues;
  CodeMetrics::collectEphemeralValues(L, &AC, EphValues);

  unsigned LoopSize =
      ApproximateLoopSize(L, NumInlineCandidates, NotDuplicatable, Convergent,
                          TTI, EphValues, UP.BEInsns);
  errs() << "  Loop Size = " << LoopSize << "\n";
  if (NotDuplicatable) {
    errs() << "  Not unrolling loop which contains non-duplicatable"
                      << " instructions.\n";
    return LoopUnrollResult::Unmodified;
  }

  // When optimizing for size, use LoopSize + 1 as threshold (we use < Threshold
  // later), to (fully) unroll loops, if it does not increase code size.
  if (OptForSize)
    UP.Threshold = std::max(UP.Threshold, LoopSize + 1);

  if (NumInlineCandidates != 0) {
    errs() << "  Not unrolling loop with inlinable calls.\n";
    return LoopUnrollResult::Unmodified;
  }

  // Find trip count and trip multiple if count is not available
  unsigned TripCount = 0;
  unsigned TripMultiple = 1;
  // If there are multiple exiting blocks but one of them is the latch, use the
  // latch for the trip count estimation. Otherwise insist on a single exiting
  // block for the trip count estimation.
  BasicBlock *ExitingBlock = L->getLoopLatch();
  if (!ExitingBlock || !L->isLoopExiting(ExitingBlock))
    ExitingBlock = L->getExitingBlock();
  if (ExitingBlock) {
    errs() << "EB: " << ExitingBlock->getName() << "\n";
    TripCount = SE.getSmallConstantTripCount(L, ExitingBlock);
    TripMultiple = SE.getSmallConstantTripMultiple(L, ExitingBlock);
  }
  TripCount = 5;
  errs() << "TC: " << (int)TripCount << " EB:" << (bool)ExitingBlock << "\n";

  // If the loop contains a convergent operation, the prelude we'd add
  // to do the first few instructions before we hit the unrolled loop
  // is unsafe -- it adds a control-flow dependency to the convergent
  // operation.  Therefore restrict remainder loop (try unrollig without).
  //
  // TODO: This is quite conservative.  In practice, convergent_op()
  // is likely to be called unconditionally in the loop.  In this
  // case, the program would be ill-formed (on most architectures)
  // unless n were the same on all threads in a thread group.
  // Assuming n is the same on all threads, any kind of unrolling is
  // safe.  But currently llvm's notion of convergence isn't powerful
  // enough to express this.
  if (Convergent)
    UP.AllowRemainder = false;

  // Try to find the trip count upper bound if we cannot find the exact trip
  // count.
  unsigned MaxTripCount = 0;
  bool MaxOrZero = false;
  if (!TripCount) {
    MaxTripCount = SE.getSmallConstantMaxTripCount(L);
    MaxOrZero = SE.isBackedgeTakenCountMaxOrZero(L);
  }

  // computeUnrollCount() decides whether it is beneficial to use upper bound to
  // fully unroll the loop.
  bool UseUpperBound = false;
  bool IsCountSetExplicitly = computeFullUnrollCount(
      L, TTI, DT, LI, SE, EphValues, &ORE, TripCount, MaxTripCount, MaxOrZero,
      TripMultiple, LoopSize, UP, UseUpperBound, UnrollMaxUpperBound);
  errs() << "Made it here " << IsCountSetExplicitly << " " << UP.Count << "\n";
  if (!UP.Count)
    return LoopUnrollResult::Unmodified;
  // Unroll factor (Count) must be less or equal to TripCount.
  if (TripCount && UP.Count > TripCount)
    UP.Count = TripCount;

  // Save loop properties before it is transformed.
  MDNode *OrigLoopID = L->getLoopID();
  errs() << UP.Count << " " << TripCount << " " << UP.Force << " " <<  UP.Runtime << " " << UP.AllowExpensiveTripCount << " "
       << UseUpperBound << " " << MaxOrZero << " " << TripMultiple << " " << UP.PeelCount << " " << UP.UnrollRemainder << " "
       << ForgetAllSCEV << "\n";
  // Unroll the loop.
  Loop *RemainderLoop = nullptr;
  LoopUnrollResult UnrollResult = UnrollLoop(
      L,
      {UP.Count, TripCount, UP.Force, UP.Runtime, UP.AllowExpensiveTripCount,
       UseUpperBound, MaxOrZero, TripMultiple, UP.PeelCount, UP.UnrollRemainder,
       ForgetAllSCEV},
      LI, &SE, &DT, &AC, &ORE, PreserveLCSSA, &RemainderLoop);
  if (UnrollResult == LoopUnrollResult::Unmodified)
    return LoopUnrollResult::Unmodified;

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
      return UnrollResult;
    }
  }

  // If loop has an unroll count pragma or unrolled by explicitly set count
  // mark loop as unrolled to prevent unrolling beyond that requested.
  // If the loop was peeled, we already "used up" the profile information
  // we had, so we don't want to unroll or peel again.
  if (UnrollResult != LoopUnrollResult::FullyUnrolled &&
      (IsCountSetExplicitly || (UP.PeelProfiledIterations && UP.PeelCount)))
    L->setLoopAlreadyUnrolled();

  return UnrollResult;
}



namespace {
struct ConvPass : public FunctionPass {
  static char ID;
  ConvPass() : FunctionPass(ID) {}

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
        //if (UnrollResult == LoopUnrollResult::Unmodified)
        //    return LoopUnrollResult::Unmodified;

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
	    errs() << nextBlock->getName() << "\n";
	    lastInst = nextBlock->getTerminator();
            numSuc = lastInst->getNumSuccessors();
	}
	assert(numSuc==2);
	BasicBlock *finalUnroll = lastInst->getSuccessor(0);
	BasicBlock *exitLatch = lastInst->getSuccessor(1);
        errs() << "Stopped at " << lastInst->getOpcodeName() << " " << finalUnroll->getName() << " " << exitLatch->getName() << "\n";
        

	lastInst->eraseFromParent();
	BranchInst *uncondBrUnroll = BranchInst::Create(finalUnroll, nextBlock);
        Instruction *unreachInst = finalUnroll->getTerminator();	
      	unreachInst->eraseFromParent();
	BranchInst *uncondBrLatch = BranchInst::Create(exitLatch, finalUnroll);
       break; 
    }


    
    return false;
  }
}; // end of struct ConvPass
}  // end of anonymous namespace

char ConvPass::ID = 0;
static RegisterPass<ConvPass> X("convpass", "Convolution Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

