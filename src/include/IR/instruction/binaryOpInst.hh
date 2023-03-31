#pragma once

#include "instruction.hh"

class IBinaryInst;
using IBinaryInstPtr = std::shared_ptr<IBinaryInst>;

class IBinaryInst : public Instruction {
private:
    BaseValuePtr i_result;
    char         i_op;
    BaseValuePtr i_lhs;
    BaseValuePtr i_rhs;
public:
    IBinaryInst(BaseValuePtr, char, BaseValuePtr, BaseValuePtr);
    ~IBinaryInst() = default;

    static IBinaryInstPtr CreatePtr(BaseValuePtr, char, BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

class FBinaryInst;
using FBinaryInstPtr = std::shared_ptr<FBinaryInst>;

class FBinaryInst : public Instruction {
private:
    BaseValuePtr f_result;
    char         f_op;
    BaseValuePtr f_lhs;
    BaseValuePtr f_rhs;
public:
    FBinaryInst(BaseValuePtr, char, BaseValuePtr, BaseValuePtr);
    ~FBinaryInst() = default;

    static FBinaryInstPtr CreatePtr(BaseValuePtr, char, BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};
