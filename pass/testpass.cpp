// #include "llvm/Pass.h"
// #include "llvm/IR/Function.h"
// #include "llvm/Support/raw_ostream.h"
// #include "llvm/Analysis/BlockFrequencyInfo.h"
// #include "llvm/Analysis/BranchProbabilityInfo.h"
// #include "llvm/Support/Format.h"

// using namespace llvm;
// namespace {
// //skeleton code
// struct test: public FunctionPass {
// 	static char ID;
// 	test() : FunctionPass(ID) {}
// 	// Specify the list of analysis passes that will be used inside your pass.
// 	void getAnalysisUsage(AnalysisUsage &AU) const {
// 		AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
// 		AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
// 		// AU.addRequired<ProfileInfo>();
// 		// AU.addRequired<BranchProbabilityInfo>();
// 	}

// 	bool runOnFunction(Function &F) override{
// 		// errs() << "Hello: ";
// 		BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
// 	    BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
// 		// BProb = &getAnalysis<BranchProbabilityInfo>();
// 		// errs() << F.getName();
// 		float DynOpCounts=0, IALU=0, FALU=0,MEM=0,Bbranch=0,UBbranch=0,Others=0, TotalBr=0; //print all 0 if the function is not executed.
// 		for (Function::iterator bb = F.begin(), bbe = F.end(); bb != bbe; bb++) { //iterate bbs
// 			 // example for the analysis pass usage.
// 			BasicBlock* block = &*bb;
			
// 			float branch = 0;
// 			for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; i++) { // iterate instructions
// 				uint64_t count = bfi.getBlockProfileCount(block).getValue();
			
// 				DynOpCounts +=count;
// 				errs()<<"get opcode\n";

// 				int opcode = i->getOpcode();
// 				errs()<<opcode<<"\n";
// 				if (opcode==14){
// 				Value *va = i->getOperand(0);
// 				Value *vb =i->getOperand(1);
// 				errs() << "value"<<*va<<*vb;
// 				errs()<<"pointer"<<va<<vb;
// 				//errs()<<"print as operand"<<va->printAsOperand(va);
// 				errs()<<"getname"<<va->getName()<<"\n";
// 				errs()<<"getType"<<va->getType()<<"\n";
// 				errs()<<"print"<<va<<"\n"<<"dump value\n";
// 				//ConstantInt* CI = dyn_cast<ConstantInt>(va);
// 				Constant * C = dyn_cast<Constant>(vb);
// 				//ConstantInt* C = dyn_cast<ConstantInt>(va);
// 				errs()<<"constant cast\n";
//     				ConstantFP *constfp_gv = llvm::dyn_cast<llvm::ConstantFP>(C);
//                             		errs()<<"if statement";
// 					float gv_fpval = (constfp_gv->getValueAPF()).convertToFloat();
//                            // Fvalue = constfp_gv;
//                             errs() << gv_fpval; // Value retrieved here
//                             // Collect Instruction to modify


                        
// 				//errs()<<"dump"<<va->dump();
// 				}
// 				if (opcode == 13 || opcode == 15 || opcode == 17 || opcode == 19 || opcode == 20 || opcode == 22 || (opcode<=30 && opcode>=25) || opcode==53 || opcode ==23 ){
// 					IALU +=count;
// 				}
				
// 				//float ALU:  fadd, fsub, fmul, fdiv, frem, fcmp
// 				else if (opcode==14 || opcode ==16 || opcode == 18 || opcode==21 || opcode==24 || opcode==54){
// 					FALU +=count;

// 				}
// 				/* Memory: alloca, load, store, getelementptr, fence, cmpxchg, atomicrmw
// 				// Memory operators... by Instruction.def
// 				 FIRST_MEMORY_INST(31)
// 				HANDLE_MEMORY_INST(31, Alloca, AllocaInst)  // Stack management
// 				HANDLE_MEMORY_INST(32, Load  , LoadInst  )  // Memory manipulation instrs
// 				HANDLE_MEMORY_INST(33, Store , StoreInst )
// 				HANDLE_MEMORY_INST(34, GetElementPtr, GetElementPtrInst)
// 				HANDLE_MEMORY_INST(35, Fence , FenceInst )
// 				HANDLE_MEMORY_INST(36, AtomicCmpXchg , AtomicCmpXchgInst )
// 				HANDLE_MEMORY_INST(37, AtomicRMW , AtomicRMWInst )
// 				  LAST_MEMORY_INST(37)
// 				  */
// 				else if (opcode<=37 && opcode>=31){
// 					MEM += count;
// 				}

