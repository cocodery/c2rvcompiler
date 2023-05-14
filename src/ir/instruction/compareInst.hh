#pragma once

#include "cfgNode.hh"
#include "instruction.hh"

class ICmpInst;
using ICmpInstPtr = std::shared_ptr<ICmpInst>;

class ICmpInst : public BinaryInstruction {
   private:
    static ICmpInstPtr CreatePtr(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

   public:
    ICmpInst(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~ICmpInst() = default;

    bool IsICmpInst() const;

    static VariablePtr DoICompare(OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

class FCmpInst;
using FCmpInstPtr = std::shared_ptr<FCmpInst>;

class FCmpInst : public BinaryInstruction {
   private:
    static FCmpInstPtr CreatePtr(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

   public:
    FCmpInst(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~FCmpInst() = default;

    bool IsFCmpInst() const;

    static VariablePtr DoFCompare(OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};
