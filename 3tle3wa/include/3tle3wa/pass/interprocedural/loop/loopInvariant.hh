#pragma once

#include <utility>

#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/function/loop.hh"

using InvariantsInBlocks = std::vector<std::pair<CfgNodePtr, std::set<VariablePtr>>>;

namespace LoopInvariant {
void LoopInvariant(NormalFuncPtr);
void InvariantMotion(Loop *);
InvariantsInBlocks FindInvariant(Loop *);
}  // namespace LoopInvariant