// 				// Branch: br, switch, indirectbr
// 				// HANDLE_TERM_INST  ( 2, Br            , BranchInst)
// 				// HANDLE_TERM_INST  ( 3, Switch        , SwitchInst)
// 				// HANDLE_TERM_INST  ( 4, IndirectBr    , IndirectBrInst)
// 				else if (opcode ==2 || opcode ==3 || opcode==4){
// 					branch+=count;
// 					TotalBr+=count;
// 				}
// 				else Others +=count;
// 			}

// 			if (branch){
// 			for(Function::iterator c = F.begin(), ce = F.end(); c != ce; c++) {
// 				BasicBlock* cblock= &*c;
// 				if (bpi.getEdgeProbability(block,cblock) >= BranchProbability(4, 5)) {
// 					Bbranch+=branch;break;}
// 				}      
// 		    }
// 		    UBbranch+=branch; 
// 		}

// 		// errs() <<format("%f",IALU)<<',';
// 		// //print count
// 		// errs() <<format("%f", TotalBr);
// 		// errs() <<','<<format("%f",FALU) << ','<<format("%f",MEM)<<','<<format("%f",Bbranch)<<','<<format("%f",UBbranch)<<','<<format("%f",Others)<<'\n';


// 		errs() << F.getName()<<',';
// 		errs() <<format("%f",DynOpCounts)<<',';
// 		if (DynOpCounts==0){
// 			errs() <<0;
// 			errs() <<','<<0 << ','<<0<<','<<0<<','<<0<<','<<0<<'\n';
// 		}
// 		else {
// 			errs() <<format("%f",IALU/DynOpCounts);
// 			errs() <<','<<format("%f",FALU/DynOpCounts) << ','<<format("%f",MEM/DynOpCounts)<<','<<format("%f",Bbranch/DynOpCounts)<<','<<format("%f",UBbranch/DynOpCounts)<<','<<format("%f",Others/DynOpCounts)<<'\n';}
//     	return false;
// 	}

// };
// }

// char test::ID =0;
// static RegisterPass<test> X("test", "dynamic opcodes counter",false, false);


//////////////// getting float 

// #include "llvm/Pass.h"
// #include "llvm/IR/Function.h"
// #include "llvm/Support/raw_ostream.h"
// #include "llvm/IR/LegacyPassManager.h"
// #include "llvm/IR/InstrTypes.h"
// #include "llvm/Transforms/IPO/PassManagerBuilder.h"
// #include "llvm/IR/IRBuilder.h"
// #include "llvm/IR/Module.h"
// #include "llvm/Transforms/Utils/BasicBlockUtils.h"
// #include "llvm/DebugInfo/DWARF/DWARFDebugLine.h"
// #include "llvm/IR/DebugLoc.h"
// #include "llvm/IR/DebugInfo.h"
// using namespace llvm;

// /* StackOverflow : https://stackoverflow.com/questions/48212351/how-to-get-llvm-global-variable-constant-value* /
// /**Bernard Nongpoh */

// namespace {
//     class test : public ModulePass {


//     public:
//         static char ID;


//         test() : ModulePass(ID) {
//             srand (time(NULL));
//         }

//         virtual bool runOnModule(Module &M) {

//             // list to collect instruction
//             /*
//              * You cannot change an iterator while iterating over it
//             • To remove instructions or modify, first collect the instructions to remove/modify
//             •
//              *
//              * **/
//             // This are the list of load to delete
//             SmallVector<Instruction*,128> *WorkListLoad=new SmallVector<Instruction*,128>();
//             // This is the list of instruction to modify the source operand
//             SmallVector<Instruction*,128> *WorkListUserOfLoad=new SmallVector<Instruction*,128>();


//             for (auto gv_iter = M.global_begin();gv_iter != M.global_end(); gv_iter++) {
//                    /* GLOBAL DATA INFO*/
//                     GlobalVariable *gv = &*gv_iter;
//                     Constant *const_gv = gv->getInitializer();
//                     ConstantFP *Fvalue;
//                     if(!const_gv->isNullValue()) {

