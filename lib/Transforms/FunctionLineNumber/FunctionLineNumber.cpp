//===- FunctionLineNumber.cpp - Pass to print function line numbers" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Pass which prints the line number and the path to the source code file
// where the function is defined.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "function-line-number"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include <llvm/Support/CommandLine.h>

#include "llvm/DebugInfo.h"

#include <llvm/IR/Function.h>
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
using namespace llvm;

static cl::opt<std::string>
FunctionName("function-name",
  cl::desc("The function of which the information should be shown."),
  cl::Hidden, cl::init(""));

namespace {
  struct FunctionLineNumber : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    FunctionLineNumber() : FunctionPass(ID) {
      if(!FunctionName.hasArgStr()) {
        report_fatal_error("function-name argument missing");
      }
    }

    static Value *getFunctionInfo(Function &F) {
      const Module *M = F.getParent();
      NamedMDNode *NMD = M->getNamedMetadata("llvm.dbg.cu");
      if (!NMD)
        return 0;
      
      for (unsigned i = 0, e = NMD->getNumOperands(); i != e; i++) {
        DIDescriptor CompileUnit(cast<MDNode>(NMD->getOperand(i)));
        unsigned subProgPos = CompileUnit->getNumOperands()-3;
        DIDescriptor SubProgs(cast<MDNode>(CompileUnit->getOperand(subProgPos)));
        for (unsigned s = 0; s < SubProgs->getNumOperands(); s++) {
          Value* VV = cast<Value>(SubProgs->getOperand(s));
          if (isa<MDNode>(VV)) {
            DIDescriptor SubProg(cast<MDNode>(VV));
            if (DISubprogram(SubProg).getFunction() == &F)
              return SubProg;
          }
        }
      }
      return 0;
    }
    
    virtual bool runOnFunction(Function &F) {
      if(F.getName().equals(FunctionName.getValue())) {
        Value* dbg = getFunctionInfo(F);
        if (dbg) {
          DISubprogram dis(cast<MDNode>(dbg));
          outs() << dis.getDirectory() << "/" << dis.getFilename() << " " 
                 << dis.getLineNumber() << "\n";
        }
        else
          outs() << "Code has no debbuging information! Compile with -g.\n";
        
      }
      return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }
  };
}

char FunctionLineNumber::ID = 0;
static RegisterPass<FunctionLineNumber>
Y("function-line-number", "Function Line Number Pass: prints the line number "
  "and the path to the source code file where the function is defined.");
