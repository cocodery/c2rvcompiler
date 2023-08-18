#pragma once

#include "3tle3wa/ir/IR.hh"

namespace TailRec2Loop {

static std::unordered_map<BaseValue *, BaseValuePtr> value_map;

void DoTailRec2Loop(NormalFuncPtr &, SymbolTable &glb_table);

void TailRec2Loop(NormalFuncPtr &);
BaseValuePtr InstCopy(InstPtr &, CfgNodePtr &);
}  // namespace TailRec2Loop