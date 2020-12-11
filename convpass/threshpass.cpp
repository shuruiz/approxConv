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
#include "llvm/IR/Constants.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/Transforms/Utils/FunctionComparator.h"

#include <vector>
#include <algorithm>

using namespace llvm;



namespace {
struct ThreshPass : public FunctionPass {
  static char ID;
  ThreshPass() : FunctionPass(ID) {}


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
  
  


  // takes an instruction with a const operand, removes the instruction
  // and fixes the dependencies for the non-const operand
  void removeAndFixInst( Instruction &I, int nonConstOperandIndex ) {
  	Value* nonConstOperand = I.getOperand(nonConstOperandIndex);
	
	errs() << *nonConstOperand << "\n";
	errs() << I << "\n";
	// replace all uses of the removed instruction with the operands source
	I.replaceAllUsesWith(nonConstOperand);
	I.eraseFromParent();

	errs() << "Done erasing\n";
  }




  float threshold = 10;
  bool runOnFunction(Function &F) override {
    //BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    //BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    
    bool modified = false;
	for (auto &BB : F) {
		for (auto &I : BB) {		
			if (I.getOpcode() == Instruction::FMul) {
				if (ConstantFP *CI = dyn_cast<ConstantFP>(I.getOperand(0))) {
					if( CI->getValueAPF().compare( APFloat(threshold)) == APFloat::cmpLessThan) {
						modified |= true;
						removeAndFixInst(I, 1);
					}
				} else if (ConstantFP *CI = dyn_cast<ConstantFP>(I.getOperand(1))) {
					if( CI->getValueAPF().compare( APFloat(threshold) ==  APFloat::cmpLessThan) {
						modified |= true;
						removeAndFixInstr(I,0);
					}
				}
			}
		}
	}
	


    
    return modified;
  }
}; // end of struct ConvPass
}  // end of anonymous namespace

char ThreshPass::ID = 0;
static RegisterPass<ThreshPass> X("threshpass", "Thresholding Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */)

