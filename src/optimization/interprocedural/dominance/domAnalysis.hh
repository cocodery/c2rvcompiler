#pragma once

#include <list>
#include <queue>

#include "IR.hh"

namespace DominanceAnalysis {
void ComputeDominanceInfo(CfgNodePtr, CfgNodeList);
void ComputeDominanceFrontier(CfgNodeList);
}  // namespace DominanceAnalysis
