#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "baseValue.hh"
#include "variable.hh"

enum OpCode {
    None,
    Ret,
    Jump,
    Branch,
    Alloca,
    Store,
    Gep,
    Call,
    Phi,
    // Unary
    Load,
    BitCast,
    SiToFp,
    FpToSi,
    Zext,
    // Binary
    // IBinary
    // FBinary
    OP_ADD,     // binary add
    OP_SUB,     // binary sub
    OP_MUL,     // binary mul
    OP_DIV,     // binary div
    OP_REM,     // binary rem
    OP_NOT,     // unary not
    OP_MINUS,   // unary minus
    OP_LSHIFT,  // left shift
    OP_RSHIFT,  // right shift
    // ICmp
    // FCmp
    OP_LTH,  // less than
    OP_LEQ,  // less or equal
    OP_GTH,  // greater
    OP_GEQ,  // greater or equal
    OP_EQU,  // equal
    OP_NEQ,  // not equal
};

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

    virtual void ReplaceTarget(CfgNodePtr, CfgNodePtr);

    bool IsCriticalOperation() const;

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

    void RemoveResParent();

    const BaseValueList UsedValue();

    virtual std::string tollvmIR() = 0;
};

void ReplaceSRC(BaseValuePtr, BaseValuePtr);
void RemoveInst(InstPtr);
