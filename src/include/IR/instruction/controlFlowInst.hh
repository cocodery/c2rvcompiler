#pragma once

#include "function/cfgNode.hh"
#include "instruction.hh"
#include "value/type/scalarType.hh"

class ReturnInst;
using RetInstPtr = std::shared_ptr<ReturnInst>;

class ReturnInst : public Instruction {
   private:
    ScalarTypePtr ret_type;
    BaseValuePtr ret_value;

   public:
    ReturnInst(ScalarTypePtr, BaseValuePtr);
    ~ReturnInst() = default;

    static RetInstPtr CreatePtr(ScalarTypePtr, BaseValuePtr);

    std::string tollvmIR();
};

class JumpInst;
using JumpInstPtr = std::shared_ptr<JumpInst>;

class JumpInst : public Instruction {
   private:
    CfgNodePtr dest;

   public:
    JumpInst(CfgNodePtr);
    ~JumpInst() = default;

    static JumpInstPtr CreatePtr(CfgNodePtr);

    void setTarget(CfgNodePtr);

    std::string tollvmIR();
};

class BranchInst;
using BranchInstPtr = std::shared_ptr<BranchInst>;

class BranchInst : public Instruction {
   private:
    BaseValuePtr cond;
    CfgNodePtr iftrue;
    CfgNodePtr iffalse;

   public:
    BranchInst(BaseValuePtr, CfgNodePtr, CfgNodePtr);
    ~BranchInst() = default;

    static BranchInstPtr CreatePtr(BaseValuePtr, CfgNodePtr, CfgNodePtr);

    void setTrueTarget(CfgNodePtr);
    void setFalseTarget(CfgNodePtr);

    std::string tollvmIR();
};
