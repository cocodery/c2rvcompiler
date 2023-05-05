#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "baseValue.hh"
#include "variable.hh"

using OpCode = uint64_t;
constexpr OpCode OP_ADD = (1ul << 0x0000);     // binary add
constexpr OpCode OP_SUB = (1ul << 0x0001);     // binary sub
constexpr OpCode OP_MUL = (1ul << 0x0002);     // binary mul
constexpr OpCode OP_DIV = (1ul << 0x0003);     // binary div
constexpr OpCode OP_REM = (1ul << 0x0004);     // binary rem
constexpr OpCode OP_NOT = (1ul << 0x0005);     // unary not
constexpr OpCode OP_MINUS = (1ul << 0x0006);   // unary minus
constexpr OpCode OP_LSHIFT = (1ul << 0x0007);  // left shift
constexpr OpCode OP_RSHIFT = (1ul << 0x0008);  // right shift
constexpr OpCode OP_LTH = (1ul << 0x0009);     // less than
constexpr OpCode OP_LEQ = (1ul << 0x000A);     // less or equal
constexpr OpCode OP_GTH = (1ul << 0x000B);     // greater
constexpr OpCode OP_GEQ = (1ul << 0x000C);     // greater or equal
constexpr OpCode OP_EQU = (1ul << 0x000D);     // equal
constexpr OpCode OP_NEQ = (1ul << 0x000E);     // not equal

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

class CtrlFlowGraphNode;
using CfgNodePtr = std::shared_ptr<CtrlFlowGraphNode>;

class Instruction {
   protected:
    size_t idx;
    CfgNodePtr parent;

   private:
    static size_t inst_idx;

   public:
    Instruction(CfgNodePtr);
    ~Instruction() = default;

    const size_t GetInstIdx() const;
    const CfgNodePtr GetParent() const;
    void SetParent(CfgNodePtr);
    void ClearParent();

    virtual bool IsTwoOprandInst() const;
    virtual bool IsOneOprandInst() const;

    virtual bool IsReturnInst() const;
    virtual bool IsJumpInst() const;
    virtual bool IsBranchInst() const;

    virtual bool IsAllocaInst() const;
    virtual bool IsStoreInst() const;
    virtual bool IsLoadInst() const;
    virtual bool IsGepInst() const;

    virtual bool IsCallInst() const;
    virtual bool IsPhiInst() const;

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
    UnaryInstruction(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~UnaryInstruction() = default;

    BaseValuePtr GetResult() const;
    BaseValuePtr GetOprand() const;

    bool IsOneOprandInst() const;

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    virtual std::string tollvmIR() = 0;
};

class BinaryInstruction : public Instruction {
   protected:
    VariablePtr result;
    OpCode op;
    BaseValuePtr lhs;
    BaseValuePtr rhs;

   public:
    BinaryInstruction(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~BinaryInstruction() = default;

    BaseValuePtr GetResult() const;
    BaseValuePtr GetLHS() const;
    BaseValuePtr GetRHS() const;

    bool IsTwoOprandInst() const;

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    virtual std::string tollvmIR() = 0;
};

void ReplaceSRC(BaseValuePtr, BaseValuePtr);
void RemoveInst(InstPtr);
