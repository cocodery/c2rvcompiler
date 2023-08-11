#include "3tle3wa/pass/interprocedural/loop/loopInterpret/loopInterpret.hh"

#include <cassert>
#include <queue>
#include <unordered_map>

#include "3tle3wa/backend/rl/RLGen.hh"
#include "3tle3wa/ir/function/basicblock.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/otherInst.hh"
#include "3tle3wa/ir/value/baseValue.hh"

bool LoopInterpret::Lattice::IsUndefine() const { return state == State::Undefine; }
bool LoopInterpret::Lattice::IsConstant() const { return state == State::Constant; }
bool LoopInterpret::Lattice::IsVariable() const { return state == State::Variable; }

BaseValuePtr LoopInterpret::Lattice::GetValue() const {
    assert(state == Constant && value->IsConstant());
    return value;
}

bool LoopInterpret::Lattice::operator!=(const Lattice &rhs) { return (state != rhs.state) || (value != rhs.value); }

LoopInterpret::Lattice LoopInterpret::CreateConstant(BaseValuePtr _value) { return Lattice(Lattice::Constant, _value); }

void LoopInterpret::Interpret(Loop *loop) {
    // mean cur-loop + its sub-loop are simple
    std::queue<CfgNodePtr> excuteStream;

    for (auto &&node : loop->GetEntireStructure()) {
        auto &&dominators = node->GetDominatorSet();
        for (auto &&inst : node->GetInstList()) {
            if (inst->IsPhiInst()) {
                PhiInst *phi_inst = static_cast<PhiInst *>(inst.get());
                for (auto &&[value, block] : phi_inst->GetDataList()) {
                    if (dominators.find(block) != dominators.end()) {
                        
                    }
                }
            }
        }
    }

    excuteStream.push(loop->cond_begin);
}

void LoopInterpret::LoopInterpret(NormalFuncPtr func) {
    assert(LatticeMap.size() == 0);

    for (auto &&sub_structure : func->loops->sub_structures) {
        auto &&sub_loop = static_cast<Loop *>(sub_structure);
        if (sub_loop->IsSimpleLoop()) {
            Interpret(sub_loop);
        }
    }
    LatticeMap.clear();
}
