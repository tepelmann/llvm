//===- FindLoops.cpp - Pass to print all functions with loops" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Pass which prints the name of all function which have a loop in it.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "find-loops"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

STATISTIC(FunctionWithLoopsCounter, "Counts number of functions with loops");

namespace {
  struct FindLoops : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    FindLoops() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      LoopInfo &LI = getAnalysis<LoopInfo>();

      if (!LI.empty()){
        ++FunctionWithLoopsCounter;
        errs().write_escaped(F.getName()) << '\n';
      }

      return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
      AU.addRequired<LoopInfo>();
    }
  };
}

char FindLoops::ID = 0;
static RegisterPass<FindLoops>
Y("find-loops", "Find Loops Pass: prints a list of all functions with loops");
