#pragma once

#include "../function/basicblock.hh"
#include "instruction.hh"

class ICmpInst;
using ICmpInstPtr = std::shared_ptr<ICmpInst>;

class ICmpInst : public Instruction {
   private:
    BaseValuePtr result;
    OpCode cond;
    BaseValuePtr lhs;
    BaseValuePtr rhs;

    static ICmpInstPtr CreatePtr(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);

   public:
    ICmpInst(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);
    ~ICmpInst() = default;

    static VariablePtr DoICompare(OpCode, BaseValuePtr, BaseValuePtr, BlockPtr);

    std::string tollvmIR();
};

class FCmpInst;
using FCmpInstPtr = std::shared_ptr<FCmpInst>;

class FCmpInst : public Instruction {
   private:
    BaseValuePtr result;
    OpCode cond;
    BaseValuePtr lhs;
    BaseValuePtr rhs;

    static FCmpInstPtr CreatePtr(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);

   public:
    FCmpInst(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr);
    ~FCmpInst() = default;

    static VariablePtr DoFCompare(OpCode, BaseValuePtr, BaseValuePtr, BlockPtr);

    std::string tollvmIR();
};
