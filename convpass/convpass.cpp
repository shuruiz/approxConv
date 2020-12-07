#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"

#include "vector"

using namespace llvm;

namespace {
struct ConvPass : public FunctionPass {
  static char ID;
  ConvPass() : FunctionPass(ID) {}


  // Specify the list of analysis passes that will be used inside your pass.
  void getAnalysisUsage(AnalysisUsage &AU) const {
    //AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
    //AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
    AU.addRequired<LoopInfoWrapperPass>(); 
  }
  
  
  bool runOnFunction(Function &F) override {
    //BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    //BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    
    errs() << F.getName() << "\n";
    int topLevelLoopCount = 0;
    for(LoopInfo::iterator i = LI.begin(); i != LI.end(); ++i){
        topLevelLoopCount += 1;
    }
    if(topLevelLoopCount!=1){
        return false;
    }

    
    
    std::vector<Loop*> convLoops;


    Loop *currLoop = *LI.begin();
    while(currLoop){
	convLoops.push_back(currLoop);
        int numSubLoops = currLoop->getSubLoops().size();
        if(numSubLoops>1){
            return false;
	}
	else if (numSubLoops==1) {
            currLoop = currLoop->getSubLoops().front();
	}
	else {
            currLoop = NULL;
	}
    }

    errs() << convLoops.size() << "\n";
    
    errs() << "\n";
    return false;
  }
}; // end of struct ConvPass
}  // end of anonymous namespace

char ConvPass::ID = 0;
static RegisterPass<ConvPass> X("convpass", "Convolution Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

