#pragma once

#include <algorithm>
#include <utility>

#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/function/loop.hh"

using InvariantsInBlocks = std::vector<std::pair<CtrlFlowGraphNode *, std::set<InstPtr>>>;

namespace LoopInvariant {
void LoopInvariant(NormalFuncPtr);
void InvariantMotion(Loop *);
bool IsInSubloop(Loop *, CtrlFlowGraphNode *);
InvariantsInBlocks FindInvariant(Loop *);
}  // namespace LoopInvariant