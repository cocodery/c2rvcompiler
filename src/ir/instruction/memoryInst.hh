#pragma once

#include "cfgNode.hh"
#include "instruction.hh"
#include "typeconvert.hh"

class AllocaInst;
using AllocaInstPtr = std::shared_ptr<AllocaInst>;

class AllocaInst : public Instruction {
   private:
    BaseTypePtr type_stored;

    static AllocaInstPtr CreatePtr(BaseTypePtr, VariablePtr, CfgNodePtr);

   public:
    AllocaInst(BaseTypePtr, VariablePtr, CfgNodePtr);
    ~AllocaInst() = default;

    static VariablePtr DoAllocaAddr(BaseTypePtr, BaseTypePtr, CfgNodePtr);

    const BaseTypePtr GetAllocaType() const;
    const VariablePtr GetAllocaAddr() const;

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList GetOprands() const;

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

    const BaseValuePtr GetStoreAddr() const;
    BaseValuePtr GetStoreValue() const;

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList GetOprands() const;

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

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

class GetElementPtrInst;
using GepInstPtr = std::shared_ptr<GetElementPtrInst>;

class GetElementPtrInst : public Instruction {
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

    void RemoveResParent();

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    const BaseValueList GetOprands() const;

    std::string tollvmIR();
};
