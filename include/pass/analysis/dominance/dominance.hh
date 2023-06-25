#pragma once

#include <list>
#include <queue>

#include "ir/IR.hh"

namespace Dominance {
void ComputeDominanceInfo(CfgNodePtr, CfgNodeList);
void ComputeDominanceFrontier(CfgNodeList);

void DominanceAnalysis(NormalFuncPtr);
}  // namespace Dominance
