#include "binaryOpInst.hh"

//===-----------------------------------------------------------===//
//                     IBinaryInst Implementation
//===-----------------------------------------------------------===//

IBinaryInst::IBinaryInst(BaseValuePtr _res, char _op, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : i_result(_res), i_op(_op), i_lhs(_lhs), i_rhs(_rhs) {
}

IBinaryInstPtr IBinaryInst::CreatePtr(BaseValuePtr _res, char _op, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<IBinaryInst>(_res, _op, _lhs, _rhs);
}

std::string IBinaryInst::tollvmIR() {
    assert(i_result->getBaseType()->checkType(INT));
    assert(i_lhs   ->getBaseType()->checkType(INT));
    assert(i_rhs   ->getBaseType()->checkType(INT));

    std::stringstream ss;
    ss << i_result->tollvmIR() << " = ";
    switch (i_op) {
        case '+': ss << "add" ; break;
        case '-': ss << "sub" ; break;
        case '*': ss << "mul" ; break;
        case '/': ss << "sdiv"; break;
        case '%': ss << "srem"; break;
        default : assert(0)   ; break;
    }
    ss << " i32 " << i_lhs->tollvmIR() << ", " << i_rhs->tollvmIR();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     FBinaryInst Implementation
//===-----------------------------------------------------------===//

FBinaryInst::FBinaryInst(BaseValuePtr _res, char _op, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : f_result(_res), f_op(_op), f_lhs(_lhs), f_rhs(_rhs) {
}

FBinaryInstPtr FBinaryInst::CreatePtr(BaseValuePtr _res, char _op, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<FBinaryInst>(_res, _op, _lhs, _rhs);
}

std::string FBinaryInst::tollvmIR() {
    assert(f_result->getBaseType()->checkType(FLOAT));
    assert(f_lhs   ->getBaseType()->checkType(FLOAT));
    assert(f_rhs   ->getBaseType()->checkType(FLOAT));

    std::stringstream ss;
    ss << f_result->tollvmIR() << " = ";
    switch (f_op) {
        case '+': ss << "fadd" ; break;
        case '-': ss << "fsub" ; break;
        case '*': ss << "fmul" ; break;
        case '/': ss << "fdiv" ; break;
        default : assert(0)    ; break;
    }
    ss << " float " <<  f_lhs->tollvmIR() << ", " << f_rhs->tollvmIR();
    return ss.str();
}

