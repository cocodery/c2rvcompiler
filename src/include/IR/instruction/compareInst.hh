#pragma once

#include "instruction.hh"

class ICmpInst;
using ICmpInstPtr = std::shared_ptr<ICmpInst>;

class ICmpInst : public Instruction {
private:
    BaseValuePtr result;
    OpCode       cond;
    BaseValuePtr lhs;
    BaseValuePtr rhs;
public:
    ICmpInst(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);
    ~ICmpInst() = default;

    static ICmpInstPtr CreatePtr(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);
    
    std::string tollvmIR();
};

class FCmpInst;
using FCmpInstPtr = std::shared_ptr<FCmpInst>;

class FCmpInst : public Instruction {
private:
    BaseValuePtr result;
    OpCode       cond;
    BaseValuePtr lhs;
    BaseValuePtr rhs;
public:
    FCmpInst(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);
    ~FCmpInst() = default;

    static FCmpInstPtr CreatePtr(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);
    
    std::string tollvmIR();
};
