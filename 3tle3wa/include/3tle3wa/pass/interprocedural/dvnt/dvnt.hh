#pragma once

#include <list>
#include <map>
#include <unordered_map>

#include "3tle3wa/ir/IR.hh"

namespace GVN {

struct BinVNExpr {
    OpCode opcode;
    BaseValue *lhs;
    BaseValue *rhs;

    bool operator==(const BinVNExpr &) const;
};

struct BinVNExprHasher {
    size_t operator()(const BinVNExpr &) const;
};

typedef std::unordered_map<BinVNExpr, BaseValuePtr, BinVNExprHasher> BinVNTable;

struct GepVNExpr {
    size_t off_size;
    BaseValue *base_addr;
    BaseValue *last_off;

    bool operator==(const GepVNExpr &) const;
};

struct GepVNExprHasher {
    size_t operator()(const GepVNExpr &) const;
};

typedef std::unordered_map<GepVNExpr, BaseValuePtr, GepVNExprHasher> GepVNTable;

struct LoadVNExpr {
    BaseValue *load_addr;

    bool operator==(const LoadVNExpr &) const;
};

struct LoadVNExprHasher {
    size_t operator()(const LoadVNExpr &) const;
};

typedef std::unordered_map<LoadVNExpr, BaseValuePtr, LoadVNExprHasher> LoadVNTable;

struct VNScope {
    BinVNTable bin_map;    // GVN
    GepVNTable gep_map;    // GVN
    LoadVNTable load_map;  // LVN

    VNScope *outer;

    VNScope(VNScope *);

    BaseValuePtr Get(InstPtr);
    void Set(InstPtr);
};

typedef std::unordered_map<BaseValuePtr, BaseValuePtr> ValueNumber;

static ValueNumber VN;

BaseValuePtr GetVN(BaseValuePtr);

bool IsMeaingLess(InstPtr);
bool IsRedundant(CfgNodePtr, InstPtr);

bool IsPhiOprandSame(InstPtr);

void AdjustPhiInst(CfgNodePtr, PhiInstPtr);

void DoDVNT(CfgNodePtr, VNScope *);
void DVNT(NormalFuncPtr, SymbolTable &);
}  // namespace GVN
