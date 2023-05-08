#pragma once

#include <list>
#include <map>
#include <queue>

#include "IR.hh"

namespace DeadCodeElimination {
void EliminateUselessCode(NormalFuncPtr);
void EliminateUselessControlFlow(NormalFuncPtr);
void EliminateUnreachableCode(NormalFuncPtr);
}  // namespace DeadCodeElimination