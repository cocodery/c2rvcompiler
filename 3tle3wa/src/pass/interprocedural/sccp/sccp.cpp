#include "3tle3wa/pass/interprocedural/sccp/sccp.hh"

#include <cassert>
#include <cstddef>

#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/value/baseValue.hh"

SCCP::LatticeAttr::LatticeAttr() : type(Undefine), value(nullptr) {}
SCCP::LatticeAttr::LatticeAttr(LatticeType _type, BaseValuePtr _value) : type(_type), value(_value) {}

SCCP::LatticeAttr SCCP::CreateConstant(BaseValuePtr _value) { return LatticeAttr(LatticeAttr::Constant, _value); }
SCCP::LatticeAttr SCCP::CreateNotAConstant() { return LatticeAttr(LatticeAttr::NotAConstant, nullptr); }

bool SCCP::LatticeAttr::IsUndefine() const { return type == LatticeType::Undefine; }
bool SCCP::LatticeAttr::IsConstant() const { return type == LatticeType::Constant; }
bool SCCP::LatticeAttr::IsNotAConstant() const { return type == LatticeType::NotAConstant; }

inline BaseValuePtr SCCP::LatticeAttr::GetValue() const {
    assert(type == LatticeAttr::Constant);
    return value;
}

bool SCCP::LatticeAttr::operator==(const LatticeAttr &rhs) { return (type == rhs.type) && (value == rhs.value); }
bool SCCP::LatticeAttr::operator!=(const LatticeAttr &rhs) { return !(*this == rhs); }

SCCP::ExcutedStatus::ExcutedStatus() : excutable(UnKnown) {}
SCCP::ExcutedStatus::ExcutedStatus(ExcutedType _type) : excutable(_type) {}

bool SCCP::ExcutedStatus::IsUnKnown() const { return excutable == UnKnown; }
bool SCCP::ExcutedStatus::IsFalse() const { return excutable == False; }
bool SCCP::ExcutedStatus::IsTrue() const { return excutable == True; }

SCCP::LatticeAttr SCCP::Meet(const LatticeAttr &lhs, const LatticeAttr &rhs) {
    if (lhs.type == LatticeAttr::Undefine) return rhs;
    if (rhs.type == LatticeAttr::Undefine) return lhs;
    if (lhs.type == LatticeAttr::NotAConstant || rhs.type == LatticeAttr::NotAConstant) {
        return SCCP::CreateNotAConstant();
    }
    assert(lhs.type == LatticeAttr::Constant && rhs.type == LatticeAttr::Constant);
    if (lhs.value == rhs.value) return lhs;
    return CreateNotAConstant();
}

SCCP::LatticeAttr SCCP::GetLatticeAttr(BaseValuePtr _value) {
    if (_value == nullptr) {
        return CreateNotAConstant();
    } else if (_value->IsConstant()) {
        return CreateConstant(_value);
    } else {
        return ValueMap[_value];
    }
}

void SCCP::SetLatticeAttr(BaseValuePtr _value, LatticeAttr _attr) {
    if (ValueMap.find(_value) == ValueMap.end()) {  // first set
        ValueMap.insert({_value, _attr});
    } else {
        auto &slot = ValueMap[_value];
        if (slot != _attr) {
            slot = _attr;
            if (std::find(SSAWorkList.begin(), SSAWorkList.end(), _value->GetParent()) != SSAWorkList.end()) {
                SSAWorkList.push_back(_value->GetParent());
            }
        }
    }
    return;
}