//                         if (ConstantFP *constfp_gv = llvm::dyn_cast<llvm::ConstantFP>(const_gv)) {
//                             float gv_fpval = (constfp_gv->getValueAPF()).convertToFloat();
//                             Fvalue = constfp_gv;
//                             errs() <<"float value"<< gv_fpval; // Value retrieved here
//                             // Collect Instruction to modify


//                         }

//                         for (auto user_of_gv: gv->users()) {
//                             // Collect in a worklist
//                             if (llvm::Instruction *instr_ld_gv = llvm::dyn_cast<Instruction>(user_of_gv)) {

//                                 if (LoadInst *loadInst = dyn_cast<LoadInst>(instr_ld_gv)) {

//                                     WorkListLoad->push_back(loadInst);
//                                     for (auto user_of_load:loadInst->users()) {
//                                         user_of_load->dump();
//                                         Instruction *instruction1 = dyn_cast<Instruction>(user_of_load);
//                                         instruction1->dump();
//                                         //instruction1->setOperand(0, Fvalue);
//                                         //instruction1->dump();
//                                         // if(Instruction *instruction1 = dyn_cast<Instruction>(user_of_load))
//                                         WorkListUserOfLoad->push_back(instruction1);
//                                         //instruction1->setOperand(0, Fvalue);
//                                         //instruction1->dump();
//                                     }

//                                 }
//                             }
//                         }


//                     // Modify Here
//                         while (!WorkListUserOfLoad->empty()) {
//                             Instruction *instruction = WorkListUserOfLoad->pop_back_val();
//                             instruction->setOperand(0, Fvalue);
//                             instruction->dump();
//                         }

//                         // Removing all loads that are used by the global variable
//                         while (!WorkListLoad->empty()) {
//                             Instruction *instruction = WorkListLoad->pop_back_val();
//                             instruction->eraseFromParent();
//                         }



//                     }
//                 }








//              return true;
//     }
//   };
// }

// char test::ID = 0;


// static RegisterPass<test> F0("test", "Constant Replacement Pass "
//                                          , false,true);


///// constant propagation 

#define DEBUG_TYPE "constprop"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Pass.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include <set>
using namespace llvm;

STATISTIC(NumInstKilled, "Number of instructions killed");

namespace {
  struct test : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    test() : FunctionPass(ID) {
      initializeConstantPropagationPass(*PassRegistry::getPassRegistry());
    }

    bool runOnFunction(Function &F);

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesCFG();
      AU.addRequired<TargetLibraryInfo>();
    }
  };
}

char ConstantPropagation::ID = 0;
INITIALIZE_PASS_BEGIN(test, "test",
                "Simple constant propagation", false, false)
INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfo)
INITIALIZE_PASS_END(test, "test",
                "Simple constant propagation", false, false)

FunctionPass *llvm::createConstantPropagationPass() {
  return new ConstantPropagation();
}

bool ConstantPropagation::runOnFunction(Function &F) {
  // Initialize the worklist to all of the instructions ready to process...
  std::set<Instruction*> WorkList;
  for(inst_iterator i = inst_begin(F), e = inst_end(F); i != e; ++i) {
      WorkList.insert(&*i);
  }
  bool Changed = false;
  DataLayout *TD = getAnalysisIfAvailable<DataLayout>();
  TargetLibraryInfo *TLI = &getAnalysis<TargetLibraryInfo>();

  while (!WorkList.empty()) {
    Instruction *I = *WorkList.begin();
    WorkList.erase(WorkList.begin());    // Get an element from the worklist...

    if (!I->use_empty())                 // Don't muck with dead instructions...
      if (Constant *C = ConstantFoldInstruction(I, TD, TLI)) {
        // Add all of the users of this instruction to the worklist, they might
        // be constant propagatable now...
        if (ConstantFP *constfp_gv = llvm::dyn_cast<llvm::ConstantFP>(const_gv)) {
                float gv_fpval = (constfp_gv->getValueAPF()).convertToFloat();
                Fvalue = constfp_gv;
                errs() <<"float value"<< gv_fpval; // Value retrieved here

                }

        for (Value::use_iterator UI = I->use_begin(), UE = I->use_end();
             UI != UE; ++UI)
          WorkList.insert(cast<Instruction>(*UI));

        // Replace all of the uses of a variable with uses of the constant.
        I->replaceAllUsesWith(C);

        // Remove the dead instruction.
        WorkList.erase(I);
        I->eraseFromParent();

        // We made a change to the function...
        Changed = true;
        ++NumInstKilled;
      }
  }
  return Changed;
}