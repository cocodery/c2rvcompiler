#include <cassert>

#include "riscv-asm.hh"

#define COMMENT_BEGIN "\t# "
#define TAB "\t"
#define COMMA ","
#define NEWLINE "\n"

#define RVCNAM(name) RV_##name::RV_##name

#define GENSTAT(fmt, ...)                                                      \
  do {                                                                         \
    FILE *out = open_memstream(&stat, &statlen);                               \
    fprintf(out, TAB fmt, ##__VA_ARGS__);                                      \
    fflush(out);                                                               \
    fclose(out);                                                               \
  } while (0)

using rifs = REGinterface;

char nonstr[] = "";

static const char *irgnm[] = {
    "zero",        "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
    "fp" /* s0 */, "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
    "a6",          "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
    "s8",          "s9", "s10", "s11", "t3", "t4", "t5", "t6",
};

static const char *frgnm[] = {
    "ft0", "ft1", "ft2",  "ft3",  "ft4", "ft5", "ft6",  "ft7",
    "fs0", "fs1", "fa0",  "fa1",  "fa2", "fa3", "fa4",  "fa5",
    "fa6", "fa7", "fs2",  "fs3",  "fs4", "fs5", "fs6",  "fs7",
    "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11",
};

//
// ----- ----- Basic Class ----- -----
//

RVInst::RVInst(u8 rd, u8 rs1, u8 rs2)
    : dst(rd), src1(rs1), src2(rs2), stat(nonstr), statlen(0),
      comt_(COMMENT_BEGIN "error inst") {
  assert(rd < 32);
  assert(src1 < 32);
  assert(src2 < 32);
}

RVInst::RVInst(rifs &&rfc)
    : dst(rfc.dst), src1(rfc.src1), src2(rfc.src2), fdst(rfc.fdst),
      fsrc1(rfc.fsrc1), fsrc2(rfc.fsrc2), fsrc3(rfc.fsrc3), stat(nonstr),
      statlen(0), comt_(COMMENT_BEGIN "error inst") {
  assert(dst < 32);
  assert(src1 < 32);
  assert(src2 < 32);
  assert(fdst < 33);
  assert(fsrc1 < 33);
  assert(fsrc2 < 33);
  assert(fsrc3 < 33);
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

//
// ----- ----- Instructions ----- -----
//

RVCNAM(LI)(u8 rd, i32 imm) : RVInst(rd, 0, 0) {
  GENSTAT("li" TAB "%s" COMMA "%d", irgnm[rd], imm);
  comt_ = COMMENT_BEGIN "load immediate";
}

RVCNAM(LA$)(u8 rd, cstr sym) : RVInst(rd, 0, 0) {
  GENSTAT("la" TAB "%s" COMMA "%s", irgnm[rd], sym);
  comt_ = COMMENT_BEGIN "load absolute address";
}

RVCNAM(LLA$)(u8 rd, cstr sym) : RVInst(rd, 0, 0) {
  GENSTAT("lla" TAB "%s" COMMA "%s", irgnm[rd], sym);
  comt_ = COMMENT_BEGIN "load local address";
}

RVCNAM(LEA$)(u8 rd, cstr sym) : RVInst(rd, 0, 0) {
  FILE *out = open_memstream(&stat, &statlen);
  fprintf(out, TAB "lui" TAB "%s" COMMA "%%hi(%s)" NEWLINE, irgnm[rd], sym);
  fprintf(out, TAB "addi" TAB "%s" COMMA "%s" COMMA "%%lo(%s)", irgnm[rd],
          irgnm[rd], sym);
  fflush(out);
  fclose(out);

  comt_ = COMMENT_BEGIN "load effective address";
}

RVCNAM(LW)(u8 rd, u8 rb, i32 off) : RVInst(rd, rb, 0) {
  GENSTAT("lw" TAB "%s" COMMA "%d(%s)", irgnm[rd], off, irgnm[rb]);
  comt_ = COMMENT_BEGIN "load word signed";
}

RVCNAM(LD)(u8 rd, u8 rb, i32 off) : RVInst(rd, rb, 0) {
  GENSTAT("ld" TAB "%s" COMMA "%d(%s)", irgnm[rd], off, irgnm[rb]);
  comt_ = COMMENT_BEGIN "load double word signed";
}

RVCNAM(LWU)(u8 rd, u8 rb, i32 off) : RVInst(rd, rb, 0) {
  GENSTAT("lwu" TAB "%s" COMMA "%d(%s)", irgnm[rd], off, irgnm[rb]);
  comt_ = COMMENT_BEGIN "load word unsigned";
}

RVCNAM(SW)(u8 rs, u8 rb, i32 off) : RVInst(0, rs, rb) {
  GENSTAT("sw" TAB "%s" COMMA "%d(%s)", irgnm[rs], off, irgnm[rb]);
  comt_ = COMMENT_BEGIN "store word";
}

RVCNAM(SD)(u8 rs, u8 rb, i32 off) : RVInst(0, rs, rb) {
  GENSTAT("sd" TAB "%s" COMMA "%d(%s)", irgnm[rs], off, irgnm[rb]);
  comt_ = COMMENT_BEGIN "store double word";
}

RVCNAM(LW$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("lw" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "load word by symbol";
}

RVCNAM(LD$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("ld" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "load double word by symbol";
}

RVCNAM(LWU$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("lwu" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "load word unsigned by symbol";
}

RVCNAM(SW$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("sw" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "store word by symbol";
}

RVCNAM(SD$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("sd" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "store double word by symbol";
}

RVCNAM(JR)(u8 rs) : RVInst(0, rs, 0) {
  GENSTAT("jr" TAB "%s", irgnm[rs]);
  comt_ = COMMENT_BEGIN "jmp by addr in reg";
}

RVCNAM(JALR)(u8 rs) : RVInst(0, rs, 0) {
  GENSTAT("jalr" TAB "%s", irgnm[rs]);
  comt_ = COMMENT_BEGIN "jmp and link by addr in reg";
}

RVCNAM(J$)(cstr sym) : RVInst(0, 0, 0) {
  GENSTAT("j" TAB "%s", sym);
  comt_ = COMMENT_BEGIN "jmp by symbol";
}

RVCNAM(JAL$)(cstr sym) : RVInst(0, 0, 0) {
  GENSTAT("jal" TAB "%s", sym);
  comt_ = COMMENT_BEGIN "jmp and link by symbol";
}

RVCNAM(RET)() : RVInst(0, 0, 0) {
  GENSTAT("ret");
  comt_ = COMMENT_BEGIN "return";
}

RVCNAM(CALL$)(cstr sym) : RVInst(0, 0, 0) {
  GENSTAT("call" TAB "%s", sym);
  comt_ = COMMENT_BEGIN "call function";
}

RVCNAM(TAIL$)(cstr sym) : RVInst(0, 0, 0) {
  GENSTAT("tail" TAB "%s", sym);
  comt_ = COMMENT_BEGIN "tail call function";
}

RVCNAM(BEQZ$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("beqz" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs == 0";
}

RVCNAM(BNEZ$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("bnez" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs != 0";
}

RVCNAM(BLEZ$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("blez" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs <= 0";
}

RVCNAM(BGEZ$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("bgez" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs >= 0";
}

RVCNAM(BLTZ$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("bltz" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs < 0";
}

RVCNAM(BGTZ$)(u8 rs, cstr sym) : RVInst(0, rs, 0) {
  GENSTAT("bgtz" TAB "%s" COMMA "%s", irgnm[rs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if rs > 0";
}

RVCNAM(BGT$)(u8 lhs, u8 rhs, cstr sym) : RVInst(0, lhs, rhs) {
  GENSTAT("bgt" TAB "%s" COMMA "%s" COMMA "%s", irgnm[lhs], irgnm[rhs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if lhs > rhs";
}

RVCNAM(BLE$)(u8 lhs, u8 rhs, cstr sym) : RVInst(0, lhs, rhs) {
  GENSTAT("ble" TAB "%s" COMMA "%s" COMMA "%s", irgnm[lhs], irgnm[rhs], sym);
  comt_ = COMMENT_BEGIN "jmp to symbol if lhs <= rhs";
}

RVCNAM(FENCE)() : RVInst(0, 0, 0) {
  GENSTAT("fence");
  comt_ = COMMENT_BEGIN "synchronize";
}

RVCNAM(NOP)() : RVInst(0, 0, 0) {
  GENSTAT("nop");
  comt_ = COMMENT_BEGIN "no op";
}

RVCNAM(MV)(u8 rd, u8 rs) : RVInst(rd, rs, 0) {
  GENSTAT("mv" TAB "%s" COMMA "%s", irgnm[rd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "move data";
}

RVCNAM(NOT)(u8 rd, u8 rs) : RVInst(rd, rs, 0) {
  GENSTAT("not" TAB "%s" COMMA "%s", irgnm[rd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "logic not";
}

RVCNAM(NEGW)(u8 rd, u8 rs) : RVInst(rd, rs, 0) {
  GENSTAT("negw" TAB "%s" COMMA "%s", irgnm[rd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "negative";
}

RVCNAM(SEXT_W)(u8 rd, u8 rs) : RVInst(rd, rs, 0) {
  GENSTAT("sext.w" TAB "%s" COMMA "%s", irgnm[rd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "signed extend";
}

RVCNAM(SEQZ)(u8 rd, u8 rs) : RVInst(rd, rs, 0) {
  GENSTAT("seqz" TAB "%s" COMMA "%s", irgnm[rd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "set rd to true if rs == 0";
}

RVCNAM(SNEZ)(u8 rd, u8 rs) : RVInst(rd, rs, 0) {
  GENSTAT("snez" TAB "%s" COMMA "%s", irgnm[rd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "set rd to true if rs != 0";
}

RVCNAM(SLTZ)(u8 rd, u8 rs) : RVInst(rd, rs, 0) {
  GENSTAT("sltz" TAB "%s" COMMA "%s", irgnm[rd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "set rd to true if rs < 0";
}

RVCNAM(SGTZ)(u8 rd, u8 rs) : RVInst(rd, rs, 0) {
  GENSTAT("sgtz" TAB "%s" COMMA "%s", irgnm[rd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "set rd to true if rs > 0";
}

RVCNAM(FMV_S)(u8 frd, u8 frs) : RVInst(rifs{.fdst = frd, .fsrc1 = frs}) {
  GENSTAT("fmv.s" TAB "%s" COMMA "%s", frgnm[frd], frgnm[frs]);
  comt_ = COMMENT_BEGIN "move float data";
}

RVCNAM(FABS_S)
(u8 frd, u8 frs) : RVInst(rifs{.fdst = frd, .fsrc1 = frs}) {
  GENSTAT("fabs.s" TAB "%s" COMMA "%s", frgnm[frd], frgnm[frs]);
  comt_ = COMMENT_BEGIN "get absolute float data";
}

RVCNAM(FNEG_S)
(u8 frd, u8 frs) : RVInst(rifs{.fdst = frd, .fsrc1 = frs}) {
  GENSTAT("fneg.s" TAB "%s" COMMA "%s", frgnm[frd], frgnm[frs]);
  comt_ = COMMENT_BEGIN "get negative float data";
}

RVCNAM(SLTI)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("slti" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "set rd to true if rs > imm";
}

RVCNAM(XORI)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("xori" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "xor imm";
}

RVCNAM(ORI)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("ori" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "or imm";
}

RVCNAM(ANDI)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("andi" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "and imm";
}

RVCNAM(SLLI)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("slli" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift left logical imm";
}

RVCNAM(SRLI)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("srli" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift right logical imm";
}

RVCNAM(SRAI)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("srai" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift right algorithm imm";
}

RVCNAM(ADDIW)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("addiw" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "add imm 32bit";
}

RVCNAM(SLLIW)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("slliw" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift left logical imm 32bit";
}

RVCNAM(SRLIW)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("srliw" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift right logical imm 32bit";
}

RVCNAM(SRAIW)(u8 rd, u8 rs, i32 imm) : RVInst(rd, rs, 0) {
  GENSTAT("sraiw" TAB "%s" COMMA "%s" COMMA "%d", irgnm[rd], irgnm[rs], imm);
  comt_ = COMMENT_BEGIN "shift right algorithm imm 32bit";
}

RVCNAM(ADDW)(u8 rd, u8 rs1, u8 rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("addw" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], irgnm[rs1],
          irgnm[rs2]);
  comt_ = COMMENT_BEGIN "add 32bit";
}

RVCNAM(SUBW)(u8 rd, u8 rs1, u8 rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("subw" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], irgnm[rs1],
          irgnm[rs2]);
  comt_ = COMMENT_BEGIN "sub 32bit";
}

RVCNAM(SLLW)(u8 rd, u8 rs1, u8 rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("sllw" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], irgnm[rs1],
          irgnm[rs2]);
  comt_ = COMMENT_BEGIN "shift left logical 32bit";
}

RVCNAM(SRLW)(u8 rd, u8 rs1, u8 rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("srlw" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], irgnm[rs1],
          irgnm[rs2]);
  comt_ = COMMENT_BEGIN "shift right logical 32bit";
}

RVCNAM(SRAW)(u8 rd, u8 rs1, u8 rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("sraw" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], irgnm[rs1],
          irgnm[rs2]);
  comt_ = COMMENT_BEGIN "shift right algorithm 32bit";
}

RVCNAM(MULW)(u8 rd, u8 rs1, u8 rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("mulw" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], irgnm[rs1],
          irgnm[rs2]);
  comt_ = COMMENT_BEGIN "mul 32bit";
}

RVCNAM(DIVW)(u8 rd, u8 rs1, u8 rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("divw" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], irgnm[rs1],
          irgnm[rs2]);
  comt_ = COMMENT_BEGIN "div 32bit";
}

RVCNAM(REMW)(u8 rd, u8 rs1, u8 rs2) : RVInst(rd, rs1, rs2) {
  GENSTAT("remw" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], irgnm[rs1],
          irgnm[rs2]);
  comt_ = COMMENT_BEGIN "rem 32bit";
}

RVCNAM(FLW)(u8 frd, u8 rb, i32 off) : RVInst(rifs{.src1 = rb, .fdst = frd}) {
  GENSTAT("flw" TAB "%s" COMMA "%d(%s)", frgnm[frd], off, irgnm[rb]);
  comt_ = COMMENT_BEGIN "load word size float data";
}

RVCNAM(FMADD_S)
(u8 frd, u8 frs1, u8 frs2, u8 frs3)
    : RVInst(
          rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2, .fsrc3 = fsrc3}) {
  GENSTAT("fmadd.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", frgnm[frd],
          frgnm[fsrc1], frgnm[fsrc2], frgnm[fsrc3]);
  comt_ = COMMENT_BEGIN "fmadd signle";
}

RVCNAM(FMSUB_S)
(u8 frd, u8 frs1, u8 frs2, u8 frs3)
    : RVInst(
          rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2, .fsrc3 = fsrc3}) {
  GENSTAT("fmsub.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", frgnm[frd],
          frgnm[fsrc1], frgnm[fsrc2], frgnm[fsrc3]);
  comt_ = COMMENT_BEGIN "fmsub signle";
}

RVCNAM(FNMADD_S)
(u8 frd, u8 frs1, u8 frs2, u8 frs3)
    : RVInst(
          rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2, .fsrc3 = fsrc3}) {
  GENSTAT("fnmadd.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", frgnm[frd],
          frgnm[fsrc1], frgnm[fsrc2], frgnm[fsrc3]);
  comt_ = COMMENT_BEGIN "fnmadd signle";
}

RVCNAM(FNMSUB_S)
(u8 frd, u8 frs1, u8 frs2, u8 frs3)
    : RVInst(
          rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2, .fsrc3 = fsrc3}) {
  GENSTAT("fnmsub.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", frgnm[frd],
          frgnm[fsrc1], frgnm[fsrc2], frgnm[fsrc3]);
  comt_ = COMMENT_BEGIN "fnmsub signle";
}

RVCNAM(FADD_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fadd.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fadd signle";
}

RVCNAM(FSUB_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fsub.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fsub signle";
}

RVCNAM(FMUL_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fmul.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fmul signle";
}

RVCNAM(FDIV_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fdiv.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fdiv signle";
}

RVCNAM(FSQRT_S)(u8 frd, u8 frs1) : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1}) {
  GENSTAT("fsqrt.s" TAB "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1]);
  comt_ = COMMENT_BEGIN "fsqrt signle";
}

RVCNAM(FSGNJ_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fsgnj.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fsgnj signle";
}

RVCNAM(FSGNJN_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fsgnjn.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fsgnjn signle";
}

RVCNAM(FSGNJX_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fsgnjx.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fsgnjx signle";
}

RVCNAM(FMIN_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fmin.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fmin signle";
}

RVCNAM(FMAX_S)
(u8 frd, u8 frs1, u8 frs2)
    : RVInst(rifs{.fdst = frd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fmax.s" TAB "%s" COMMA "%s" COMMA "%s", frgnm[frd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fmax signle";
}

RVCNAM(FCVT_W_S)(u8 rd, u8 frs) : RVInst(rifs{.dst = rd, .fsrc1 = fsrc1}) {
  GENSTAT("fcvt.w.s" TAB "%s" COMMA "%s", irgnm[rd], frgnm[fsrc1]);
  comt_ = COMMENT_BEGIN "fcvt s -> w";
}

RVCNAM(FCVT_S_W)(u8 frd, u8 rs) : RVInst(rifs{.src1 = rs, .fdst = frd}) {
  GENSTAT("fcvt.w.s" TAB "%s" COMMA "%s", frgnm[frd], irgnm[rs]);
  comt_ = COMMENT_BEGIN "fcvt w -> s";
}

RVCNAM(FEQ_S)
(u8 rd, u8 frs1, u8 frs2)
    : RVInst(rifs{.dst = rd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("feq.s" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "feq signle";
}

RVCNAM(FLT_S)
(u8 rd, u8 frs1, u8 frs2)
    : RVInst(rifs{.dst = rd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fmin.s" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "flt signle";
}

RVCNAM(FLE_S)
(u8 rd, u8 frs1, u8 frs2)
    : RVInst(rifs{.dst = rd, .fsrc1 = fsrc1, .fsrc2 = fsrc2}) {
  GENSTAT("fmin.s" TAB "%s" COMMA "%s" COMMA "%s", irgnm[rd], frgnm[fsrc1],
          frgnm[fsrc2]);
  comt_ = COMMENT_BEGIN "fle signle";
}

RVCNAM(FCLASS_S)
(u8 rd, u8 frs) : RVInst(rifs{.dst = rd, .fsrc1 = frs}) {
  GENSTAT("fmin.s" TAB "%s" COMMA "%s", irgnm[rd], frgnm[fsrc1]);
  comt_ = COMMENT_BEGIN "fclass signle";
}