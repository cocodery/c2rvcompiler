#pragma once

#include "../function/basicblock.hh"
#include "instruction.hh"
#include "typeconvert.hh"

class AllocaInst;
using AllocaInstPtr = std::shared_ptr<AllocaInst>;

class AllocaInst : public Instruction {
private:
    BaseTypePtr  type_stored;
    BaseValuePtr addr_alloca;

    static AllocaInstPtr CreatePtr(BaseTypePtr, BaseValuePtr);
public:
    AllocaInst(BaseTypePtr, BaseValuePtr);
    ~AllocaInst() = default;

    static VariablePtr DoAllocaAddr(BaseTypePtr, BaseTypePtr, BlockPtr);

    std::string tollvmIR();
};

class StoreInst;
using StoreInstPtr = std::shared_ptr<StoreInst>;

class StoreInst : public Instruction {
private:
    BaseValuePtr store_addr;
    BaseValuePtr store_value;

    static StoreInstPtr CreatePtr(BaseValuePtr, BaseValuePtr);
public:
    StoreInst(BaseValuePtr, BaseValuePtr);
    ~StoreInst() = default;

    static void DoStoreValue(BaseValuePtr, BaseValuePtr, BlockPtr);

    std::string tollvmIR();
};

class LoadInst;
using LoadInstPtr = std::shared_ptr<LoadInst>;

class LoadInst : public Instruction {
private:
    BaseValuePtr load_value;
    BaseValuePtr load_addr;

    static LoadInstPtr CreatePtr(BaseValuePtr, BaseValuePtr);
public:
    LoadInst(BaseValuePtr, BaseValuePtr);
    ~LoadInst() = default;

    static BaseValuePtr DoLoadValue(BaseValuePtr, BlockPtr);

    std::string tollvmIR();
};

class GetElementPtrInst;
using GepInstPtr = std::shared_ptr<GetElementPtrInst>;
using OffsetList = std::list<BaseValuePtr>;

class GetElementPtrInst : public Instruction {
private:
    BaseValuePtr target_ptr;
    BaseTypePtr  store_type;
    BaseValuePtr base_addr;
    OffsetList   offset_list;

    static GepInstPtr CreatePtr(BaseValuePtr, BaseTypePtr, BaseValuePtr, OffsetList);
public:
    GetElementPtrInst(BaseValuePtr, BaseTypePtr, BaseValuePtr, OffsetList);
    ~GetElementPtrInst() = default;

    static BaseValuePtr DoGetPointer(BaseTypePtr, BaseValuePtr, OffsetList, BlockPtr);

    std::string tollvmIR();
};
