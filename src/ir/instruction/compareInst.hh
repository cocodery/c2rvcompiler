#pragma once

#include "cfgNode.hh"
#include "instruction.hh"

class ICmpInst;
using ICmpInstPtr = std::shared_ptr<ICmpInst>;

class ICmpInst : public Instruction {
   private:
    BaseValuePtr result;
    OpCode cond;
    BaseValuePtr lhs;
    BaseValuePtr rhs;

    static ICmpInstPtr CreatePtr(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

   public:
    ICmpInst(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~ICmpInst() = default;

    static VariablePtr DoICompare(OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

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

    static FCmpInstPtr CreatePtr(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

   public:
    FCmpInst(BaseValuePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~FCmpInst() = default;

    static VariablePtr DoFCompare(OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

    std::string tollvmIR();
};
