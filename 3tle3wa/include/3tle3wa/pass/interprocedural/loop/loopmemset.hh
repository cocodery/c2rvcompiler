#pragma once

#include <set>
#include <utility>

#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/function/structure/loop.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/value/baseValue.hh"
#include "3tle3wa/ir/value/use.hh"
#include "3tle3wa/pass/analysis/structure/structure.hh"
#include "3tle3wa/pass/interprocedural/loop/loopunrolling.hh"

// TODO:finish MemsetArray
// called by LoopTreeTraversal in line 95 in loopmemset.cpp
typedef std::pair<BaseValuePtr, BaseValuePtr> ArrayAndInit;
namespace LoopMemset {

ArrayAndInit GetArrayInit(Loop *);
bool FindInst(InstPtr, CfgNodePtr);
bool CheckLoopStep(Loop *);
void LoopTreeTraversal(Loop *, BaseValuePtr &, BaseValuePtr &, int &);
void LoopMemset(NormalFuncPtr);
void MemsetArray(BaseValuePtr, BaseValuePtr);
void RemoveLoop(Loop *);
}  // namespace LoopMemset