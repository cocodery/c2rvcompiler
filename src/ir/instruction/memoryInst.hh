#pragma once

#include "cfgNode.hh"
#include "instruction.hh"
#include "typeconvert.hh"

class AllocaInst;
using AllocaInstPtr = std::shared_ptr<AllocaInst>;

class AllocaInst : public Instruction {
   private:
    BaseTypePtr type_stored;
    BaseValuePtr addr_alloca;

    static AllocaInstPtr CreatePtr(BaseTypePtr, BaseValuePtr, CfgNodePtr);

   public:
    AllocaInst(BaseTypePtr, BaseValuePtr, CfgNodePtr);
    ~AllocaInst() = default;

    static VariablePtr DoAllocaAddr(BaseTypePtr, BaseTypePtr, CfgNodePtr);

    bool IsAllocaInst() const;
    const BaseTypePtr GetAllocaType() const;
    const BaseValuePtr GetAllocaAddr() const;

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    std::string tollvmIR();
};

class StoreInst;
using StoreInstPtr = std::shared_ptr<StoreInst>;

class StoreInst : public Instruction {
   private:
    BaseValuePtr store_addr;
    BaseValuePtr store_value;

    static StoreInstPtr CreatePtr(BaseValuePtr, BaseValuePtr, CfgNodePtr);

   public:
    StoreInst(BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~StoreInst() = default;

    static void DoStoreValue(BaseValuePtr, BaseValuePtr, CfgNodePtr);

    bool IsStoreInst() const;
    const BaseValuePtr GetStoreAddr() const;
    BaseValuePtr GetStoreValue() const;

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    std::string tollvmIR();
};

class LoadInst;
using LoadInstPtr = std::shared_ptr<LoadInst>;

class LoadInst : public UnaryInstruction {
   private:
    static LoadInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    LoadInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~LoadInst() = default;

    static BaseValuePtr DoLoadValue(BaseValuePtr, CfgNodePtr);

    bool IsLoadInst() const;

    std::string tollvmIR();
};

class GetElementPtrInst;
using GepInstPtr = std::shared_ptr<GetElementPtrInst>;

class GetElementPtrInst : public Instruction {
   private:
    VariablePtr target_ptr;
    BaseTypePtr store_type;
    BaseValuePtr base_addr;
    BaseValueList offset_list;

    static GepInstPtr CreatePtr(VariablePtr, BaseTypePtr, BaseValuePtr, BaseValueList, CfgNodePtr);

   public:
    GetElementPtrInst(VariablePtr, BaseTypePtr, BaseValuePtr, BaseValueList, CfgNodePtr);
    ~GetElementPtrInst() = default;

    static VariablePtr DoGetPointer(BaseTypePtr, BaseValuePtr, BaseValueList, CfgNodePtr);

    bool IsGepInst() const;

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList UsedValue();

    std::string tollvmIR();
};
