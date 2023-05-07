#include "compareInst.hh"

//===-----------------------------------------------------------===//
//                     ICmpInst Implementation
//===-----------------------------------------------------------===//

ICmpInst::ICmpInst(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block)
    : BinaryInstruction(_res, _op, _lhs, _rhs, block) {
    assert(lhs->IsOprand() && rhs->IsOprand());
    assert(lhs->GetBaseType()->GetAttrType() == rhs->GetBaseType()->GetAttrType());
    assert(lhs->GetBaseType()->BoolType() || lhs->GetBaseType()->IntType());
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

bool ICmpInst::ReplaceSRC(BaseValuePtr replacee, BaseValuePtr replacer) {
    assert(replacer->IsOprand());
    assert(replacer->GetBaseType()->BoolType() || replacer->GetBaseType()->IntType());
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

std::string ICmpInst::tollvmIR() {
    std::stringstream ss;
    ss << result->tollvmIR() << " = icmp ";
    switch (opcode) {
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
    ss << " " << lhs->GetBaseType()->tollvmIR() << " " << lhs->tollvmIR() << ", " << rhs->tollvmIR();
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
    assert(lhs->GetBaseType()->GetAttrType() == rhs->GetBaseType()->GetAttrType());
    assert(lhs->GetBaseType()->FloatType());
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

bool FCmpInst::ReplaceSRC(BaseValuePtr replacee, BaseValuePtr replacer) {
    assert(replacer->GetBaseType()->FloatType() && replacer->IsOprand());
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

std::string FCmpInst::tollvmIR() {
    std::stringstream ss;
    ss << result->tollvmIR() << " = fcmp ";
    switch (opcode) {
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
