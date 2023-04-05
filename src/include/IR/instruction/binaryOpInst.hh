#pragma once

#include "instruction.hh"

class IBinaryInst;
using IBinaryInstPtr = std::shared_ptr<IBinaryInst>;

class IBinaryInst : public Instruction {
private:
    BaseValuePtr i_result;
    OpCode       i_op;
    BaseValuePtr i_lhs;
    BaseValuePtr i_rhs;
public:
    IBinaryInst(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);
    ~IBinaryInst() = default;

    static IBinaryInstPtr CreatePtr(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

class FBinaryInst;
using FBinaryInstPtr = std::shared_ptr<FBinaryInst>;

class FBinaryInst : public Instruction {
private:
    BaseValuePtr f_result;
    OpCode       f_op;
    BaseValuePtr f_lhs;
    BaseValuePtr f_rhs;
public:
    FBinaryInst(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);
    ~FBinaryInst() = default;

    static FBinaryInstPtr CreatePtr(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};
