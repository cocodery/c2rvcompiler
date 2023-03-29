#pragma once

#include "instruction.hh"

class AllocaInst;
using AllocaInstPtr = std::shared_ptr<AllocaInst>;

class AllocaInst : public Instruction {
private:
    BaseTypePtr  alloca_type;
    BaseValuePtr alloca_addr;
public:
    AllocaInst(TypeID, BaseValuePtr);
    AllocaInst(ListTypePtr, BaseValuePtr);
    ~AllocaInst() = default;

    static AllocaInstPtr CreatePtr(TypeID, BaseValuePtr);
    static AllocaInstPtr CreatePtr(ListTypePtr, BaseValuePtr);

    std::string tollvmIR();
};

class StoreInst;
using StoreInstPtr = std::shared_ptr<StoreInst>;

class StoreInst : public Instruction {
private:
    BaseValuePtr store_addr;
    BaseValuePtr store_value;
public:
    StoreInst(BaseValuePtr, BaseValuePtr);

    static StoreInstPtr CreatePtr(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};
