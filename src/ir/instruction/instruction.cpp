#include "instruction.hh"

//===-----------------------------------------------------------===//
//                     Instruction Implementation
//===-----------------------------------------------------------===//

size_t Instruction::inst_idx = 1;

Instruction::Instruction(VariablePtr _res, OpCode _code, CfgNodePtr _parent)
    : idx(inst_idx++), result(_res), opcode(_code), parent(_parent) {}

const OpCode &Instruction::GetOpCode() const { return opcode; }
const size_t &Instruction::GetInstIdx() const { return idx; }
const CfgNodePtr &Instruction::GetParent() const { return parent; }
void Instruction::SetParent(CfgNodePtr node) { parent = node; }
void Instruction::ClearParent() { SetParent(nullptr); }

bool Instruction::IsTwoOprandInst() const { return (OP_ADD <= opcode && opcode <= OP_NEQ); }
bool Instruction::IsOneOprandInst() const { return (Load <= opcode && opcode <= Zext); }

bool Instruction::IsReturnInst() const { return opcode == Ret; }
bool Instruction::IsJumpInst() const { return opcode == Jump; }
bool Instruction::IsBranchInst() const { return opcode == Branch; }

bool Instruction::IsAllocaInst() const { return opcode == Alloca; }
bool Instruction::IsLoadInst() const { return opcode == Load; }
bool Instruction::IsStoreInst() const { return opcode == Store; }
bool Instruction::IsGepInst() const { return opcode == Gep; }

bool Instruction::IsCallInst() const { return opcode == Call; }
bool Instruction::IsPhiInst() const { return opcode == Phi; }

bool Instruction::IsCriticalOperation() const {
    return IsStoreInst() || IsCallInst() || IsReturnInst() || IsBranchInst();
}

bool Instruction::IsValueNumberInst() const { return IsTwoOprandInst() || IsGepInst(); }

VariablePtr Instruction::GetResult() const { return result; }

std::pair<BaseValuePtr, BaseValuePtr> Instruction::DoFlod() const { return {nullptr, nullptr}; }

void Instruction::ReplaceTarget(CfgNodePtr, CfgNodePtr) { return; }

//===-----------------------------------------------------------===//
//                     UnaryInstruction Implementation
//===-----------------------------------------------------------===//

UnaryInstruction::UnaryInstruction(VariablePtr _res, OpCode _op, BaseValuePtr _opr, CfgNodePtr node)
    : Instruction(_res, _op, node), oprand(_opr) {}

BaseValuePtr UnaryInstruction::GetOprand() const { return oprand; }

std::pair<BaseValuePtr, BaseValuePtr> UnaryInstruction::DoFlod() const {
    if (oprand->IsConstant()) {
        ATTR_TYPE _type;
        ConstType _value;

        auto constant = std::static_pointer_cast<Constant>(oprand);
        std::visit(
            [&_type, &_value, op = opcode](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                switch (op) {
                    case SiToFp:
                        assert((std::is_same_v<T, bool>) || (std::is_same_v<T, int32_t>));
                        _type = FLOAT;
                        _value = static_cast<float>(arg);
                        break;
                    case FpToSi:
                        assert((std::is_same_v<T, float>));
                        _type = INT32;
                        _value = static_cast<int32_t>(arg);
                        break;
                    case Zext:
                        assert((std::is_same_v<T, bool>));
                        _type = INT32;
                        _value = static_cast<int32_t>(arg);
                        break;
                    default:
                        assert(false);
                        break;
                }
            },
            constant->GetValue());

        ScalarTypePtr _stype = (_type == INT32)   ? type_const_int
                               : (_type == FLOAT) ? type_const_float
                                                  : type_const_bool;
        return {result, Constant::CreatePtr(_stype, _value)};
    }
    return {nullptr, nullptr};
}

void UnaryInstruction::RemoveResParent() { result->SetParent(nullptr); }

const BaseValueList UnaryInstruction::GetOprands() const { return BaseValueList({oprand}); }

//===-----------------------------------------------------------===//
//                     BinaryInstruction Implementation
//===-----------------------------------------------------------===//

