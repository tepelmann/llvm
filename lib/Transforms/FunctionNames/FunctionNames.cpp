//===- FunctionNames.cpp - Pass to print all function names" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Pass which prints all function names.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "function-names"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

STATISTIC(FunctionCounter, "Counts number of functions");

namespace {
  struct FunctionNames : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    FunctionNames() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      ++FunctionCounter;
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }
  };
}

char FunctionNames::ID = 0;
static RegisterPass<FunctionNames>
Y("function-names", "Function Names Pass: prints a list of all functions");
