
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
  void placeZeros( Instruction &I, int nonConstOperandIndex ) {
  	Value* nonConstOperand = I.getOperand(nonConstOperandIndex);
	
	// replace all uses of the removed instruction with the operands source
	LLVMContext context;
	Constant* zero = ConstantFP::get(I.getType(), 0);
	
    I.replaceAllUsesWith(zero);
	I.eraseFromParent();

  }


  void removeAndFixInst( Instruction &I, int nonConstOperandIndex ) {
  	Value* nonConstOperand = I.getOperand(nonConstOperandIndex);
	
	errs() << "Removing inst: " << I << "\n";
	// replace all uses of the removed instruction with the operands source
	I.replaceAllUsesWith(nonConstOperand);
	I.eraseFromParent();
  }

  float threshold = 10;
  bool runOnFunction(Function &F) override {
    //BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    //BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    
    bool modified = false;
	for (auto &BB : F) {
		for (auto instItr = BB.begin(); instItr != BB.end();) {
			Instruction &I = *instItr;
			++instItr;

			if (I.getOpcode() == Instruction::FMul) {
				if (ConstantFP *CI = dyn_cast<ConstantFP>(I.getOperand(0))) {
					if((CI->getValueAPF()).convertToFloat() <= threshold/* CI->getValueAPF().compare( APFloat(threshold)) == APFloat::cmpLessThan*/) {
						modified |= true;
						placeZeros(I, 1);
						errs() << CI->getValueAPF().convertToFloat() << "\n";
					}
				} else if (ConstantFP *CI = dyn_cast<ConstantFP>(I.getOperand(1))) {
					if( (CI->getValueAPF()).convertToFloat() <= threshold /*CI->getValueAPF().compare( APFloat(threshold)) ==  APFloat::cmpLessThan*/) {
						modified |= true;
						placeZeros(I,0);
						errs() << CI->getValueAPF().convertToFloat() << "\n";
					}
				}
			}
		}

		for (auto instItr = BB.begin(); instItr != BB.end();){
			Instruction& I = *instItr;
			++instItr;
			errs() << "New inst " << I << "\n";
			if (I.getOpcode() == Instruction::FAdd) {
				if ( ConstantFP *CI = dyn_cast<ConstantFP>( I.getOperand(0) )) {
					// if the first operand is constant
					if (CI->isZero()) {
						modified |= true;
						removeAndFixInst(I, 1);
					}
				} else if ( ConstantFP *CI = dyn_cast<ConstantFP>( I.getOperand(1) )) {
					// if the second operand is constant
					if (CI->isZero()) {
						modified |= true;
						removeAndFixInst(I, 0);		
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
                             false /* Analysis Pass */);

