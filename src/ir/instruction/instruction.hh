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

    VariablePtr result;
    OpCode opcode;

    CfgNodePtr parent;

   private:
    static size_t inst_idx;

   public:
    Instruction(VariablePtr, OpCode, CfgNodePtr);
    ~Instruction() = default;

    const OpCode &GetOpCode() const;
    const size_t &GetInstIdx() const;
    const CfgNodePtr &GetParent() const;
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

    bool IsValueNumberInst() const;

    bool IsCriticalOperation() const;

    VariablePtr GetResult() const;

    virtual const BaseValueList GetOprands() const = 0;

    virtual std::pair<BaseValuePtr, BaseValuePtr> DoFlod() const;

    virtual void ReplaceTarget(CfgNodePtr, CfgNodePtr);

    virtual void RemoveResParent() = 0;

    virtual bool ReplaceSRC(BaseValuePtr, BaseValuePtr) = 0;

    virtual std::string tollvmIR() = 0;
};

class UnaryInstruction;
using UnaryInstPtr = std::shared_ptr<UnaryInstruction>;
class UnaryInstruction : public Instruction {
   protected:
    BaseValuePtr oprand;

   public:
    UnaryInstruction(VariablePtr, OpCode, BaseValuePtr, CfgNodePtr);
    ~UnaryInstruction() = default;

    BaseValuePtr GetOprand() const;

    std::pair<BaseValuePtr, BaseValuePtr> DoFlod() const;

    void RemoveResParent();

    const BaseValueList GetOprands() const;

    virtual std::string tollvmIR() = 0;
};

class BinaryInstruction;
using BinaryInstPtr = std::shared_ptr<BinaryInstruction>;
class BinaryInstruction : public Instruction {
   protected:
    BaseValuePtr lhs;
    BaseValuePtr rhs;

   public:
    BinaryInstruction(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~BinaryInstruction() = default;

    BaseValuePtr GetLHS() const;
    BaseValuePtr GetRHS() const;

    virtual bool IsIBinaryInst() const;
    virtual bool IsFBinaryInst() const;
    virtual bool IsICmpInst() const;
    virtual bool IsFCmpInst() const;

    std::pair<BaseValuePtr, BaseValuePtr> DoFlod() const;

    void RemoveResParent();

    const BaseValueList GetOprands() const;

    virtual std::string tollvmIR() = 0;
};

void ReplaceSRC(BaseValuePtr, BaseValuePtr);
void RemoveInst(InstPtr);
