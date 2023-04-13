#include "compareInst.hh"

//===-----------------------------------------------------------===//
//                     ICmpInst Implementation
//===-----------------------------------------------------------===//

ICmpInst::ICmpInst(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : result(_res), cond(_cond), lhs(_lhs), rhs(_rhs) {
    assert(lhs->isBinaryOprand() && rhs->isBinaryOprand());
    assert(lhs->getBaseType()->getAttrType() == rhs->getBaseType()->getAttrType());
}

ICmpInstPtr ICmpInst::CreatePtr(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<ICmpInst>(_res, _cond, _lhs, _rhs);
}

VariablePtr ICmpInst::DoICompare(OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, BlockPtr block) {
    VariablePtr _res = Variable::CreatePtr(type_bool);
    block->insertInst(CreatePtr(_res, _op, _lhs, _rhs));
    return _res;
}
    
std::string ICmpInst::tollvmIR() {
    std::stringstream ss;
    ss << result->tollvmIR() << " = icmp ";
    switch (cond) {
        case OP_LTH: ss << "slt"; break;
        case OP_LEQ: ss << "sle"; break;
        case OP_GTH: ss << "sgt"; break;
        case OP_GEQ: ss << "sge"; break;
        case OP_EQU: ss << "eq" ; break;
        case OP_NEQ: ss << "ne" ; break;
        default : assert(0)     ; break;
    }
    ss << " " << lhs->getBaseType()->tollvmIR() << " " << lhs->tollvmIR() << ", " << rhs->tollvmIR();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     FCmpInst Implementation
//===-----------------------------------------------------------===//

FCmpInst::FCmpInst(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : result(_res), cond(_cond), lhs(_lhs), rhs(_rhs) {
    assert(lhs->isBinaryOprand() && rhs->isBinaryOprand());
    assert(lhs->getBaseType()->getAttrType() == rhs->getBaseType()->getAttrType());
}

FCmpInstPtr FCmpInst::CreatePtr(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<FCmpInst>(_res, _cond, _lhs, _rhs);
}

VariablePtr FCmpInst::DoFCompare(OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, BlockPtr block) {
    VariablePtr _res = Variable::CreatePtr(type_bool);
    block->insertInst(CreatePtr(_res, _op, _lhs, _rhs));
    return _res;
}
    
std::string FCmpInst::tollvmIR() {
    std::stringstream ss;
    ss << result->tollvmIR() << " = fcmp ";
    switch (cond) {
        case OP_LTH: ss << "olt"; break;
        case OP_LEQ: ss << "ole"; break;
        case OP_GTH: ss << "ogt"; break;
        case OP_GEQ: ss << "oge"; break;
        case OP_EQU: ss << "oeq"; break;
        case OP_NEQ: ss << "one"; break;
        default : assert(0)     ; break;
    }
    ss << " float " << lhs->tollvmIR() << ", " << rhs->tollvmIR();
    return ss.str();
}
