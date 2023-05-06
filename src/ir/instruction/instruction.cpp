#include "instruction.hh"

//===-----------------------------------------------------------===//
//                     Instruction Implementation
//===-----------------------------------------------------------===//

size_t Instruction::inst_idx = 1;

Instruction::Instruction(CfgNodePtr _parent) : idx(inst_idx++), parent(_parent) {}

const size_t Instruction::GetInstIdx() const { return idx; }
const CfgNodePtr Instruction::GetParent() const { return parent; }
void Instruction::SetParent(CfgNodePtr node) { parent = node; }
void Instruction::ClearParent() { SetParent(nullptr); }

bool Instruction::IsTwoOprandInst() const { return false; }
bool Instruction::IsOneOprandInst() const { return false; }

bool Instruction::IsReturnInst() const { return false; }
bool Instruction::IsJumpInst() const { return false; }
bool Instruction::IsBranchInst() const { return false; }

bool Instruction::IsAllocaInst() const { return false; }
bool Instruction::IsLoadInst() const { return false; }
bool Instruction::IsStoreInst() const { return false; }
bool Instruction::IsGepInst() const { return false; }

bool Instruction::IsCallInst() const { return false; }
bool Instruction::IsPhiInst() const { return false; }

void Instruction::ReplaceTarget(CfgNodePtr, CfgNodePtr) { return; }

bool Instruction::IsCriticalOperation() const {
    return IsStoreInst() || IsCallInst() || IsReturnInst() || IsBranchInst();
}

//===-----------------------------------------------------------===//
//                     UnaryInstruction Implementation
//===-----------------------------------------------------------===//

UnaryInstruction::UnaryInstruction(VariablePtr _res, BaseValuePtr _opr, CfgNodePtr node)
    : result(_res), oprand(_opr), Instruction(node) {}

BaseValuePtr UnaryInstruction::GetResult() const { return result; }
BaseValuePtr UnaryInstruction::GetOprand() const { return oprand; }

bool UnaryInstruction::IsOneOprandInst() const { return true; }

void UnaryInstruction::RemoveResParent() { result->SetParent(nullptr); }

bool UnaryInstruction::ReplaceSRC(BaseValuePtr replacee, BaseValuePtr replacer) {
    if (replacee == oprand) {
        oprand = replacer;
        return true;
    }
    return false;
}

const BaseValueList UnaryInstruction::UsedValue() { return BaseValueList({oprand}); }

//===-----------------------------------------------------------===//
//                     BinaryInstruction Implementation
//===-----------------------------------------------------------===//

BinaryInstruction::BinaryInstruction(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs,
                                     CfgNodePtr node)
    : result(_res), op(_op), lhs(_lhs), rhs(_rhs), Instruction(node) {}

BaseValuePtr BinaryInstruction::GetResult() const { return result; }
BaseValuePtr BinaryInstruction::GetLHS() const { return lhs; }
BaseValuePtr BinaryInstruction::GetRHS() const { return rhs; }

void BinaryInstruction::RemoveResParent() { result->SetParent(nullptr); }

bool BinaryInstruction::ReplaceSRC(BaseValuePtr replacee, BaseValuePtr replacer) {
    bool ret = false;
    if (replacee == lhs) {
        lhs = replacer;
        ret = true;
    }
    if (replacee == rhs) {
        rhs = replacer;
        ret = true;
    }
    return ret;
}

bool BinaryInstruction::IsTwoOprandInst() const { return true; }

const BaseValueList BinaryInstruction::UsedValue() { return BaseValueList({lhs, rhs}); }

//===-----------------------------------------------------------===//
//                     ReplaceSRC Implementation
//===-----------------------------------------------------------===//

void ReplaceSRC(BaseValuePtr replacee, BaseValuePtr replacer) {
    auto &&use_list = replacee->GetUserList();
    std::for_each(use_list.begin(), use_list.end(), [&replacee, &replacer](const auto &inst) {
        if (inst->ReplaceSRC(replacee, replacer)) {
            replacer->InsertUser(inst);
        }
    });
}

//===-----------------------------------------------------------===//
//                     RemoveInst Implementation
//===-----------------------------------------------------------===//

void RemoveInst(InstPtr inst) {
    auto &&used_value = inst->UsedValue();
    std::for_each(used_value.begin(), used_value.end(), [&inst](BaseValuePtr value) { value->RemoveUser(inst); });
    inst->RemoveResParent();
}