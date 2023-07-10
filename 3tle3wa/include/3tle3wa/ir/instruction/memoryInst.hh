#pragma once

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/typeconvert.hh"

class AllocaInst;
using AllocaInstPtr = std::shared_ptr<AllocaInst>;

class AllocaInst final : public Instruction {
   private:
    BaseTypePtr type_stored;

   public:
    AllocaInst(BaseTypePtr, VariablePtr, CfgNodePtr);
    ~AllocaInst() = default;

    static AllocaInstPtr CreatePtr(BaseTypePtr, VariablePtr, CfgNodePtr);

    static VariablePtr DoAllocaAddr(BaseTypePtr, BaseTypePtr, CfgNodePtr);

    const BaseTypePtr GetAllocaType() const;
    const VariablePtr GetAllocaAddr() const;

    void RemoveResParent() final override;

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr) final override;

    const BaseValueList GetOprands() const final override;

    std::string tollvmIR() final override;

    void TranslateTo(InternalTranslation &itx) final override;
};

class StoreInst;
using StoreInstPtr = std::shared_ptr<StoreInst>;

class StoreInst final : public Instruction {
   private:
    BaseValuePtr store_addr;
    BaseValuePtr store_value;

   public:
    StoreInst(BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~StoreInst() = default;

    static StoreInstPtr CreatePtr(BaseValuePtr, BaseValuePtr, CfgNodePtr);

    static void DoStoreValue(BaseValuePtr, BaseValuePtr, CfgNodePtr);

    const BaseValuePtr GetStoreAddr() const;
    BaseValuePtr GetStoreValue() const;

    void RemoveResParent() final override;

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr) final override;

    const BaseValueList GetOprands() const final override;

    std::string tollvmIR() final override;

    void TranslateTo(InternalTranslation &itx) final override;
};

class LoadInst;
using LoadInstPtr = std::shared_ptr<LoadInst>;

class LoadInst final : public UnaryInstruction {
   public:
    LoadInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~LoadInst() = default;

    static LoadInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

    static BaseValuePtr DoLoadValue(BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr) final override;

    std::string tollvmIR() final override;

    void TranslateTo(InternalTranslation &itx) final override;
};

class GetElementPtrInst;
using GepInstPtr = std::shared_ptr<GetElementPtrInst>;

class GetElementPtrInst final : public Instruction {
   private:
    BaseTypePtr store_type;
    BaseValuePtr base_addr;
    BaseValueList offset_list;

    static GepInstPtr CreatePtr(VariablePtr, BaseTypePtr, BaseValuePtr, BaseValueList, CfgNodePtr);

   public:
    GetElementPtrInst(VariablePtr, BaseTypePtr, BaseValuePtr, BaseValueList, CfgNodePtr);
    ~GetElementPtrInst() = default;

    static VariablePtr DoGetPointer(BaseTypePtr, BaseValuePtr, BaseValueList, CfgNodePtr);

    BaseTypePtr GetStoreType() const;
    BaseValuePtr GetBaseAddr() const;
    BaseValueList GetOffList() const;

    void RemoveResParent() final override;

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr) final override;

    const BaseValueList GetOprands() const final override;

    std::string tollvmIR() final override;

    void TranslateTo(InternalTranslation &itx) final override;
};