BinaryInstruction::BinaryInstruction(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs,
                                     CfgNodePtr node)
    : Instruction(_res, _op, node), lhs(_lhs), rhs(_rhs) {}

BaseValuePtr BinaryInstruction::GetLHS() const { return lhs; }
BaseValuePtr BinaryInstruction::GetRHS() const { return rhs; }

bool BinaryInstruction::IsIBinaryInst() const { return false; }
bool BinaryInstruction::IsFBinaryInst() const { return false; }
bool BinaryInstruction::IsICmpInst() const { return false; }
bool BinaryInstruction::IsFCmpInst() const { return false; }

std::pair<BaseValuePtr, BaseValuePtr> BinaryInstruction::DoFlod() const {
    BaseValuePtr replacee = nullptr;
    BaseValuePtr replacer = nullptr;
    if (lhs->IsConstant() && rhs->IsConstant()) {
        replacee = result;
        replacer = ExprFlod::BinaryOperate(opcode, std::static_pointer_cast<Constant>(lhs),
                                           std::static_pointer_cast<Constant>(rhs));
    } else if (OP_ADD <= opcode && opcode <= OP_RSHIFT) {
        // only care about arithmetic operation
        if (lhs->IsConstant()) {
            auto constant_lhs = std::static_pointer_cast<Constant>(lhs);
            std::visit(
                [&](auto &&arg) {
                    using T = std::decay_t<decltype(arg)>;
                    assert(!(std::is_same_v<T, bool>));
                    if (opcode == OP_ADD && arg == static_cast<T>(0)) {
                        replacee = result;
                        replacer = rhs;
                    } else if (opcode == OP_MUL) {
                        if (arg == static_cast<T>(0)) {
                            replacee = result;
                            replacer = lhs;
                        } else if (arg == static_cast<T>(1)) {
                            replacee = result;
                            replacer = rhs;
                        }
                    } else if (opcode == OP_DIV && arg == static_cast<T>(0)) {
                        replacee = result;
                        replacer = lhs;
                    } else if (opcode == OP_REM && arg == static_cast<T>(0)) {
                        replacee = result;
                        replacer = lhs;
                    }
                },
                constant_lhs->GetValue());
        }
        if (rhs->IsConstant()) {
            auto constant_rhs = std::static_pointer_cast<Constant>(rhs);
            std::visit(
                [&](auto &&arg) {
                    using T = std::decay_t<decltype(arg)>;
                    assert(!(std::is_same_v<T, bool>));
                    if (opcode == OP_ADD && arg == static_cast<T>(0)) {
                        replacee = result;
                        replacer = lhs;
                    } else if (opcode == OP_SUB && arg == static_cast<T>(0)) {
                        replacee = result;
                        replacer = lhs;
                    } else if (opcode == OP_MUL) {
                        if (arg == static_cast<T>(0)) {
                            replacee = result;
                            replacer = rhs;
                        } else if (arg == static_cast<T>(1)) {
                            replacee = result;
                            replacer = lhs;
                        }
                    } else if (opcode == OP_DIV && arg == static_cast<T>(1)) {
                        replacee = result;
                        replacer = lhs;
                    } else if (opcode == OP_REM && arg == static_cast<T>(1)) {
                        replacee = result;
                        replacer = lhs;
                    }
                },
                constant_rhs->GetValue());
        }
    }
    return {replacee, replacer};
}

void BinaryInstruction::RemoveResParent() { result->SetParent(nullptr); }

const BaseValueList BinaryInstruction::GetOprands() const { return BaseValueList({lhs, rhs}); }

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
    // after replace use-count of `replacee` will be automatically deleted
}

//===-----------------------------------------------------------===//
//                     RemoveInst Implementation
//===-----------------------------------------------------------===//

void RemoveInst(InstPtr inst) {
    auto &&used_value = inst->GetOprands();
    std::for_each(used_value.begin(), used_value.end(), [&inst](BaseValuePtr value) { value->RemoveUser(inst); });
    inst->RemoveResParent();
}