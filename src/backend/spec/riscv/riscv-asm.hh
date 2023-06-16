#pragma once

#include "../asm.hh"

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

constexpr size_t $zero = 0;
constexpr size_t $ra = 1;
constexpr size_t $sp = 2;
constexpr size_t $gp = 3;
constexpr size_t $tp = 4;
constexpr size_t $t0 = 5;
constexpr size_t $t1 = 5 + 1;
constexpr size_t $t2 = 5 + 2;
constexpr size_t $fp = 8;

constexpr size_t $a0 = 10;
constexpr size_t $a1 = 10 + 1;
constexpr size_t $a2 = 10 + 2;
constexpr size_t $a3 = 10 + 3;
constexpr size_t $a4 = 10 + 4;
constexpr size_t $a5 = 10 + 5;
constexpr size_t $a6 = 10 + 6;
constexpr size_t $a7 = 10 + 7;

constexpr size_t $t6 = 31;

constexpr size_t $fa0 = 10 + 32;
constexpr size_t $fa1 = 10 + 1 + 32;
constexpr size_t $fa2 = 10 + 2 + 32;
constexpr size_t $fa3 = 10 + 3 + 32;
constexpr size_t $fa4 = 10 + 4 + 32;
constexpr size_t $fa5 = 10 + 5 + 32;

constexpr size_t $ft0 = 0 + 32;
constexpr size_t $ft1 = 1 + 32;
constexpr size_t $ft2 = 2 + 32;
constexpr size_t $ft3 = 3 + 32;
constexpr size_t $ft4 = 4 + 32;

#define RVINST(name, ...)             \
    class RV_##name : public RVInst { \
       public:                        \
        RV_##name(__VA_ARGS__);       \
    };

//
// memory operations
//

RVINST(LI, rid_t rd, i32 imm);     // imm => R[rd] 加载常数
RVINST(LA$, rid_t rd, cstr sym);   // addr(sym) => R[rd] 加载绝对地址
RVINST(LLA$, rid_t rd, cstr sym);  // addr(sym) => R[rd] 加载本地地址
RVINST(LEA$, rid_t rd, cstr sym);  // addr(sym) => R[rd] 加载符号地址

RVINST(LW, rid_t rd, rid_t rb, i32 off);  // @word(off + R[rb]) => R[rd] 加载字数据
RVINST(LD, rid_t rd, rid_t rb, i32 off);  // @dword(off + R[rb]) => R[rd] 加载双字数据

RVINST(LWU, rid_t rd, rid_t rb, i32 off);  // @uword(off + R[rb]) => R[rd] 加载无符号字数据

RVINST(SW, rid_t rs, rid_t rb, i32 off);  // R[rs] => @word(off + R[rb]) 存储字数据
RVINST(SD, rid_t rs, rid_t rb, i32 off);  // R[rs] => @dword(off + R[rb]) 存储双字数据

RVINST(LW$, rid_t rd, cstr sym);  // R[rs] => @(sym) 存储字数据到符号
RVINST(LD$, rid_t rd, cstr sym);  // R[rs] => @(sym) 存储字数据到符号

RVINST(LWU$, rid_t rd, cstr sym);

RVINST(SW$, rid_t rs, cstr sym, rid_t rt);
RVINST(SD$, rid_t rs, cstr sym, rid_t rt);

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
RVINST(FMV_S_W, rid_t frd, rid_t rs);
RVINST(FMV_W_S, rid_t rd, rid_t frs);
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
RVINST(ADDI, rid_t rd, rid_t rs, i32 imm);

RVINST(ADD, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(SUB, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(XOR, rid_t rd, rid_t rs1, rid_t rs2);
RVINST(SLT, rid_t rd, rid_t rs1, rid_t rs2);

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
RVINST(FSW, rid_t frs, rid_t rb, i32 off);

RVINST(FLW$, rid_t frd, cstr sym);
RVINST(FSW$, rid_t frs, cstr sym, rid_t rt);

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