#pragma once

#include "../function/cfgNode.hh"
#include "instruction.hh"

class IBinaryInst;
using IBinaryInstPtr = std::shared_ptr<IBinaryInst>;

class IBinaryInst : public Instruction {
   private:
    VariablePtr i_res;
    OpCode i_op;
    BaseValuePtr i_lhs;
    BaseValuePtr i_rhs;

    static IBinaryInstPtr CreatePtr(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr);

   public:
    IBinaryInst(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr);
    ~IBinaryInst() = default;

    static VariablePtr DoIBinOperate(OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

    std::string tollvmIR();
};

class FBinaryInst;
using FBinaryInstPtr = std::shared_ptr<FBinaryInst>;

class FBinaryInst : public Instruction {
   private:
    VariablePtr f_res;
    OpCode f_op;
    BaseValuePtr f_lhs;
    BaseValuePtr f_rhs;

    static FBinaryInstPtr CreatePtr(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr);

   public:
    FBinaryInst(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr);
    ~FBinaryInst() = default;

    static VariablePtr DoFBinOperate(OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

    std::string tollvmIR();
};
