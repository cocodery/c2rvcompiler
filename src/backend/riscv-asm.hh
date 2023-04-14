#pragma once

#include "asm.hh"

#define RVINST(name, ...)                                                      \
  class RV_##name : public RVInst {                                         \
  public:                                                                      \
    RV_##name(__VA_ARGS__);                                                    \
  };

using u8 = uint8_t;
using u64 = uint64_t;
using i32 = int32_t;
using cstr = const char *;

struct REGinterface {
  u8 dst{};
  u8 src1{};
  u8 src2{};
  u8 fdst{32};
  u8 fsrc1{32};
  u8 fsrc2{32};
  u8 fsrc3{32};
};

class RVInst : public ASMInst {
public:
  RVInst(u8 rd, u8 rs1, u8 rs2);
  RVInst(REGinterface &&rfc);
  ~RVInst();
  virtual std::string_view toString();
  virtual std::string_view Comment();
  virtual void setComment(cstr comt);

protected:
  u8 dst{};
  u8 src1{};
  u8 src2{};
  u8 fdst{};
  u8 fsrc1{};
  u8 fsrc2{};
  u8 fsrc3{};

  char *stat;
  size_t statlen;
  cstr comt_;
};

//
// memory operations
//

RVINST(LI, u8 rd, i32 imm);    // imm => R[rd] 加载常数
RVINST(LA$, u8 rd, cstr sym);  // addr(sym) => R[rd] 加载绝对地址
RVINST(LLA$, u8 rd, cstr sym); // addr(sym) => R[rd] 加载本地地址
RVINST(LEA$, u8 rd, cstr sym); // addr(sym) => R[rd] 加载符号地址

RVINST(LW, u8 rd, u8 rb, i32 off); // @word(off + R[rb]) => R[rd] 加载字数据
RVINST(LD, u8 rd, u8 rb, i32 off); // @dword(off + R[rb]) => R[rd] 加载双字数据

// @uword(off + R[rb]) => R[rd] 加载无符号字数据
RVINST(LWU, u8 rd, u8 rb, i32 off);

RVINST(SW, u8 rs, u8 rb, i32 off); // R[rs] => @word(off + R[rb]) 存储字数据
RVINST(SD, u8 rs, u8 rb, i32 off); // R[rs] => @dword(off + R[rb]) 存储双字数据

RVINST(LW$, u8 rd, cstr sym); // R[rs] => @(sym) 存储字数据到符号
RVINST(LD$, u8 rd, cstr sym); // R[rs] => @(sym) 存储字数据到符号

RVINST(LWU$, u8 rd, cstr sym);

RVINST(SW$, u8 rs, cstr sym);
RVINST(SD$, u8 rs, cstr sym);

//
// jmp operations
//

RVINST(JR, u8 rs);
RVINST(JALR, u8 rs);

RVINST(J$, cstr sym);
RVINST(JAL$, cstr sym);

RVINST(RET);

RVINST(CALL$, cstr sym);
RVINST(TAIL$, cstr sym);

//
// branch operation
//

RVINST(BEQZ$, u8 rs, cstr sym);
RVINST(BNEZ$, u8 rs, cstr sym);
RVINST(BLEZ$, u8 rs, cstr sym);
RVINST(BGEZ$, u8 rs, cstr sym);
RVINST(BLTZ$, u8 rs, cstr sym);
RVINST(BGTZ$, u8 rs, cstr sym);

RVINST(BGT$, u8 lhs, u8 rhs, cstr sym);
RVINST(BLE$, u8 lhs, u8 rhs, cstr sym);

//
// mem-misc operation
//

RVINST(FENCE);

//
// misc operation
//

RVINST(NOP);
RVINST(MV, u8 rd, u8 rs);
RVINST(NOT, u8 rd, u8 rs);
RVINST(NEGW, u8 rd, u8 rs);
RVINST(SEXT_W, u8 rd, u8 rs);

RVINST(SEQZ, u8 rd, u8 rs);
RVINST(SNEZ, u8 rd, u8 rs);
RVINST(SLTZ, u8 rd, u8 rs);
RVINST(SGTZ, u8 rd, u8 rs);

//
// float misc operation
//

RVINST(FMV_S, u8 frd, u8 frs);
RVINST(FABS_S, u8 frd, u8 frs);
RVINST(FNEG_S, u8 frd, u8 frs);

//
// RV32I
//

RVINST(SLTI, u8 rd, u8 rs, i32 imm);
RVINST(XORI, u8 rd, u8 rs, i32 imm);
RVINST(ORI, u8 rd, u8 rs, i32 imm);
RVINST(ANDI, u8 rd, u8 rs, i32 imm);
RVINST(SLLI, u8 rd, u8 rs, i32 imm);
RVINST(SRLI, u8 rd, u8 rs, i32 imm);
RVINST(SRAI, u8 rd, u8 rs, i32 imm);

//
// RV64I
//

RVINST(ADDIW, u8 rd, u8 rs, i32 imm);
RVINST(SLLIW, u8 rd, u8 rs, i32 imm);
RVINST(SRLIW, u8 rd, u8 rs, i32 imm);
RVINST(SRAIW, u8 rd, u8 rs, i32 imm);

RVINST(ADDW, u8 rd, u8 rs1, u8 rs2);
RVINST(SUBW, u8 rd, u8 rs1, u8 rs2);
RVINST(SLLW, u8 rd, u8 rs1, u8 rs2);
RVINST(SRLW, u8 rd, u8 rs1, u8 rs2);
RVINST(SRAW, u8 rd, u8 rs1, u8 rs2);

//
// RV64M
//

RVINST(MULW, u8 rd, u8 rs1, u8 rs2);
RVINST(DIVW, u8 rd, u8 rs1, u8 rs2);
RVINST(REMW, u8 rd, u8 rs1, u8 rs2);

//
// RV32F
//

RVINST(FLW, u8 frd, u8 rb, i32 off);

RVINST(FMADD_S, u8 frd, u8 frs1, u8 frs2, u8 frs3);
RVINST(FMSUB_S, u8 frd, u8 frs1, u8 frs2, u8 frs3);

RVINST(FNMADD_S, u8 frd, u8 frs1, u8 frs2, u8 frs3);
RVINST(FNMSUB_S, u8 frd, u8 frs1, u8 frs2, u8 frs3);

RVINST(FADD_S, u8 frd, u8 frs1, u8 frs2);
RVINST(FSUB_S, u8 frd, u8 frs1, u8 frs2);
RVINST(FMUL_S, u8 frd, u8 frs1, u8 frs2);
RVINST(FDIV_S, u8 frd, u8 frs1, u8 frs2);

RVINST(FSQRT_S, u8 frd, u8 frs1);

RVINST(FSGNJ_S, u8 frd, u8 frs1, u8 frs2);
RVINST(FSGNJN_S, u8 frd, u8 frs1, u8 frs2);
RVINST(FSGNJX_S, u8 frd, u8 frs1, u8 frs2);

RVINST(FMIN_S, u8 frd, u8 frs1, u8 frs2);
RVINST(FMAX_S, u8 frd, u8 frs1, u8 frs2);

RVINST(FCVT_W_S, u8 rd, u8 frs);

RVINST(FCVT_S_W, u8 frd, u8 rs);

RVINST(FEQ_S, u8 rd, u8 frs1, u8 frs2);
RVINST(FLT_S, u8 rd, u8 frs1, u8 frs2);
RVINST(FLE_S, u8 rd, u8 frs1, u8 frs2);
RVINST(FCLASS_S, u8 rd, u8 frs);

//
// RV32A
// TODO: implement it
