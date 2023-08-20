#pragma once

#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/instruction/instruction.hh"

namespace RecursionOpt {

static std::unordered_map<BaseValue *, BaseValuePtr> value_map;

void DoTailRec2Loop(NormalFuncPtr &, SymbolTable &glb_table);

void TailRec2Loop(NormalFuncPtr &);
BaseValuePtr InstCopy(Instruction *, CfgNodePtr &);
}  // namespace RecursionOpt