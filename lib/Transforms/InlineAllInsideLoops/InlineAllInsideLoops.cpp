//===- InlineAllInsideLoops.cpp -                                ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements a custom inliner that inlines all calls into a on the
// command line given function.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "inline"
#include "llvm/Transforms/IPO.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/InlineCost.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Transforms/IPO/InlinerPass.h"
#include "llvm/Support/CommandLine.h"

#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static cl::opt<std::string>
InliningFunction("inline-only-in-function",
                       cl::desc("Inline all function calls inside of the given function"),
                       cl::Hidden, cl::init(""));

namespace {

/// \brief Inliner pass which only handles "always inline" functions.
class AllInsideLoopInliner : public Inliner {
  InlineCostAnalysis *ICA;

public:
  // Use extremely low threshold.
  AllInsideLoopInliner() : Inliner(ID, -2000000000, /*InsertLifetime*/ true), ICA(0) {
  }

  AllInsideLoopInliner(bool InsertLifetime)
      : Inliner(ID, -2000000000, InsertLifetime), ICA(0) {
  }

  static char ID; // Pass identification, replacement for typeid

  virtual InlineCost getInlineCost(CallSite CS);

  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual bool runOnSCC(CallGraphSCC &SCC);

  using llvm::Pass::doFinalization;
  virtual bool doFinalization(CallGraph &CG) {
    return removeDeadFunctions(CG, /*AlwaysInlineOnly=*/ true);
  }
};

}

static RegisterPass<AllInsideLoopInliner>
 Y("all-inline", "all inliner");

char AllInsideLoopInliner::ID = 0;

InlineCost AllInsideLoopInliner::getInlineCost(CallSite CS) {
  Function *Callee = CS.getCalledFunction();
  Function *Caller = CS.getCaller();

  if (Caller->getName().equals(InliningFunction)) {
    if (Callee && !Callee->isDeclaration() && ICA->isInlineViable(*Callee)) {
      errs() << "inlineing '" << Callee->getName() << " into " << Caller->getName() << "'\n";
      return InlineCost::getAlways();
    }
  }
  return InlineCost::getNever();
}

bool AllInsideLoopInliner::runOnSCC(CallGraphSCC &SCC) {
  ICA = &getAnalysis<InlineCostAnalysis>();
  return Inliner::runOnSCC(SCC);
}

void AllInsideLoopInliner::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<InlineCostAnalysis>();
  Inliner::getAnalysisUsage(AU);
}
