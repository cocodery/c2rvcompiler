#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>

#include "baseValue.hh"
#include "exprFlod.hh"
#include "opCode.hh"
#include "variable.hh"

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

class CtrlFlowGraphNode;
using CfgNodePtr = std::shared_ptr<CtrlFlowGraphNode>;

class Instruction {
   protected:
    size_t idx;
    CfgNodePtr parent;
    OpCode opcode;

   private:
    static size_t inst_idx;

   public:
    Instruction(OpCode, CfgNodePtr);
    ~Instruction() = default;

    const OpCode GetOpCode() const;
    const size_t GetInstIdx() const;
    const CfgNodePtr GetParent() const;
    void SetParent(CfgNodePtr);
    void ClearParent();

    bool IsTwoOprandInst() const;
    bool IsOneOprandInst() const;

    bool IsReturnInst() const;
    bool IsJumpInst() const;
    bool IsBranchInst() const;

    bool IsAllocaInst() const;
    bool IsStoreInst() const;
    bool IsLoadInst() const;
    bool IsGepInst() const;

    bool IsCallInst() const;
    bool IsPhiInst() const;

    bool IsCriticalOperation() const;

    virtual std::pair<BaseValuePtr, BaseValuePtr> DoFlod() const;

    virtual void ReplaceTarget(CfgNodePtr, CfgNodePtr);

    virtual void RemoveResParent() = 0;

    virtual bool ReplaceSRC(BaseValuePtr, BaseValuePtr) = 0;

    virtual const BaseValueList UsedValue() = 0;

    virtual std::string tollvmIR() = 0;
};

class UnaryInstruction : public Instruction {
   protected:
    VariablePtr result;
    BaseValuePtr oprand;

   public:
    UnaryInstruction(VariablePtr, OpCode, BaseValuePtr, CfgNodePtr);
    ~UnaryInstruction() = default;

    BaseValuePtr GetResult() const;
    BaseValuePtr GetOprand() const;

    std::pair<BaseValuePtr, BaseValuePtr> DoFlod() const;

    void RemoveResParent();

    const BaseValueList UsedValue();

    virtual std::string tollvmIR() = 0;
};

class BinaryInstruction : public Instruction {
   protected:
    VariablePtr result;
    BaseValuePtr lhs;
    BaseValuePtr rhs;

   public:
    BinaryInstruction(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~BinaryInstruction() = default;

    BaseValuePtr GetResult() const;
    BaseValuePtr GetLHS() const;
    BaseValuePtr GetRHS() const;

    std::pair<BaseValuePtr, BaseValuePtr> DoFlod() const;

    void RemoveResParent();

    const BaseValueList UsedValue();

    virtual std::string tollvmIR() = 0;
};

void ReplaceSRC(BaseValuePtr, BaseValuePtr);
void RemoveInst(InstPtr);
