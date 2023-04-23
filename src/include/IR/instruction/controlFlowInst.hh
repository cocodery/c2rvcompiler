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
    CfgNodePtr block;
    CfgNodePtr dest;

   public:
    JumpInst(CfgNodePtr, CfgNodePtr);
    ~JumpInst() = default;

    static JumpInstPtr CreatePtr(CfgNodePtr, CfgNodePtr);

    void setTarget(CfgNodePtr);

    std::string tollvmIR();
};

class BranchInst;
using BranchInstPtr = std::shared_ptr<BranchInst>;

class BranchInst : public Instruction {
   private:
    CfgNodePtr block;
    BaseValuePtr cond;
    CfgNodePtr iftrue;
    CfgNodePtr iffalse;

   public:
    BranchInst(CfgNodePtr, BaseValuePtr, CfgNodePtr, CfgNodePtr);
    ~BranchInst() = default;

    static BranchInstPtr CreatePtr(CfgNodePtr, BaseValuePtr, CfgNodePtr, CfgNodePtr);

    void setTrueTarget(CfgNodePtr);
    void setFalseTarget(CfgNodePtr);

    std::string tollvmIR();
};
