#include "binaryOpInst.hh"

//===-----------------------------------------------------------===//
//                     IBinaryInst Implementation
//===-----------------------------------------------------------===//

IBinaryInst::IBinaryInst(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : i_res(_res), i_op(_op), i_lhs(_lhs), i_rhs(_rhs) {
    assert(i_lhs->getBaseType()->IntType() && i_lhs->IsOprand());
    assert(i_rhs->getBaseType()->IntType() && i_rhs->IsOprand());
}

IBinaryInstPtr IBinaryInst::CreatePtr(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<IBinaryInst>(_res, _op, _lhs, _rhs);
}

VariablePtr IBinaryInst::DoIBinOperate(OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block) {
    VariablePtr _res = Variable::CreatePtr(type_int_L);
    block->insertInst(CreatePtr(_res, _op, _lhs, _rhs));
    return _res;
}

std::string IBinaryInst::tollvmIR() {
    std::stringstream ss;
    ss << i_res->tollvmIR() << " = ";
    switch (i_op) {
        case OP_ADD:
            ss << "add";
            break;
        case OP_SUB:
            ss << "sub";
            break;
        case OP_MUL:
            ss << "mul";
            break;
        case OP_DIV:
            ss << "sdiv";
            break;
        case OP_REM:
            ss << "srem";
            break;
        default:
            assert(0);
            break;
    }
    ss << " i32 " << i_lhs->tollvmIR() << ", " << i_rhs->tollvmIR();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     FBinaryInst Implementation
//===-----------------------------------------------------------===//

FBinaryInst::FBinaryInst(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs)
    : f_res(_res), f_op(_op), f_lhs(_lhs), f_rhs(_rhs) {
    assert(f_lhs->getBaseType()->FloatType() && f_lhs->IsOprand());
    assert(f_rhs->getBaseType()->FloatType() && f_rhs->IsOprand());
}

FBinaryInstPtr FBinaryInst::CreatePtr(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs) {
    return std::make_shared<FBinaryInst>(_res, _op, _lhs, _rhs);
}

VariablePtr FBinaryInst::DoFBinOperate(OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs, CfgNodePtr block) {
    VariablePtr _res = Variable::CreatePtr(type_float_L);
    block->insertInst(CreatePtr(_res, _op, _lhs, _rhs));
    return _res;
}

std::string FBinaryInst::tollvmIR() {
    std::stringstream ss;
    ss << f_res->tollvmIR() << " = ";
    switch (f_op) {
        case OP_ADD:
            ss << "fadd";
            break;
        case OP_SUB:
            ss << "fsub";
            break;
        case OP_MUL:
            ss << "fmul";
            break;
        case OP_DIV:
            ss << "fdiv";
            break;
        default:
            assert(0);
            break;
    }
    ss << " float " << f_lhs->tollvmIR() << ", " << f_rhs->tollvmIR();
    return ss.str();
}
