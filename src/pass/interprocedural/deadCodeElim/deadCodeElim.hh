#pragma once

#include <list>
#include <map>
#include <queue>

#include "IR.hh"

namespace DeadCodeElimination {
void EliminateUselessCode(CfgNodeList);
// void EliminateUselessControlFlow();
void EliminateUnreachableCode(CfgNodePtr, CfgNodeList);
}  // namespace DeadCodeElimination