#pragma once

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/instruction.hh"

class IBinaryInst;
using IBinaryInstPtr = std::shared_ptr<IBinaryInst>;

class IBinaryInst final : public BinaryInstruction {
   private:
    static IBinaryInstPtr CreatePtr(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

   public:
    IBinaryInst(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~IBinaryInst() = default;

    bool IsIBinaryInst() const final override;

    static VariablePtr DoIBinOperate(OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr) final override;

    std::string tollvmIR() final override;

    void TranslateTo(InternalTranslation &itx) final override;
};

class FBinaryInst;
using FBinaryInstPtr = std::shared_ptr<FBinaryInst>;

class FBinaryInst final : public BinaryInstruction {
   private:
    static FBinaryInstPtr CreatePtr(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

   public:
    FBinaryInst(VariablePtr, OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);
    ~FBinaryInst() = default;

    bool IsFBinaryInst() const final override;

    static VariablePtr DoFBinOperate(OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr) final override;

    std::string tollvmIR() final override;

    void TranslateTo(InternalTranslation &itx) final override;
};

class FNegInst;
using FNegInstPtr = std::shared_ptr<FNegInst>;
class FNegInst final : public UnaryInstruction {
   private:
    static FNegInstPtr CreatePtr(VariablePtr, BaseValuePtr, CfgNodePtr);

   public:
    FNegInst(VariablePtr, BaseValuePtr, CfgNodePtr);
    ~FNegInst() = default;

    static VariablePtr DoFloatNeg(BaseValuePtr, CfgNodePtr);

    bool ReplaceSRC(BaseValuePtr, BaseValuePtr) final override;

    std::string tollvmIR() final override;

    void TranslateTo(InternalTranslation &itx) final override;
};
