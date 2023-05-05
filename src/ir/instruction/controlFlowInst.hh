#pragma once

#include "cfgNode.hh"
#include "instruction.hh"
#include "scalarType.hh"

class ReturnInst;
using RetInstPtr = std::shared_ptr<ReturnInst>;

class ReturnInst : public Instruction {
   private:
    ScalarTypePtr ret_type;
    BaseValuePtr ret_value;

   public:
    ReturnInst(ScalarTypePtr, BaseValuePtr, CfgNodePtr);
    ~ReturnInst() = default;

    static RetInstPtr CreatePtr(ScalarTypePtr, BaseValuePtr, CfgNodePtr);

    bool IsReturnInst() const;

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    std::string tollvmIR();
};

class JumpInst;
using JumpInstPtr = std::shared_ptr<JumpInst>;

class JumpInst : public Instruction {
   private:
    CfgNodePtr dest;

   public:
    JumpInst(CfgNodePtr, CfgNodePtr);
    ~JumpInst() = default;

    static JumpInstPtr CreatePtr(CfgNodePtr, CfgNodePtr);

    bool IsJumpInst() const;

    void SetTarget(CfgNodePtr);

    CfgNodePtr GetTarget();

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

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
    BranchInst(BaseValuePtr, CfgNodePtr, CfgNodePtr, CfgNodePtr);
    ~BranchInst() = default;

    static BranchInstPtr CreatePtr(BaseValuePtr, CfgNodePtr, CfgNodePtr, CfgNodePtr);

    bool IsBranchInst() const;

    void SetTrueTarget(CfgNodePtr);
    void SetFalseTarget(CfgNodePtr);

    CfgNodePtr GetTrueTarget();
    CfgNodePtr GetFalseTarget();

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    std::string tollvmIR();
};
