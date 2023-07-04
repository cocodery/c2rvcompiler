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
    SRA = OP_RSHIFT
};

enum class FBIN_KIND {
    // 运算枚举对应 opCode

    ADD = OP_ADD,
    SUB = OP_SUB,
    MUL = OP_MUL,
    DIV = OP_DIV
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
class InternalUop : UopGeneral {};

class UopRet : public InternalUop<UopRet> {
    VirtualRegister *src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopCall : public InternalUop<UopCall> {
    std::vector<VirtualRegister *> params_;

    VirtualRegister *retval_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopLui : public InternalUop<UopLui> {
    VirtualRegister *dst_;

    uint32_t imm_up20_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopMv : public InternalUop<UopMv> {
    VirtualRegister *dst_;
    VirtualRegister *src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopCvtS2W : public InternalUop<UopCvtS2W> {
    VirtualRegister *dst_;
    VirtualRegister *src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopCvtW2S : public InternalUop<UopCvtW2S> {
    VirtualRegister *dst_;
    VirtualRegister *src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopBranch : public InternalUop<UopBranch> {
    VirtualRegister *cond_;

    size_t dst_idx_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopICmpBranch : public InternalUop<UopICmpBranch> {
    VirtualRegister *lhs_;
    VirtualRegister *rhs_;

    size_t dst_idx_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopJump : public InternalUop<UopJump> {
    size_t dst_idx_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopLLa : public InternalUop<UopLLa> {
    VirtualRegister *dst_;

    std::string src_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopLoad : public InternalUop<UopLoad> {
    VirtualRegister *dst_;
    VirtualRegister *base_;

    int32_t off_lo12_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopStore : public InternalUop<UopStore> {
    VirtualRegister *src_;
    VirtualRegister *base_;

    int32_t off_lo12_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopFLoad : public InternalUop<UopFLoad> {
    VirtualRegister *dst_;
    VirtualRegister *base_;

    int32_t off_lo12_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
};

class UopFStore : public InternalUop<UopFStore> {
    VirtualRegister *src_;
    VirtualRegister *base_;

    int32_t off_lo12_;

    void formatString(FILE *fp) final;

   public:
    const std::vector<VirtualRegister *> GetOperands() const;
    VirtualRegister *GetResult() const;
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
};