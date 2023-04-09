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

class GetElementPtrInst : public Instruction {
private:
    BaseValuePtr target_ptr;
    ListTypePtr  base_type;
    BaseValuePtr base_addr;
    BaseValuePtr offset;

    static GepInstPtr CreatePtr(BaseValuePtr, ListTypePtr, BaseValuePtr, BaseValuePtr);
public:
    GetElementPtrInst(BaseValuePtr, ListTypePtr, BaseValuePtr, BaseValuePtr);
    ~GetElementPtrInst() = default;

    static BaseValuePtr DoGetPointer(ListTypePtr, BaseValuePtr, BaseValuePtr, BlockPtr);

    std::string tollvmIR();
};
