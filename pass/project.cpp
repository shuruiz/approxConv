// 583 final project 
// approximate convolutions 

#include "llvm/Transforms/Scalar/LICM.h"
#include "llvm/ADT/SetOperations.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasSetTracker.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CaptureTracking.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/GuardUtils.h"
#include "llvm/Analysis/Loads.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/IR/PredIteratorCache.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/SSAUpdater.h"
#include <algorithm>
#include <utility>
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"

#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "vector"

using namespace llvm;

namespace {
struct ConvPass : public FunctionPass {
  static char ID;
  ConvPass() : FunctionPass(ID) {}


  // Specify the list of analysis passes that will be used inside your pass.
  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
    AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
  }

  bool runOnFunction(Function &F) override {
    BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();

 //    Value* L = lhs_codegen_elements.back();
	// Value* R = rhs_codegen_elements.back();
    
    //%s = add %a, %b

    //matrix multiplication, 
    

	// 1. get the value for addition (in input file)

		// void multiply(int mat1[][N],
		//               int mat2[][N],
		//               int res[][N])
		// {
		//     int i, j, k;
		//     for (i = 0; i < N; i++) {
		//         for (j = 0; j < N; j++) {
		//             res[i][j] = 0;
		//             for (k = 0; k < N; k++)
		//                 res[i][j] = res[i][j] + mat1[i][k] * mat2[k][j]; // instruction I
		//         }
		//     }
		// }
        ////// call the function 

		//     int main()
		// {
		//     int i, j;
		//     int res[N][N]; // To store result
		//     int mat1[N][N] = { { 1, 1, 1, 1 },
		//                        { 2, 2, 2, 2 },
		//                        { 3, 3, 3, 3 },
		//                        { 4, 4, 4, 4 } };
		 
		//     int mat2[N][N] = { { 0.01, 1, 1, 1 },
		//                        { 2, 2, 2, 2 },
		//                        { 3, 3, 3, 3 },
		//                        { 4, 4, 4, 4 } };

    	 // 0.01*1 + 2*2+... 
		 
		//     multiply(mat1, mat2, res);
		 
		//     cout << "Result matrix is \n";
		//     for (i = 0; i < N; i++) {
		//         for (j = 0; j < N; j++)
		//             cout << res[i][j] << " ";
		//         cout << "\n";
		//     }
		 
		//     return 0;
		// }




	// 2. the pass only takes care of two addition (in this file, targeting instruction I above)

	float threshold=0.05; 

    // exam before the for loop 


	//run on loop?  http://lists.llvm.org/pipermail/llvm-dev/2016-April/098692.html
	Instruction * i = <instructionos that store the add operationo>;
	Value * Va = i->getOperand(0);
	Value * Vb =i->getOperand(1)
	if(*Va<= threshold || *Vb<=threshold):
		// skip the step 
		continue 

	// printf(*L, *R);


    return false;
  }
}; // end of struct ConvPass
}  // end of anonymous namespace

char ConvPass::ID = 0;
static RegisterPass<ConvPass> X("ConvPass", "Convolution Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
