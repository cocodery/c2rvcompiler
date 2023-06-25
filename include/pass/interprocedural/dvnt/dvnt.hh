#pragma once

#include <list>
#include <map>
#include <unordered_map>

#include "ir/IR.hh"

namespace GVN {

struct VNExpr {
    OpCode opcode;
    BaseValuePtr lhs;
    BaseValuePtr rhs;

    bool operator==(const VNExpr &) const;
};

struct VNExprHasher {
    size_t operator()(const VNExpr &) const;
};

using VNTable = std::unordered_map<VNExpr, BaseValuePtr, VNExprHasher>;

struct VNScope {
    VNTable map;
    VNScope *outer;

    VNScope(VNScope *);

    BaseValuePtr Get(BinaryInstPtr);
    void Set(BinaryInstPtr);
};

using ValueNumber = std::map<BaseValuePtr, BaseValuePtr>;

static ValueNumber VN;

BaseValuePtr GetVN(BaseValuePtr);

bool IsMeaingLess(InstPtr);
bool IsRedundant(CfgNodePtr, InstPtr);

bool IsPhiOprandSame(InstPtr);

void AdjustPhiInst(CfgNodePtr, PhiInstPtr);

void DoDVNT(CfgNodePtr, VNScope *);
void DVNT(NormalFuncPtr);
}  // namespace GVN
