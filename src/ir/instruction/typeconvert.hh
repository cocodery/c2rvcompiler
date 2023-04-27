#pragma once

#include "cfgNode.hh"
#include "compareInst.hh"
#include "instruction.hh"
#include "valueHeader.hh"

class SitoFpInst;
using SitoFpInstPtr = std::shared_ptr<SitoFpInst>;

class SitoFpInst : public Instruction {
   private:
    VariablePtr fp_value;
    BaseValuePtr si_value;

    static SitoFpInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    SitoFpInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~SitoFpInst() = default;

    static VariablePtr DoSitoFp(BaseValuePtr, CfgNodePtr);

    std::string tollvmIR();
};

class FptoSiInst;
using FptoSiInstPtr = std::shared_ptr<FptoSiInst>;

class FptoSiInst : public Instruction {
   private:
    VariablePtr si_value;
    BaseValuePtr fp_value;

    static FptoSiInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    FptoSiInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~FptoSiInst() = default;

    static VariablePtr DoFptoSi(ATTR_TYPE, BaseValuePtr, CfgNodePtr);

    std::string tollvmIR();
};

class ZextInst;
using ZextInstPtr = std::shared_ptr<ZextInst>;

class ZextInst : public Instruction {
   private:
    VariablePtr l_value;
    BaseValuePtr s_value;

    static ZextInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    ZextInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~ZextInst() = default;

    static VariablePtr DoZeroExt(BaseValuePtr, CfgNodePtr);

    std::string tollvmIR();
};
