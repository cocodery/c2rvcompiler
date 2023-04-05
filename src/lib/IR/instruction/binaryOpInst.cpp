#include "binaryOpInst.hh"

//===-----------------------------------------------------------===//
//                     IBinaryInst Implementation
//===-----------------------------------------------------------===//

IBinaryInst::IBinaryInst(BaseValuePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : i_result(_res), i_op(_op), i_lhs(_lhs), i_rhs(_rhs) {
    assert(i_result->getBaseType()->checkType(INT));
    assert(i_lhs   ->getBaseType()->checkType(INT));
    assert(i_rhs   ->getBaseType()->checkType(INT));
}

IBinaryInstPtr IBinaryInst::CreatePtr(BaseValuePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<IBinaryInst>(_res, _op, _lhs, _rhs);
}

std::string IBinaryInst::tollvmIR() {
    std::stringstream ss;
    ss << i_result->tollvmIR() << " = ";
    switch (i_op) {
        case OP_ADD: ss << "add" ; break;
        case OP_SUB: ss << "sub" ; break;
        case OP_MUL: ss << "mul" ; break;
        case OP_DIV: ss << "sdiv"; break;
        case OP_REM: ss << "srem"; break;
        default : assert(0)   ; break;
    }
    ss << " i32 " << i_lhs->tollvmIR() << ", " << i_rhs->tollvmIR();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     FBinaryInst Implementation
//===-----------------------------------------------------------===//

FBinaryInst::FBinaryInst(BaseValuePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : f_result(_res), f_op(_op), f_lhs(_lhs), f_rhs(_rhs) {
    assert(f_result->getBaseType()->checkType(FLOAT));
    assert(f_lhs   ->getBaseType()->checkType(FLOAT));
    assert(f_rhs   ->getBaseType()->checkType(FLOAT));
}

FBinaryInstPtr FBinaryInst::CreatePtr(BaseValuePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<FBinaryInst>(_res, _op, _lhs, _rhs);
}

std::string FBinaryInst::tollvmIR() {
    std::stringstream ss;
    ss << f_result->tollvmIR() << " = ";
    switch (f_op) {
        case OP_ADD: ss << "fadd" ; break;
        case OP_SUB: ss << "fsub" ; break;
        case OP_MUL: ss << "fmul" ; break;
        case OP_DIV: ss << "fdiv" ; break;
        default : assert(0)    ; break;
    }
    ss << " float " <<  f_lhs->tollvmIR() << ", " << f_rhs->tollvmIR();
    return ss.str();
}

