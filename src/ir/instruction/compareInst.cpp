#include "compareInst.hh"

//===-----------------------------------------------------------===//
//                     ICmpInst Implementation
//===-----------------------------------------------------------===//

ICmpInst::ICmpInst(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block)
    : BinaryInstruction(_res, _op, _lhs, _rhs, block) {
    assert(lhs->IsOprand() && rhs->IsOprand());
    assert(lhs->getBaseType()->getAttrType() == rhs->getBaseType()->getAttrType());
    assert(lhs->getBaseType()->BoolType() || lhs->getBaseType()->IntType());
}

ICmpInstPtr ICmpInst::CreatePtr(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block) {
    return std::make_shared<ICmpInst>(_res, _op, _lhs, _rhs, block);
}

VariablePtr ICmpInst::DoICompare(OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block) {
    VariablePtr _res = Variable::CreatePtr(type_bool, nullptr);
    auto &&inst = CreatePtr(_res, _op, _lhs, _rhs, block);
    _res->SetParent(inst);
    _lhs->InsertUser(inst);
    _rhs->InsertUser(inst);
    block->InsertInstBack(inst);
    return _res;
}

std::string ICmpInst::tollvmIR() {
    std::stringstream ss;
    ss << result->tollvmIR() << " = icmp ";
    switch (op) {
        case OP_LTH:
            ss << "slt";
            break;
        case OP_LEQ:
            ss << "sle";
            break;
        case OP_GTH:
            ss << "sgt";
            break;
        case OP_GEQ:
            ss << "sge";
            break;
        case OP_EQU:
            ss << "eq";
            break;
        case OP_NEQ:
            ss << "ne";
            break;
        default:
            assert(0);
            break;
    }
    ss << " " << lhs->getBaseType()->tollvmIR() << " " << lhs->tollvmIR() << ", " << rhs->tollvmIR();
    ss << "; Inst_" << GetInstIdx() << " from Block_";
    if (parent == nullptr) {
        ss << "None";
    } else {
        ss << parent->GetBlockIdx();
    }
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     FCmpInst Implementation
//===-----------------------------------------------------------===//

FCmpInst::FCmpInst(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block)
    : BinaryInstruction(_res, _op, _lhs, _rhs, block) {
    assert(lhs->IsOprand() && rhs->IsOprand());
    assert(lhs->getBaseType()->getAttrType() == rhs->getBaseType()->getAttrType());
    assert(lhs->getBaseType()->FloatType());
}

FCmpInstPtr FCmpInst::CreatePtr(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block) {
    return std::make_shared<FCmpInst>(_res, _op, _lhs, _rhs, block);
}

VariablePtr FCmpInst::DoFCompare(OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block) {
    VariablePtr _res = Variable::CreatePtr(type_bool, nullptr);
    auto &&inst = CreatePtr(_res, _op, _lhs, _rhs, block);
    _res->SetParent(inst);
    _lhs->InsertUser(inst);
    _rhs->InsertUser(inst);
    block->InsertInstBack(inst);
    return _res;
}

std::string FCmpInst::tollvmIR() {
    std::stringstream ss;
    ss << result->tollvmIR() << " = fcmp ";
    switch (op) {
        case OP_LTH:
            ss << "olt";
            break;
        case OP_LEQ:
            ss << "ole";
            break;
        case OP_GTH:
            ss << "ogt";
            break;
        case OP_GEQ:
            ss << "oge";
            break;
        case OP_EQU:
            ss << "oeq";
            break;
        case OP_NEQ:
            ss << "one";
            break;
        default:
            assert(0);
            break;
    }
    ss << " float " << lhs->tollvmIR() << ", " << rhs->tollvmIR();
    ss << "; Inst_" << GetInstIdx() << " from Block_";
    if (parent == nullptr) {
        ss << "None";
    } else {
        ss << parent->GetBlockIdx();
    }
    return ss.str();
}
