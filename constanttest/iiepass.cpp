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

// Identity Instruction Elimination
// Pass that removes add instructions that add 0
struct IIEPass : public FunctionPass {
  static char ID;
  IIEPass() : FunctionPass(ID) {}


  // Specify the list of analysis passes that will be used inside your pass.
  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
    AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
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



  bool runOnFunction(Function &F) override {
    BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
	
	bool modified = false;

	errs() << "Running function\n";

	for (BasicBlock &BB : F){
		errs() << "New bb\n";
		for (auto instItr = BB.begin(); instItr != BB.end();){
			Instruction& I = *instItr;
			++instItr;
			errs() << "New inst " << I << "\n";
			if (I.getOpcode() == Instruction::Add) {
				errs() << "Found an add instruction\n";
				if ( ConstantInt *CI = dyn_cast<ConstantInt>( I.getOperand(0) )) {
					// if the first operand is constant
					if (CI->isZero()) {
						modified |= true;
						removeAndFixInst(I, 1);
						errs() << "out of remove func\n";
					}
					errs() << "out of zero if\n";
				} else if ( ConstantInt *CI = dyn_cast<ConstantInt>( I.getOperand(1) )) {
					// if the second operand is constant
					if (CI->isZero()) {
						modified |= true;
						removeAndFixInst(I, 0);		
					}
				}
				errs() << "out of constantInt check\n";
			}
			errs() << "out of get opcode\n";
		}
		errs() << "out of instruction loop\n";
	}
	errs() << "out of bb loop\n";
    
    return modified;
  }
}; // end of struct CTPass 
}  // end of anonymous namespace

char IIEPass::ID = 0;
static RegisterPass<IIEPass> X("iiepass", "Identity Instruction Elimination Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

