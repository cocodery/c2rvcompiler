#pragma once

#include <cstdint>
#include <vector>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/rl/Enums.hh"
#include "3tle3wa/ir/instruction/opCode.hh"

class VirtualRegister;
class AsmBasicBlock;
class RLPlanner;

class UopGeneral : public Serializable {
   protected:
    size_t uop_idx_;

   public:
    size_t GetUopIdx() const;
    void SetUopIdx(size_t idx);

    virtual const std::vector<VirtualRegister *> GetOperands() const = 0;
    virtual VirtualRegister *GetResult() const = 0;

    virtual OPERATION_KIND GetOpKind() const = 0;

    virtual ~UopGeneral() = default;

    virtual void ToAsm(AsmBasicBlock *abb, RLPlanner *plan) = 0;
};

template <typename T>
class InternalUop : public UopGeneral {};

class UopRet : public InternalUop<UopRet> {
    VirtualRegister *retval_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetRetVal(VirtualRegister *retval);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopCall : public InternalUop<UopCall> {
    std::vector<VirtualRegister *> params_;

    VirtualRegister *retval_;

    std::string callee_;

    std::vector<VirtualRegister *> living_regs_;

    bool libcall_{false};
    bool tailcall_{false};
    bool callself_{false};

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetRetVal(VirtualRegister *retval);
    void SetCallee(std::string &callee);
    void SetLibCall(bool libcall);
    void SetTailCall(bool tailcall);
    void SetCallSelf(bool callself);

    void PushParam(VirtualRegister *param);
    void PushLiver(VirtualRegister *liver);

    void BroadCastCall(size_t lbidx);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopLui : public InternalUop<UopLui> {
    VirtualRegister *dst_;

    uint32_t imm_up20_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDst(VirtualRegister *dst);
    void SetImm(uint32_t imm);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopMv : public InternalUop<UopMv> {
    VirtualRegister *dst_;
    VirtualRegister *src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDst(VirtualRegister *dst);
    void SetSrc(VirtualRegister *src);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopCvtS2W : public InternalUop<UopCvtS2W> {
    VirtualRegister *dst_;
    VirtualRegister *src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDst(VirtualRegister *dst);
    void SetSrc(VirtualRegister *src);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopCvtW2S : public InternalUop<UopCvtW2S> {
    VirtualRegister *dst_;
    VirtualRegister *src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDst(VirtualRegister *dst);
    void SetSrc(VirtualRegister *src);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopBranch : public InternalUop<UopBranch> {
    VirtualRegister *cond_;

    size_t dst_idx_;
    bool on_true_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetCond(VirtualRegister *cond);
    void SetOnTrue(bool cond);
    void SetDstIdx(size_t dst_idx);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopJump : public InternalUop<UopJump> {
    size_t dst_idx_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDstIdx(size_t dst_idx);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopLla : public InternalUop<UopLla> {
    VirtualRegister *dst_;

    std::string src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDst(VirtualRegister *dst);
    void SetSrc(std::string &src);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopLoad : public InternalUop<UopLoad> {
    VirtualRegister *dst_;
    VirtualRegister *base_;

    int32_t off_lo12_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDst(VirtualRegister *dst);
    void SetBase(VirtualRegister *base);
    void SetOff(int32_t off);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopStore : public InternalUop<UopStore> {
    VirtualRegister *src_;
    VirtualRegister *base_;

    int32_t off_lo12_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetSrc(VirtualRegister *src);
    void SetBase(VirtualRegister *base);
    void SetOff(int32_t off);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopFLoad : public InternalUop<UopFLoad> {
    VirtualRegister *dst_;
    VirtualRegister *base_;

    int32_t off_lo12_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDst(VirtualRegister *dst);
    void SetBase(VirtualRegister *base);
    void SetOff(int32_t off);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopFStore : public InternalUop<UopFStore> {
    VirtualRegister *src_;
    VirtualRegister *base_;

    int32_t off_lo12_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetSrc(VirtualRegister *src);
    void SetBase(VirtualRegister *base);
    void SetOff(int32_t off);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopICmp : public InternalUop<UopICmp> {
    VirtualRegister *lhs_;
    VirtualRegister *rhs_;

    VirtualRegister *dst_;

    COMP_KIND kind_;

    void formatString(FILE *fp) final;

   public:
    COMP_KIND GetKind() const;

    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetLhs(VirtualRegister *lhs);
    void SetRhs(VirtualRegister *rhs);
    void SetDst(VirtualRegister *dst);
    void SetKind(COMP_KIND kind);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopFCmp : public InternalUop<UopFCmp> {
    VirtualRegister *lhs_;
    VirtualRegister *rhs_;

    VirtualRegister *dst_;

    COMP_KIND kind_;

    void formatString(FILE *fp) final;

   public:
    COMP_KIND GetKind() const;

    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetLhs(VirtualRegister *lhs);
    void SetRhs(VirtualRegister *rhs);
    void SetDst(VirtualRegister *dst);
    void SetKind(COMP_KIND kind);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopIBin : public InternalUop<UopIBin> {
    VirtualRegister *lhs_;
    VirtualRegister *rhs_;

    VirtualRegister *dst_;

    IBIN_KIND kind_;

    void formatString(FILE *fp) final;

   public:
    IBIN_KIND GetKind() const;

    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetLhs(VirtualRegister *lhs);
    void SetRhs(VirtualRegister *rhs);
    void SetDst(VirtualRegister *dst);
    void SetKind(IBIN_KIND kind);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopIBinImm : public InternalUop<UopIBinImm> {
    VirtualRegister *lhs_;
    int32_t imm_lo12_;

    VirtualRegister *dst_;

    IBIN_KIND kind_;

    void formatString(FILE *fp) final;

   public:
    IBIN_KIND GetKind() const;

    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetLhs(VirtualRegister *lhs);
    void SetImm(int32_t imm);
    void SetDst(VirtualRegister *dst);
    void SetKind(IBIN_KIND kind);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopFBin : public InternalUop<UopFBin> {
    VirtualRegister *lhs_;
    VirtualRegister *rhs_;

    VirtualRegister *dst_;

    FBIN_KIND kind_;

    void formatString(FILE *fp) final;

   public:
    FBIN_KIND GetKind() const;

    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetLhs(VirtualRegister *lhs);
    void SetRhs(VirtualRegister *rhs);
    void SetDst(VirtualRegister *dst);
    void SetKind(FBIN_KIND kind);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

class UopICmpBranch : public InternalUop<UopICmpBranch> {
    VirtualRegister *lhs_;
    VirtualRegister *rhs_;

    size_t dst_idx_;

    COMP_KIND kind_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetLhs(VirtualRegister *lhs);
    void SetRhs(VirtualRegister *rhs);
    void SetDstIdx(size_t dst_idx);
    void SetKind(COMP_KIND kind);

    OPERATION_KIND GetOpKind() const;
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};

// for phi operation

struct PhiOperand {
    PHI_KIND kind;
    size_t data;
    size_t lbidx;
};

class UopPhi : public InternalUop<UopPhi> {
    std::vector<PhiOperand> operands_;

    VirtualRegister *dst_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void PushOperand(PhiOperand &operand);
    void SetDst(size_t dst_idx);

    OPERATION_KIND GetOpKind() const { return OPERATION_KIND::INTOPT; };
    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};