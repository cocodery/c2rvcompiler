#pragma once

#include "function/cfgNode.hh"
#include "instruction.hh"
#include "value/type/scalarType.hh"

class CallInst;
using CallInstPtr = std::shared_ptr<CallInst>;
using RParamList = std::vector<BaseValuePtr>;

class CallInst : public Instruction {
   private:
    ScalarTypePtr ret_type;
    VariablePtr ret_value;
    std::string callee_name;
    RParamList rparam_list;

    static CallInstPtr CreatePtr(ScalarTypePtr, VariablePtr, std::string &, RParamList &);

   public:
    CallInst(ScalarTypePtr, VariablePtr, std::string &, RParamList &);
    ~CallInst() = default;

    static BaseValuePtr DoCallFunction(ScalarTypePtr, std::string &, RParamList &, CfgNodePtr);

    std::string tollvmIR();
};

class BitCastInst;
using BitCastInstPtr = std::shared_ptr<BitCastInst>;

class BitCastInst : public Instruction {
   private:
    BaseValuePtr result;
    BaseValuePtr oprand;

    static BitCastInstPtr CreatePtr(BaseValuePtr, BaseValuePtr);

   public:
    BitCastInst(BaseValuePtr, BaseValuePtr);
    ~BitCastInst() = default;

    static BaseValuePtr DoBitCast(BaseValuePtr, CfgNodePtr);

    std::string tollvmIR();
};
