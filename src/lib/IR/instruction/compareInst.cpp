#include "compareInst.hh"

//===-----------------------------------------------------------===//
//                     ICmpInst Implementation
//===-----------------------------------------------------------===//

ICmpInst::ICmpInst(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : result(_res), cond(_cond), lhs(_lhs), rhs(_rhs) {
    assert(_res->getBaseType()->checkType(BOOL, VARIABLE));
    TypeID tid_lhs = _lhs->getBaseType()->getMaskedType(BOOL | INT);
    TypeID tid_rhs = _rhs->getBaseType()->getMaskedType(BOOL | INT);
    assert(tid_lhs == tid_rhs);
}

ICmpInstPtr ICmpInst::CreatePtr(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<ICmpInst>(_res, _cond, _lhs, _rhs);
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
        default : assert(0)   ; break;
    }
    ss << " " << lhs->getBaseType()->tollvmIR() << " " << lhs->tollvmIR() << ", " << rhs->tollvmIR();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     FCmpInst Implementation
//===-----------------------------------------------------------===//

FCmpInst::FCmpInst(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : result(_res), cond(_cond), lhs(_lhs), rhs(_rhs) {
    assert(_res->getBaseType()->checkType(BOOL, VARIABLE));
    assert(_lhs->getBaseType()->checkType(FLOAT));
    assert(_rhs->getBaseType()->checkType(FLOAT));
}

FCmpInstPtr FCmpInst::CreatePtr(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<FCmpInst>(_res, _cond, _lhs, _rhs);
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
        default : assert(0)   ; break;
    }
    ss << " float " << lhs->tollvmIR() << ", " << rhs->tollvmIR();
    return ss.str();
}
