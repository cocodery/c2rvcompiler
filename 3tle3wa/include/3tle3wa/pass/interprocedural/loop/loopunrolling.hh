#pragma once

#include <variant>

#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/function/structure/loop.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/value/baseValue.hh"
#include "3tle3wa/ir/value/use.hh"

typedef std::vector<BaseValuePtr> Operands;
namespace LoopUnrolling {
void LoopUnrolling(NormalFuncPtr);
void ExpandLoop(Loop *);
int LoopTime(Loop *);
int ConstCheck(InstPtr);
OpCode FlipComparisonOp(OpCode);
Operands InstOperandsInVector(InstPtr);
}  // namespace LoopUnrolling