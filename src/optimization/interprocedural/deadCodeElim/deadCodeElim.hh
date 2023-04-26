#pragma once

#include <list>
#include <map>
#include <queue>

#include "IR.hh"

namespace DeadCodeElimination {
// void EliminateUselessCode();
// void EliminateUselessControlFlow();
void EliminateUnreachableCode(CfgNodePtr, CfgNodeList);
}  // namespace DeadCodeElimination