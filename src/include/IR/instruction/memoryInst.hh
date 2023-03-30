#pragma once

#include "../function/basicblock.hh"
#include "instruction.hh"

class AllocaInst;
using AllocaInstPtr = std::shared_ptr<AllocaInst>;

class AllocaInst : public Instruction {
private:
    BaseTypePtr  alloca_type;
    BaseValuePtr alloca_addr;
public:
    AllocaInst(BaseTypePtr, BaseValuePtr);
    ~AllocaInst() = default;

    static AllocaInstPtr CreatePtr(BaseTypePtr, BaseValuePtr);

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

    static StoreInstPtr StoreValue2Mem(BaseValuePtr, BaseValuePtr, BlockPtr);

    std::string tollvmIR();
};

class LoadInst;
using LoadInstPtr = std::shared_ptr<LoadInst>;

class LoadInst : public Instruction {
private:
    BaseValuePtr load_value;
    BaseValuePtr load_addr;
public:
    LoadInst(BaseValuePtr, BaseValuePtr);

    static LoadInstPtr CreatePtr(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};
