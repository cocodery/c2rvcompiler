#pragma once

#include "basefunc.hh"
#include "cfgNode.hh"
#include "instruction.hh"

class BaseFunction;
using BaseFuncPtr = std::shared_ptr<BaseFunction>;

class CallInst;
using CallInstPtr = std::shared_ptr<CallInst>;

class CallInst : public Instruction {
   private:
    ScalarTypePtr ret_type;
    VariablePtr ret_value;
    BaseFuncPtr callee_func;
    ParamList rparam_list;

    static CallInstPtr CreatePtr(ScalarTypePtr, VariablePtr, BaseFuncPtr, ParamList &, CfgNodePtr);

   public:
    CallInst(ScalarTypePtr, VariablePtr, BaseFuncPtr, ParamList &, CfgNodePtr);
    ~CallInst() = default;

    static BaseValuePtr DoCallFunction(ScalarTypePtr, BaseFuncPtr, ParamList &, CfgNodePtr);

    bool IsCallInst() const;

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
