#pragma once

#include <list>
#include <map>

#include "3tle3wa/ir/IR.hh"

namespace SCCP {

struct LatticeAttr {
    enum LatticeType {
        Undefine,
        Constant,
        NotAConstant,
    };

    LatticeType type;
    BaseValuePtr value;

    LatticeAttr();
    LatticeAttr(LatticeType, BaseValuePtr);

    inline bool IsUndefine() const;
    inline bool IsConstant() const;
    inline bool IsNotAConstant() const;

    inline BaseValuePtr GetValue() const;

    bool operator==(const LatticeAttr &rhs);
    bool operator!=(const LatticeAttr &rhs);
};

struct ExcutedStatus {
    enum ExcutedType {
        UnKnown,
        False,
        True,
    };

    ExcutedType excutable;

    ExcutedStatus();
    ExcutedStatus(ExcutedType);

    inline bool IsUnKnown() const;
    inline bool IsFalse() const;
    inline bool IsTrue() const;
};

inline LatticeAttr CreateConstant(BaseValuePtr);
inline LatticeAttr CreateNotAConstant();

LatticeAttr Meet(const LatticeAttr &, const LatticeAttr &);

static std::list<CfgNodePtr> CFGWorkList;
static std::list<InstPtr> SSAWorkList;

static std::map<CfgNodePtr, ExcutedStatus> ExcutedMap;
static std::map<BaseValuePtr, LatticeAttr> ValueMap;

LatticeAttr GetLatticeAttr(BaseValuePtr);
void SetLatticeAttr(BaseValuePtr, LatticeAttr);

bool EvaluateOnInst(InstPtr);

void SCCP(NormalFuncPtr);

}  // namespace SCCP