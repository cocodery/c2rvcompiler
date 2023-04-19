#pragma once

#include "function/basicblock.hh"
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
    BlockPtr dest;

   public:
    JumpInst(BlockPtr);
    ~JumpInst() = default;

    static JumpInstPtr CreatePtr(BlockPtr);

    void setTarget(BlockPtr);

    std::string tollvmIR();
};

class BranchInst;
using BranchInstPtr = std::shared_ptr<BranchInst>;

class BranchInst : public Instruction {
   private:
    BaseValuePtr cond;
    BlockPtr iftrue;
    BlockPtr iffalse;

   public:
    BranchInst(BaseValuePtr, BlockPtr, BlockPtr);
    ~BranchInst() = default;

    static BranchInstPtr CreatePtr(BaseValuePtr, BlockPtr, BlockPtr);

    void setTrueTarget(BlockPtr);
    void setFalseTarget(BlockPtr);

    std::string tollvmIR();
};
