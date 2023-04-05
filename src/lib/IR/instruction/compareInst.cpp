#include "compareInst.hh"

//===-----------------------------------------------------------===//
//                     ICmpInst Implementation
//===-----------------------------------------------------------===//

ICmpInst::ICmpInst(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : result(_res), cond(_cond), lhs(_lhs), rhs(_rhs) {
    assert(_res->getBaseType()->checkType(BOOL, VARIABLE));
    assert(_lhs->getBaseType()->checkType(BOOL | INT));
    assert(_rhs->getBaseType()->checkType(BOOL | INT));
}

ICmpInstPtr ICmpInst::CreatePtr(BaseValuePtr _res, OpCode _cond, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<ICmpInst>(_res, _cond, _lhs, _rhs);
}
    
std::string ICmpInst::tollvmIR() {
    std::stringstream ss;
    ss << result->tollvmIR() << " = icmp ";
    if (cond == OP_LTH) {
        ss << "slt";
    } else if (cond == OP_LEQ) {
        ss << "sle";
    } else if (cond == OP_EQU) {
        ss << "eq";
    } else if (cond == OP_NEQ) {
        ss << "ne";
    } else { assert(0); }
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
    if (cond == OP_LTH) {
        ss << "olt";
    } else if (cond == OP_LEQ) {
        ss << "ole";
    } else if (cond == OP_EQU) {
        ss << "oeq";
    } else if (cond == OP_NEQ) {
        ss << "one";
    } else { assert(0); }
    ss << " float " << lhs->tollvmIR() << ", " << rhs->tollvmIR();
    return ss.str();
}
