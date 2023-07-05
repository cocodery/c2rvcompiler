#pragma once

#include <cstdint>
#include <vector>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/ir/instruction/opCode.hh"

class VirtualRegister;
class AsmBasicBlock;

enum class COMP_KIND {
    // 比较枚举对应 opCode

    LTH = OP_LTH,
    GTH = OP_GTH,
    LEQ = OP_LEQ,
    GEQ = OP_GEQ,
    EQU = OP_EQU,
    NEQ = OP_NEQ,
};

enum class IBIN_KIND {
    // 运算枚举对应 opCode

    ADD = OP_ADD,
    SUB = OP_SUB,
    MUL = OP_MUL,
    DIV = OP_DIV,
    REM = OP_REM,
    SLL = OP_LSHIFT,
    SRA = OP_RSHIFT,
    XOR,
    AND,
    OR,
};

enum class FBIN_KIND {
    // 运算枚举对应 opCode

    ADD = OP_ADD,
    SUB = OP_SUB,
    MUL = OP_MUL,
    DIV = OP_DIV
};

enum class PHI_KIND {
    IMM,
    REG
};

class UopGeneral : public Serializable {
   protected:
    size_t uop_idx_;

   public:
    size_t GetUopIdx() const;
    void SetUopIdx(size_t idx);

    virtual const std::vector<VirtualRegister *> GetOperands() const = 0;
    virtual VirtualRegister *GetResult() const = 0;

    virtual ~UopGeneral() = default;
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
};

class UopCall : public InternalUop<UopCall> {
    std::vector<VirtualRegister *> params_;

    VirtualRegister *retval_;

    std::string callee_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetRetVal(VirtualRegister *retval);
    void SetCallee(std::string &callee);

    void PushParam(VirtualRegister *param);
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
};

class UopJump : public InternalUop<UopJump> {
    size_t dst_idx_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetDstIdx(size_t dst_idx);
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
};

class UopIBinImm : public InternalUop<UopIBinImm> {
    VirtualRegister *lhs_;
    uint32_t imm_lo12_;

    VirtualRegister *dst_;

    IBIN_KIND kind_;

    void formatString(FILE *fp) final;

   public:
    IBIN_KIND GetKind() const;

    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;

    void SetLhs(VirtualRegister *lhs);
    void SetImm(uint32_t imm);
    void SetDst(VirtualRegister *dst);
    void SetKind(IBIN_KIND kind);
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
    void SetKind(COMP_KIND kind);
};