#pragma once

#include "compareInst.hh"
#include "instruction.hh"
#include "ir/function/cfgNode.hh"
#include "ir/valueHeader.hh"

class SitoFpInst;
using SitoFpInstPtr = std::shared_ptr<SitoFpInst>;

class SitoFpInst : public UnaryInstruction {
   private:
    static SitoFpInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    SitoFpInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~SitoFpInst() = default;

    static VariablePtr DoSitoFp(BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

class FptoSiInst;
using FptoSiInstPtr = std::shared_ptr<FptoSiInst>;

class FptoSiInst : public UnaryInstruction {
   private:
    static FptoSiInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    FptoSiInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~FptoSiInst() = default;

    static VariablePtr DoFptoSi(ATTR_TYPE, BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};

class ZextInst;
using ZextInstPtr = std::shared_ptr<ZextInst>;

class ZextInst : public UnaryInstruction {
   private:
    static ZextInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    ZextInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~ZextInst() = default;

    static VariablePtr DoZeroExt(BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr);

    std::string tollvmIR();
};
