#pragma once

#include <list>
#include <queue>

#include "IR.hh"

namespace Dominance {
void ComputeDominanceInfo(CfgNodePtr, CfgNodeList);
void ComputeDominanceFrontier(CfgNodeList);

void DominanceAnalysis(NormalFuncPtr);
}  // namespace Dominance
