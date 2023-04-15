#include <cassert>

#include "riscv-asm.hh"

#define COMMENT_BEGIN "\t# "
#define TAB "\t"
#define COMMA ","
#define NEWLINE "\n"

constexpr uint64_t LTY_ZERO = 0;
constexpr uint64_t LTY_ONE = 1;
constexpr uint64_t LTY_LOAD = 5;
constexpr uint64_t LTY_MUL = 3;
constexpr uint64_t LTY_DR = 35;
constexpr uint64_t LTY_RST = -1;

#define RVCNAM(name) RV_##name::RV_##name

#define GENSTAT(fmt, ...)                                                      \
  do {                                                                         \
    FILE *out = open_memstream(&stat, &statlen);                               \
    fprintf(out, TAB fmt, ##__VA_ARGS__);                                      \
    fflush(out);                                                               \
    fclose(out);                                                               \
  } while (0)

char nonstr[] = "";

static const char *rgnm[] = {
    // integer registers
    "zero", // x0
    "ra",   // x1
    "sp",   // x2
    "gp",   // x3
    "tp",   // x4
    "t0",   // x5
    "t1",   // x6
    "t2",   // x7
    "fp",   // x8 s0
    "s1",   // x9
    "a0",   // x10
    "a1",   // x11
    "a2",   // x12
    "a3",   // x13
    "a4",   // x14
    "a5",   // x15
    "a6",   // x16
    "a7",   // x17
    "s2",   // x18
    "s3",   // x19
    "s4",   // x20
    "s5",   // x21
    "s6",   // x22
    "s7",   // x23
    "s8",   // x24
    "s9",   // x25
    "s10",  // x26
    "s11",  // x27
    "t3",   // x28
    "t4",   // x29
    "t5",   // x30
    "t6",   // x31
    // float registers freg n idx = 32 + n
    "ft0",  // f0
    "ft1",  // f1
    "ft2",  // f2
    "ft3",  // f3
    "ft4",  // f4
    "ft5",  // f5
    "ft6",  // f6
    "ft7",  // f7
    "fs0",  // f8
    "fs1",  // f9
    "fa0",  // f10
    "fa1",  // f11
    "fa2",  // f12
    "fa3",  // f13
    "fa4",  // f14
    "fa5",  // f15
    "fa6",  // f16
    "fa7",  // f17
    "fs2",  // f18
    "fs3",  // f19
    "fs4",  // f20
    "fs5",  // f21
    "fs6",  // f22
    "fs7",  // f23
    "fs8",  // f24
    "fs9",  // f25
    "fs10", // f26
    "fs11", // f27
    "ft8",  // f28
    "ft9",  // f29
    "ft10", // f30
    "ft11", // f31
};

//
// ----- ----- Basic Class ----- -----
//

RVInst::RVInst(rid_t rd, rid_t rs1, rid_t rs2, rid_t rs3)
    : dst(rd), src1(rs1), src2(rs2), src3(rs3), stat(nonstr), statlen(0),
      comt_(COMMENT_BEGIN "error inst") {
  assert(rd < 64);
  assert(src1 < 64);
  assert(src2 < 64);
  assert(src3 < 64);
}

RVInst::~RVInst() {
  if (statlen)
    delete[] stat;
  stat = nullptr;
  statlen = 0;
}

std::string_view RVInst::toString() { return stat; }
std::string_view RVInst::Comment() { return comt_; }
void RVInst::setComment(cstr comt) { comt_ = comt; }

void RVInst::setMAttr(uint64_t lty, opKind opk) {
  latency = lty;
  opkind = opk;
}

//
// ----- ----- Instructions ----- -----
//

RVCNAM(LI)(rid_t rd, i32 imm) : RVInst(rd) {
  GENSTAT("li  " TAB "%s" COMMA "%d", rgnm[rd], imm);
  comt_ = COMMENT_BEGIN "load immediate";
  setMAttr(LTY_ONE, opKind::ALU);
}

RVCNAM(LA$)(rid_t rd, cstr sym) : RVInst(rd) {
  GENSTAT("la  " TAB "%s" COMMA "%s", rgnm[rd], sym);
  comt_ = COMMENT_BEGIN "load absolute address";
  setMAttr(LTY_ONE, opKind::ALU);
}

RVCNAM(LLA$)(rid_t rd, cstr sym) : RVInst(rd) {
  GENSTAT("lla " TAB "%s" COMMA "%s", rgnm[rd], sym);
  comt_ = COMMENT_BEGIN "load local address";
  setMAttr(LTY_ONE, opKind::ALU);
}

RVCNAM(LEA$)(rid_t rd, cstr sym) : RVInst(rd) {
  FILE *out = open_memstream(&stat, &statlen);
  fprintf(out, TAB "lui" TAB "%s" COMMA "%%hi(%s)" NEWLINE, rgnm[rd], sym);
  fprintf(out, TAB "addi" TAB "%s" COMMA "%s" COMMA "%%lo(%s)", rgnm[rd],
          rgnm[rd], sym);
  fflush(out);
  fclose(out);

  comt_ = COMMENT_BEGIN "load effective address";
  setMAttr(LTY_ONE, opKind::ALU);
}

RVCNAM(LW)(rid_t rd, rid_t rb, i32 off) : RVInst(rd, rb) {
  GENSTAT("lw  " TAB "%s" COMMA "%d(%s)", rgnm[rd], off, rgnm[rb]);
  comt_ = COMMENT_BEGIN "load word signed";
  setMAttr(LTY_LOAD, opKind::MEMR);
}

RVCNAM(LD)(rid_t rd, rid_t rb, i32 off) : RVInst(rd, rb) {
  GENSTAT("ld  " TAB "%s" COMMA "%d(%s)", rgnm[rd], off, rgnm[rb]);
  comt_ = COMMENT_BEGIN "load double word signed";
  setMAttr(LTY_LOAD, opKind::MEMR);
}

RVCNAM(LWU)(rid_t rd, rid_t rb, i32 off) : RVInst(rd, rb) {
  GENSTAT("lwu " TAB "%s" COMMA "%d(%s)", rgnm[rd], off, rgnm[rb]);
  comt_ = COMMENT_BEGIN "load word unsigned";
  setMAttr(LTY_LOAD, opKind::MEMR);
}

RVCNAM(SW)(rid_t rs, rid_t rb, i32 off) : RVInst(0, rs, rb) {
  GENSTAT("sw  " TAB "%s" COMMA "%d(%s)", rgnm[rs], off, rgnm[rb]);
  comt_ = COMMENT_BEGIN "store word";
  setMAttr(LTY_ONE, opKind::MEMW);
}

RVCNAM(SD)(rid_t rs, rid_t rb, i32 off) : RVInst(0, rs, rb) {
  GENSTAT("sd  " TAB "%s" COMMA "%d(%s)", rgnm[rs], off, rgnm[rb]);
  comt_ = COMMENT_BEGIN "store double word";
  setMAttr(LTY_ONE, opKind::MEMW);
}

RVCNAM(LW$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("lw  " TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "load word by symbol";
  setMAttr(LTY_LOAD, opKind::MEMR);
}

RVCNAM(LD$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("ld  " TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "load double word by symbol";
  setMAttr(LTY_LOAD, opKind::MEMR);
}

RVCNAM(LWU$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("lwu " TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "load word unsigned by symbol";
  setMAttr(LTY_LOAD, opKind::MEMR);
}

RVCNAM(SW$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("sw  " TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "store word by symbol";
  setMAttr(LTY_ONE, opKind::MEMW);
}

RVCNAM(SD$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("sd  " TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "store double word by symbol";
  setMAttr(LTY_ONE, opKind::MEMW);
}

RVCNAM(JR)(rid_t rs) : RVInst(0, rs) {
  GENSTAT("jr  " TAB "%s", rgnm[rs]);
  comt_ = COMMENT_BEGIN "jmp by addr in reg";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(JALR)(rid_t rs) : RVInst(0, rs) {
  GENSTAT("jalr" TAB "%s", rgnm[rs]);
  comt_ = COMMENT_BEGIN "jmp and link by addr in reg";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(J$)(cstr sym) : RVInst() {
  GENSTAT("j   " TAB "%s", sym);
  comt_ = COMMENT_BEGIN "jmp by symbol";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(JAL$)(cstr sym) : RVInst() {
  GENSTAT("jal " TAB "%s", sym);
  comt_ = COMMENT_BEGIN "jmp and link by symbol";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(RET)() : RVInst() {
  GENSTAT("ret ");
  comt_ = COMMENT_BEGIN "return";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(CALL$)(cstr sym) : RVInst() {
  GENSTAT("call" TAB "%s", sym);
  comt_ = COMMENT_BEGIN "call function";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(TAIL$)(cstr sym) : RVInst() {
  GENSTAT("tail" TAB "%s", sym);
  comt_ = COMMENT_BEGIN "tail call function";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(BEQZ$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("beqz" TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs == 0";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(BNEZ$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("bnez" TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs != 0";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(BLEZ$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("blez" TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs <= 0";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(BGEZ$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("bgez" TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs >= 0";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(BLTZ$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("bltz" TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs < 0";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(BGTZ$)(rid_t rs, cstr sym) : RVInst(0, rs) {
  GENSTAT("bgtz" TAB "%s" COMMA "%s", rgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs > 0";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(BGT$)(rid_t lhs, rid_t rhs, cstr sym) : RVInst(0, lhs, rhs) {
  GENSTAT("bgt " TAB "%s" COMMA "%s" COMMA "%s", rgnm[lhs], rgnm[rhs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if lhs > rhs";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(BLE$)(rid_t lhs, rid_t rhs, cstr sym) : RVInst(0, lhs, rhs) {
  GENSTAT("ble " TAB "%s" COMMA "%s" COMMA "%s", rgnm[lhs], rgnm[rhs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if lhs <= rhs";
  setMAttr(LTY_ONE, opKind::BJ);
}

RVCNAM(FENCE)() : RVInst() {
  GENSTAT("fence");
  comt_ = COMMENT_BEGIN "synchronize";
  setMAttr(LTY_RST, opKind::FENCE);
}

RVCNAM(NOP)() : RVInst() {
  GENSTAT("nop ");
  comt_ = COMMENT_BEGIN "no op";
  setMAttr(LTY_ZERO, opKind::FENCE);
}

RVCNAM(MV)(rid_t rd, rid_t rs) : RVInst(rd, rs) {
  GENSTAT("mv  " TAB "%s" COMMA "%s", rgnm[rd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "move data";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(NOT)(rid_t rd, rid_t rs) : RVInst(rd, rs) {
  GENSTAT("not " TAB "%s" COMMA "%s", rgnm[rd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "logic not";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(NEGW)(rid_t rd, rid_t rs) : RVInst(rd, rs) {
  GENSTAT("negw" TAB "%s" COMMA "%s", rgnm[rd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "negative";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SEXT_W)(rid_t rd, rid_t rs) : RVInst(rd, rs) {
  GENSTAT("sext.w" TAB "%s" COMMA "%s", rgnm[rd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "signed extend";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SEQZ)(rid_t rd, rid_t rs) : RVInst(rd, rs) {
  GENSTAT("seqz" TAB "%s" COMMA "%s", rgnm[rd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "set rd to true if rs == 0";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SNEZ)(rid_t rd, rid_t rs) : RVInst(rd, rs) {
  GENSTAT("snez" TAB "%s" COMMA "%s", rgnm[rd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "set rd to true if rs != 0";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SLTZ)(rid_t rd, rid_t rs) : RVInst(rd, rs) {
  GENSTAT("sltz" TAB "%s" COMMA "%s", rgnm[rd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "set rd to true if rs < 0";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SGTZ)(rid_t rd, rid_t rs) : RVInst(rd, rs) {
  GENSTAT("sgtz" TAB "%s" COMMA "%s", rgnm[rd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "set rd to true if rs > 0";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(FMV_S)
(rid_t frd, rid_t frs) : RVInst(frd, frs) {
  GENSTAT("fmv.s" TAB "%s" COMMA "%s", rgnm[frd], rgnm[frs]);
  comt_ = COMMENT_BEGIN "move float data";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FABS_S)
(rid_t frd, rid_t frs) : RVInst(frd, frs) {
  GENSTAT("fabs.s" TAB "%s" COMMA "%s", rgnm[frd], rgnm[frs]);
  comt_ = COMMENT_BEGIN "get absolute float data";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FNEG_S)
(rid_t frd, rid_t frs) : RVInst(frd, frs) {
  GENSTAT("fneg.s" TAB "%s" COMMA "%s", rgnm[frd], rgnm[frs]);
  comt_ = COMMENT_BEGIN "get negative float data";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(SLTI)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("slti" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "set rd to true if rs > imm";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(XORI)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("xori" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "xor imm";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(ORI)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("ori " TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "or imm";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(ANDI)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("andi" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "and imm";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SLLI)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("slli" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift left logical imm";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SRLI)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("srli" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift right logical imm";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SRAI)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("srai" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift right algorithm imm";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(ADDIW)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("addiw" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "add imm 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SLLIW)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("slliw" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift left logical imm 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SRLIW)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("srliw" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift right logical imm 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SRAIW)(rid_t rd, rid_t rs, i32 imm) : RVInst(rd, rs) {
  GENSTAT("sraiw" TAB "%s" COMMA "%s" COMMA "%d", rgnm[rd], rgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift right algorithm imm 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(ADDW)(rid_t rd, rid_t rs1, rid_t rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("addw" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[rs1],
          rgnm[rs2]);
  comt_ = COMMENT_BEGIN "add 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SUBW)(rid_t rd, rid_t rs1, rid_t rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("subw" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[rs1],
          rgnm[rs2]);
  comt_ = COMMENT_BEGIN "sub 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SLLW)(rid_t rd, rid_t rs1, rid_t rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("sllw" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[rs1],
          rgnm[rs2]);
  comt_ = COMMENT_BEGIN "shift left logical 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SRLW)(rid_t rd, rid_t rs1, rid_t rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("srlw" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[rs1],
          rgnm[rs2]);
  comt_ = COMMENT_BEGIN "shift right logical 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(SRAW)(rid_t rd, rid_t rs1, rid_t rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("sraw" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[rs1],
          rgnm[rs2]);
  comt_ = COMMENT_BEGIN "shift right algorithm 32bit";
  setMAttr(LTY_ZERO, opKind::ALU);
}

RVCNAM(MULW)(rid_t rd, rid_t rs1, rid_t rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("mulw" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[rs1],
          rgnm[rs2]);
  comt_ = COMMENT_BEGIN "mul 32bit";
  setMAttr(LTY_MUL, opKind::MDR);
}

RVCNAM(DIVW)(rid_t rd, rid_t rs1, rid_t rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("divw" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[rs1],
          rgnm[rs2]);
  comt_ = COMMENT_BEGIN "div 32bit";
  setMAttr(LTY_DR, opKind::MDR);
}

RVCNAM(REMW)(rid_t rd, rid_t rs1, rid_t rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("remw" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[rs1],
          rgnm[rs2]);
  comt_ = COMMENT_BEGIN "rem 32bit";
  setMAttr(LTY_DR, opKind::MDR);
}

RVCNAM(FLW)
(rid_t frd, rid_t rb, i32 off) : RVInst(rb, frd) {
  GENSTAT("flw " TAB "%s" COMMA "%d(%s)", rgnm[frd], off, rgnm[rb]);
  comt_ = COMMENT_BEGIN "load word size float data";
  setMAttr(LTY_LOAD, opKind::MEMR);
}

RVCNAM(FSW)
(rid_t frd, rid_t rb, i32 off) : RVInst(rb, frd) {
  GENSTAT("fsw " TAB "%s" COMMA "%d(%s)", rgnm[frd], off, rgnm[rb]);
  comt_ = COMMENT_BEGIN "store word size float data";
  setMAttr(LTY_LOAD, opKind::MEMW);
}

RVCNAM(FMADD_S)
(rid_t frd, rid_t frs1, rid_t frs2, rid_t frs3)
    : RVInst(frd, frs1, frs2, frs3) {
  GENSTAT("fmadd.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", rgnm[frd],
          rgnm[frs1], rgnm[frs2], rgnm[frs3]);
  comt_ = COMMENT_BEGIN "fmadd signle";
  setMAttr(LTY_ONE + LTY_MUL, opKind::FLT);
}

RVCNAM(FMSUB_S)
(rid_t frd, rid_t frs1, rid_t frs2, rid_t frs3)
    : RVInst(frd, frs1, frs2, frs3) {
  GENSTAT("fmsub.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", rgnm[frd],
          rgnm[frs1], rgnm[frs2], rgnm[frs3]);
  comt_ = COMMENT_BEGIN "fmsub signle";
  setMAttr(LTY_ONE + LTY_MUL, opKind::FLT);
}

RVCNAM(FNMADD_S)
(rid_t frd, rid_t frs1, rid_t frs2, rid_t frs3)
    : RVInst(frd, frs1, frs2, frs3) {
  GENSTAT("fnmadd.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", rgnm[frd],
          rgnm[frs1], rgnm[frs2], rgnm[frs3]);
  comt_ = COMMENT_BEGIN "fnmadd signle";
  setMAttr(LTY_ONE + LTY_MUL, opKind::FLT);
}

RVCNAM(FNMSUB_S)
(rid_t frd, rid_t frs1, rid_t frs2, rid_t frs3)
    : RVInst(frd, frs1, frs2, frs3) {
  GENSTAT("fnmsub.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", rgnm[frd],
          rgnm[frs1], rgnm[frs2], rgnm[frs3]);
  comt_ = COMMENT_BEGIN "fnmsub signle";
  setMAttr(LTY_ONE + LTY_MUL, opKind::FLT);
}

RVCNAM(FADD_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fadd.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fadd signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FSUB_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fsub.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fsub signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FMUL_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fmul.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fmul signle";
  setMAttr(LTY_ONE + LTY_MUL, opKind::FLT);
}

RVCNAM(FDIV_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fdiv.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fdiv signle";
  setMAttr(LTY_ONE + LTY_MUL, opKind::FLT);
}

RVCNAM(FSQRT_S)
(rid_t frd, rid_t frs) : RVInst(frd, frs) {
  GENSTAT("fsqrt.s" TAB "%s" COMMA "%s", rgnm[frd], rgnm[frs]);
  comt_ = COMMENT_BEGIN "fsqrt signle";
  setMAttr(LTY_ONE + LTY_DR, opKind::FLT);
}

RVCNAM(FSGNJ_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fsgnj.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fsgnj signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FSGNJN_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fsgnjn.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fsgnjn signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FSGNJX_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fsgnjx.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fsgnjx signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FMIN_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fmin.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fmin signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FMAX_S)
(rid_t frd, rid_t frs1, rid_t frs2) : RVInst(frd, frs1, frs2) {
  GENSTAT("fmax.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[frd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fmax signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FCVT_W_S)
(rid_t rd, rid_t frs) : RVInst(rd, frs) {
  GENSTAT("fcvt.w.s" TAB "%s" COMMA "%s", rgnm[rd], rgnm[frs]);
  comt_ = COMMENT_BEGIN "fcvt s -> w";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FCVT_S_W)
(rid_t frd, rid_t rs) : RVInst(frd, rs) {
  GENSTAT("fcvt.w.s" TAB "%s" COMMA "%s", rgnm[frd], rgnm[rs]);
  comt_ = COMMENT_BEGIN "fcvt w -> s";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FEQ_S)
(rid_t rd, rid_t frs1, rid_t frs2) : RVInst(rd, frs1, frs2) {
  GENSTAT("feq.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "feq signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FLT_S)
(rid_t rd, rid_t frs1, rid_t frs2) : RVInst(rd, frs1, frs2) {
  GENSTAT("fmin.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "flt signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FLE_S)
(rid_t rd, rid_t frs1, rid_t frs2) : RVInst(rd, frs1, frs2) {
  GENSTAT("fmin.s" TAB "%s" COMMA "%s" COMMA "%s", rgnm[rd], rgnm[frs1],
          rgnm[frs2]);
  comt_ = COMMENT_BEGIN "fle signle";
  setMAttr(LTY_ONE, opKind::FLT);
}

RVCNAM(FCLASS_S)
(rid_t rd, rid_t frs) : RVInst(rd, frs) {
  GENSTAT("fmin.s" TAB "%s" COMMA "%s", rgnm[rd], rgnm[frs]);
  comt_ = COMMENT_BEGIN "fclass signle";
  setMAttr(LTY_ONE, opKind::FLT);
}