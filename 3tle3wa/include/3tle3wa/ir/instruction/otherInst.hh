#pragma once

#include "3tle3wa/ir/function/basefunc.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/value/baseValue.hh"

class BaseFunction;
using BaseFuncPtr = std::shared_ptr<BaseFunction>;

class CallInst;
using CallInstPtr = std::shared_ptr<CallInst>;

class CallInst final : public Instruction {
   private:
    ScalarTypePtr ret_type;
    BaseFuncPtr callee_func;
    ParamList rparam_list;

    static CallInstPtr CreatePtr(ScalarTypePtr, VariablePtr, BaseFuncPtr, ParamList &, CfgNodePtr);

   public:
    CallInst(ScalarTypePtr, VariablePtr, BaseFuncPtr, ParamList &, CfgNodePtr);
    ~CallInst() = default;

    static BaseValuePtr DoCallFunction(ScalarTypePtr, BaseFuncPtr, ParamList &, CfgNodePtr);

    ScalarTypePtr GetRetType() const;
    BaseFuncPtr GetCalleeFunc() const;
    const ParamList &GetParamList() const;

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList GetOprands() const;

    std::string tollvmIR();

    void TranslateTo(InternalTranslation &itx) final override;
};

class BitCastInst;
using BitCastInstPtr = std::shared_ptr<BitCastInst>;

class BitCastInst final : public UnaryInstruction {
   private:
    static BitCastInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    BitCastInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~BitCastInst() = default;

    static VariablePtr DoBitCast(BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();

    void TranslateTo(InternalTranslation &itx) final override;
};

class PhiInst;
using PhiInstPtr = std::shared_ptr<PhiInst>;

class AllocaInst;
using AllocaInstPtr = std::shared_ptr<AllocaInst>;
using PhiInstList = std::list<PhiInstPtr>;

class PhiInst final : public Instruction {
   private:
    AllocaInstPtr origin_alloca;

    std::list<std::pair<BaseValuePtr, CfgNodePtr>> datalist;

   public:
    PhiInst(VariablePtr, CfgNodePtr);
    ~PhiInst() = default;

    static PhiInstPtr CreatePtr(BaseTypePtr, CfgNodePtr);

    void SetOriginAlloca(AllocaInstPtr);

    const AllocaInstPtr GetOriginAlloca() const;

    static void InsertPhiData(PhiInstPtr, BaseValuePtr, CfgNodePtr);

    std::list<std::pair<BaseValuePtr, CfgNodePtr>> &GetRefList();
    const std::list<std::pair<BaseValuePtr, CfgNodePtr>> &GetDataList() const;

    void RemoveResParent();

    BaseValuePtr FindInComingUse(CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList GetOprands() const;

    std::string tollvmIR();

    void TranslateTo(InternalTranslation &itx) final override;
};
