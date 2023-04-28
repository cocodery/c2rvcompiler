#pragma once

#include "cfgNode.hh"
#include "instruction.hh"
#include "scalarType.hh"

class CallInst;
using CallInstPtr = std::shared_ptr<CallInst>;
using RParamList = std::vector<BaseValuePtr>;

class CallInst : public Instruction {
   private:
    ScalarTypePtr ret_type;
    VariablePtr ret_value;
    std::string callee_name;
    RParamList rparam_list;

    static CallInstPtr CreatePtr(ScalarTypePtr, VariablePtr, std::string &, RParamList &, CfgNodePtr);

   public:
    CallInst(ScalarTypePtr, VariablePtr, std::string &, RParamList &, CfgNodePtr);
    ~CallInst() = default;

    static BaseValuePtr DoCallFunction(ScalarTypePtr, std::string &, RParamList &, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    std::string tollvmIR();
};

class BitCastInst;
using BitCastInstPtr = std::shared_ptr<BitCastInst>;

class BitCastInst : public UnaryInstruction {
   private:
    static BitCastInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    BitCastInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~BitCastInst() = default;

    static VariablePtr DoBitCast(BaseValuePtr, CfgNodePtr);

    std::string tollvmIR();
};

class PhiInst;
using PhiInstPtr = std::shared_ptr<PhiInst>;

class PhiInst : public Instruction {
   private:
    VariablePtr result;
    std::list<std::pair<BaseValuePtr, BlockPtr>> datalist;

   public:
    PhiInst(VariablePtr, CfgNodePtr);
    ~PhiInst() = default;

    static PhiInstPtr CreatePtr(BaseTypePtr, CfgNodePtr);

    VariablePtr GetResult();

    static void InsertPhiData(PhiInstPtr, BaseValuePtr, CfgNodePtr);

    bool IsPhiInst() const;

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    std::string tollvmIR();
};
