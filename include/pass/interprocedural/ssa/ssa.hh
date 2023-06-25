#pragma once

#include <list>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "ir/IR.hh"
#include "ir/function/function.hh"

namespace SSA {

using ValueVector = std::vector<BaseValuePtr>;

struct RenameData;
using RenameDatePtr = std::shared_ptr<RenameData>;
struct RenameData {
    CfgNodePtr node;
    CfgNodePtr pred;
    ValueVector valuelist;

    RenameData(CfgNodePtr, CfgNodePtr, ValueVector);

    static RenameDatePtr CreatePtr(CfgNodePtr, CfgNodePtr, ValueVector);
};

void SSAConstruction(NormalFuncPtr);

void SSADestruction(NormalFuncPtr);
}  // namespace SSA