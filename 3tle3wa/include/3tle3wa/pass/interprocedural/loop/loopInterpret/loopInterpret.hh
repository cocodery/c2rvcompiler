#pragma once

#include <algorithm>
#include <cstddef>
#include <list>
#include <unordered_map>
#include <utility>

#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/function/function.hh"
#include "3tle3wa/ir/function/structure/loop.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/value/use.hh"

namespace LoopInterpret {

struct Lattice {
    enum State {
        Undefine,  // Top
        Constant,  // Constant
        Variable,  // Bottom
    };
    State state;
    BaseValuePtr value;

    Lattice() : state(Undefine), value(nullptr) {}
    Lattice(State _state, BaseValuePtr _value) : state(_state), value(_value) {}

    inline bool IsUndefine() const;
    inline bool IsConstant() const;
    inline bool IsVariable() const;

    inline BaseValuePtr GetValue() const;

    bool operator!=(const Lattice &rhs);
};

inline Lattice CreateConstant(BaseValuePtr);

static std::unordered_map<BaseValuePtr, Lattice> LatticeMap;

void Interpret(Loop *);
void LoopInterpret(NormalFuncPtr);
}  // namespace LoopInterpret