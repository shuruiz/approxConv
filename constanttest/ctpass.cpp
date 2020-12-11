#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Transforms/Scalar/SROA.h"

#include "vector"

using namespace llvm;

namespace {
struct CTPass : public FunctionPass {
  static char ID;
  CTPass() : FunctionPass(ID) {}


  // Specify the list of analysis passes that will be used inside your pass.
  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
    AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
  	//AU.addRequired<llvm::sroa::SROALegacyPass>();
  }

  bool runOnFunction(Function &F) override {
    BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
	
	AnalysisManager<Function> FAM;
//	llvm:SROA run(F, FAM);
    
    return true;
  }
}; // end of struct CTPass 
}  // end of anonymous namespace

char CTPass::ID = 0;
static RegisterPass<CTPass> X("ctpass", "Constant Test Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