bool SCCP::EvaluateOnInst(InstPtr inst) {
    bool ret = false;

    // if (inst->IsJumpInst()) {
    //     auto jump_inst = std::static_pointer_cast<JumpInst>(inst);
    //     CFGWorkList.push_back(jump_inst->GetTarget());
    // } else if (inst->IsBranchInst()) {
    //     auto br_inst = std::static_pointer_cast<BranchInst>(inst);
    //     auto cond = br_inst->GetCondition();
    //     assert(cond->GetBaseType()->BoolType());

    //     if (cond->IsConstant()) {
    //         CFGWorkList.push_back(std::get<bool>(std::static_pointer_cast<Constant>(cond)->GetValue())
    //                                   ? br_inst->GetTrueTarget()
    //                                   : br_inst->GetFalseTarget());
    //     } else {
    //         CFGWorkList.push_back(br_inst->GetTrueTarget());
    //         CFGWorkList.push_back(br_inst->GetFalseTarget());
    //     }
    // } else if (inst->IsOneOprandInst()) {
    //     auto _attr = CreateNotAConstant();
    //     auto unary_inst = std::static_pointer_cast<UnaryInstruction>(inst);

    //     // auto [replacee, replacer] = unary_inst->DoFlod();

    //     if (replacee != nullptr && replacee != nullptr) {  // replace successful
    //         ret = true;
    //         // ReplaceSRC(replacee, replacer);
    //         assert(replacer->IsConstant());  // only constant result on unary-inst floding
    //     } else {
    //         SetLatticeAttr(unary_inst->GetResult(), _attr);
    //     }
    // } else if (inst->IsTwoOprandInst()) {
    //     auto _attr = CreateNotAConstant();
    //     auto bin_inst = std::static_pointer_cast<BinaryInstruction>(inst);
    //     assert(!GetLatticeAttr(bin_inst->GetLHS()).IsUndefine() && !GetLatticeAttr(bin_inst->GetRHS()).IsUndefine());

    //     // auto [replacee, replacer] = bin_inst->DoFlod();

    //     if (replacee != nullptr && replacer != nullptr) {  // replace successful
    //         ret = true;
    //         // ReplaceSRC(replacee, replacer);
    //         // replacer have been set in ValueMap when it first travelled
    //     } else {
    //         SetLatticeAttr(bin_inst->GetResult(), _attr);
    //     }
    // } else if (inst->IsPhiInst()) {
    //     auto phi_inst = std::static_pointer_cast<PhiInst>(inst);
    //     LatticeAttr _attr;  // Undefine

    //     auto &&ref_list = phi_inst->GetRefList();
    //     for (auto &&iter = ref_list.begin(); iter != ref_list.end();) {
    //         auto [_value, _node] = (*iter);

    //         auto excutable = ExcutedMap[_node];
    //         if (excutable.IsFalse()) {
    //             iter = ref_list.erase(iter);
    //         } else {
    //             if (excutable.IsTrue()) {
    //                 _attr = Meet(_attr, GetLatticeAttr(_value));
    //             }
    //             ++iter;
    //         }
    //     }
    //     if (ref_list.size() == 1) {
    //         ret = true;

    //         BaseValuePtr replacee = phi_inst->GetResult();
    //         BaseValuePtr replacer = ref_list.begin()->first;

    //         ReplaceSRC(replacee, replacer);
    //     } else {
    //         _attr = CreateNotAConstant();
    //         SetLatticeAttr(phi_inst->GetResult(), _attr);
    //     }
    // } else {
    //     auto result = inst->GetResult();
    //     if (result != nullptr) {
    //         if (result->IsConstant()) {
    //             SetLatticeAttr(result, CreateConstant(result));
    //         } else {
    //             SetLatticeAttr(result, CreateNotAConstant());
    //         }
    //     }
    // }
    return ret;
}

void SCCP::SCCP(NormalFuncPtr func) {
    for (auto param : func->GetParamList()) {
        ValueMap.insert({param, CreateNotAConstant()});
    }

    CFGWorkList.push_back(func->GetEntryNode());

    while (!CFGWorkList.empty() || !SSAWorkList.empty()) {
        if (!CFGWorkList.empty()) {
            auto node = CFGWorkList.back();
            CFGWorkList.pop_back();

            if (!ExcutedMap[node].IsTrue()) {
                ExcutedMap[node] = ExcutedStatus::True;

                auto &&inst_list = node->GetInstList();
                for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
                    bool ret = EvaluateOnInst((*iter));
                    if (ret) {
                        iter = inst_list.erase(iter);
                    } else {
                        ++iter;
                    }
                }
            }
        }
        // TODO
        // if (!SSAWorkList.empty()) {
        //     assert(false);
        //     auto inst = SSAWorkList.back();
        //     SSAWorkList.pop_back();

        //     if (ExcutedMap[inst->GetParent()].IsTrue()) {
        //         EvaluateOnInst(inst);
        //     }
        // }
    }

    ValueMap.clear();
    ExcutedMap.clear();
    return;
}
