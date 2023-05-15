#pragma once

#include <asm.hh>

#define RVINST(name, ...)             \
    class RV_##name : public RVInst { \
       public:                        \
        RV_##name(__VA_ARGS__);       \
    };

using rid_t = uint64_t;
using i32 = int32_t;
using cstr = const char *;

enum class opKind { MEMR, MEMW, BJ, FLT, MDR, ALU, FENCE };

class RVInst : public ASMInst {
   public:
    RVInst(rid_t rd = 0, rid_t rs1 = 0, rid_t rs2 = 0, rid_t rs3 = 0);
    ~RVInst();
    virtual std::string_view toString();
    virtual std::string_view Comment();
    virtual void setComment(cstr comt);
    virtual void setMAttr(uint64_t lty, opKind opk);

   public:
    rid_t dst{};
    rid_t src1{};
    rid_t src2{};
    rid_t src3{};

   public:
    uint64_t latency{};
    opKind opkind{opKind::FENCE};

   protected:
    char *stat;
    size_t statlen;
    cstr comt_;
};

class RVBasicBlock : public ASMBasicBlock {};

//
// memory operations
//

RVINST(LI, rid_t rd, i32 imm);     // imm => R[rd] 加载常数
RVINST(LA$, rid_t rd, cstr sym);   // addr(sym) => R[rd] 加载绝对地址
RVINST(LLA$, rid_t rd, cstr sym);  // addr(sym) => R[rd] 加载本地地址
RVINST(LEA$, rid_t rd, cstr sym);  // addr(sym) => R[rd] 加载符号地址

RVINST(LW, rid_t rd, rid_t rb,
       i32 off);  // @word(off + R[rb]) => R[rd] 加载字数据
RVINST(LD, rid_t rd, rid_t rb,
       i32 off);  // @dword(off + R[rb]) => R[rd] 加载双字数据

// @uword(off + R[rb]) => R[rd] 加载无符号字数据
RVINST(LWU, rid_t rd, rid_t rb, i32 off);

RVINST(SW, rid_t rs, rid_t rb,
       i32 off);  // R[rs] => @word(off + R[rb]) 存储字数据
RVINST(SD, rid_t rs, rid_t rb,
       i32 off);  // R[rs] => @dword(off + R[rb]) 存储双字数据

RVINST(LW$, rid_t rd, cstr sym);  // R[rs] => @(sym) 存储字数据到符号
RVINST(LD$, rid_t rd, cstr sym);  // R[rs] => @(sym) 存储字数据到符号

RVINST(LWU$, rid_t rd, cstr sym);

RVINST(SW$, rid_t rs, cstr sym);
RVINST(SD$, rid_t rs, cstr sym);

//
// jmp operations
//

RVINST(JR, rid_t rs);
RVINST(JALR, rid_t rs);

RVINST(J$, cstr sym);
RVINST(JAL$, cstr sym);

RVINST(RET);

RVINST(CALL$, cstr sym);
RVINST(TAIL$, cstr sym);

//
// branch operation
//

RVINST(BEQZ$, rid_t rs, cstr sym);
RVINST(BNEZ$, rid_t rs, cstr sym);
RVINST(BLEZ$, rid_t rs, cstr sym);
RVINST(BGEZ$, rid_t rs, cstr sym);
RVINST(BLTZ$, rid_t rs, cstr sym);
RVINST(BGTZ$, rid_t rs, cstr sym);

RVINST(BGT$, rid_t lhs, rid_t rhs, cstr sym);
RVINST(BLE$, rid_t lhs, rid_t rhs, cstr sym);

//
// mem-misc operation
//

RVINST(FENCE);

//
// misc operation
//

RVINST(NOP);
RVINST(MV, rid_t rd, rid_t rs);
RVINST(NOT, rid_t rd, rid_t rs);
RVINST(NEGW, rid_t rd, rid_t rs);
RVINST(SEXT_W, rid_t rd, rid_t rs);

RVINST(SEQZ, rid_t rd, rid_t rs);
RVINST(SNEZ, rid_t rd, rid_t rs);
RVINST(SLTZ, rid_t rd, rid_t rs);
RVINST(SGTZ, rid_t rd, rid_t rs);

//
// float misc operation
//

RVINST(FMV_S, rid_t frd, rid_t frs);
RVINST(FABS_S, rid_t frd, rid_t frs);
RVINST(FNEG_S, rid_t frd, rid_t frs);

//
// RV32I
//

RVINST(SLTI, rid_t rd, rid_t rs, i32 imm);
RVINST(XORI, rid_t rd, rid_t rs, i32 imm);
RVINST(ORI, rid_t rd, rid_t rs, i32 imm);
RVINST(ANDI, rid_t rd, rid_t rs, i32 imm);
RVINST(SLLI, rid_t rd, rid_t rs, i32 imm);
RVINST(SRLI, rid_t rd, rid_t rs, i32 imm);
RVINST(SRAI, rid_t rd, rid_t rs, i32 imm);

//
// RV64I
//

RVINST(ADDIW, rid_t rd, rid_t rs, i32 imm);
RVINST(SLLIW, rid_t rd, rid_t rs, i32 imm);
RVINST(SRLIW, rid_t rd, rid_t rs, i32 imm);
RVINST(SRAIW, rid_t rd, rid_t rs, i32 imm);

RVINST(ADDW, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(SUBW, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(SLLW, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(SRLW, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(SRAW, rid_t rd, rid_t rs1, rid_t rs2);

//
// RV64M
//

RVINST(MULW, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(DIVW, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(REMW, rid_t rd, rid_t rs1, rid_t rs2);

//
// RV32F
//

RVINST(FLW, rid_t frd, rid_t rb, i32 off);
RVINST(FSW, rid_t frd, rid_t rb, i32 off);

RVINST(FMADD_S, rid_t frd, rid_t frs1, rid_t frs2, rid_t frs3);
RVINST(FMSUB_S, rid_t frd, rid_t frs1, rid_t frs2, rid_t frs3);

RVINST(FNMADD_S, rid_t frd, rid_t frs1, rid_t frs2, rid_t frs3);
RVINST(FNMSUB_S, rid_t frd, rid_t frs1, rid_t frs2, rid_t frs3);

RVINST(FADD_S, rid_t frd, rid_t frs1, rid_t frs2);
RVINST(FSUB_S, rid_t frd, rid_t frs1, rid_t frs2);
RVINST(FMUL_S, rid_t frd, rid_t frs1, rid_t frs2);
RVINST(FDIV_S, rid_t frd, rid_t frs1, rid_t frs2);

RVINST(FSQRT_S, rid_t frd, rid_t frs1);

RVINST(FSGNJ_S, rid_t frd, rid_t frs1, rid_t frs2);
RVINST(FSGNJN_S, rid_t frd, rid_t frs1, rid_t frs2);
RVINST(FSGNJX_S, rid_t frd, rid_t frs1, rid_t frs2);

RVINST(FMIN_S, rid_t frd, rid_t frs1, rid_t frs2);
RVINST(FMAX_S, rid_t frd, rid_t frs1, rid_t frs2);

RVINST(FCVT_W_S, rid_t rd, rid_t frs);

RVINST(FCVT_S_W, rid_t frd, rid_t rs);

RVINST(FEQ_S, rid_t rd, rid_t frs1, rid_t frs2);
RVINST(FLT_S, rid_t rd, rid_t frs1, rid_t frs2);
RVINST(FLE_S, rid_t rd, rid_t frs1, rid_t frs2);
RVINST(FCLASS_S, rid_t rd, rid_t frs);

//
// RV32A
// TODO: implement it
