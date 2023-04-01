#pragma once

#include "valueHeader.hh"
#include "../function/basicblock.hh"
#include "instruction.hh"

class SitoFpInst;
using SitoFpInstPtr = std::shared_ptr<SitoFpInst>;

class SitoFpInst : public Instruction {
private:
    BaseValuePtr fp_value;
    BaseValuePtr si_value;
public:
    SitoFpInst(BaseValuePtr, BaseValuePtr);
    ~SitoFpInst() = default;

    static SitoFpInstPtr CreatePtr(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

class FptoSiInst;
using FptoSiInstPtr = std::shared_ptr<FptoSiInst>;

class FptoSiInst : public Instruction {
private:
    BaseValuePtr si_value;
    BaseValuePtr fp_value;
public:
    FptoSiInst(BaseValuePtr, BaseValuePtr);
    ~FptoSiInst() = default;

    static FptoSiInstPtr CreatePtr(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

class ZextInst;
using ZextInstPtr = std::shared_ptr<ZextInst>;

class ZextInst : public Instruction {
private:
    BaseValuePtr l_value;
    BaseValuePtr s_value;
public:
    ZextInst(BaseValuePtr, BaseValuePtr);
    ~ZextInst() = default;

    static ZextInstPtr CreatePtr(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

BaseValuePtr scalarTypeConvert(TypeID, BaseValuePtr, BlockPtr);
