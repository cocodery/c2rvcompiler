#pragma once

#include <list>
#include <queue>
#include <set>

#include "ir/IR.hh"

namespace DCE {
void EliminateUselessCode(NormalFuncPtr);
void EliminateUselessControlFlow(NormalFuncPtr);
void EliminateUnreachableCode(NormalFuncPtr);
}  // namespace DCE