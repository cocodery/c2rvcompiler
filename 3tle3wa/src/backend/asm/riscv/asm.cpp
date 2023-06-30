#include "3tle3wa/backend/asm/riscv/asm.hh"

#define COMMENT_BEGIN "# "
#define TAB "\t"
#define COMMA ", "
#define NEWLINE "\n"

char nonstr[] = "";

#define RVCNAM(name) rv_##name::rv_##name

#define GENINST(fmt, ...)                         \
    do {                                          \
        size_t len;                               \
        FILE *out = open_memstream(&inst_, &len); \
        fprintf(out, fmt, ##__VA_ARGS__);         \
        fflush(out);                              \
        fclose(out);                              \
    } while (0)

//
// ----- ----- Basic Class ----- -----
//

rv_inst::rv_inst(rid_t rd, rid_t rs, rid_t rt, rid_t ra)
    : rd_(rd), rs_(rs), rt_(rt), ra_(ra), inst_(nonstr), comt_(nonstr) {
    Assert(rd_ < 64, "rd(%02lu) exceed range i(0, 31), f(32, 63)", rd);
    Assert(rs_ < 64, "rs(%02lu) exceed range i(0, 31), f(32, 63)", rs_);
    Assert(rt_ < 64, "rt(%02lu) exceed range i(0, 31), f(32, 63)", rt_);
    Assert(ra_ < 64, "ra(%02lu) exceed range i(0, 31), f(32, 63)", ra_);
}

rv_inst::~rv_inst() {
    if (inst_) {
        delete inst_;
        inst_ = nullptr;
    }
}

std::string_view rv_inst::to_string() { return inst_; }

std::string_view rv_inst::comment() { return comt_; }

void rv_inst::set_comment(cstr comt) { comt_ = comt; }

RVCNAM(fake_lb)(cstr sym) : rv_inst() {
    GENINST("%s: ", sym);
    comt_ = COMMENT_BEGIN "fake label";
}

RVCNAM(fake_br)(rid_t rs, cstr sym) : rv_inst(riscv::zero, rs) { GENINST("beqz" TAB "%s" COMMA "%s", gpr[rs], sym); }

//
// asm with label
//

RVCNAM(la_l)(rid_t rd, cstr sym) : rv_inst(rd) {
    GENINST("la" TAB "%s" COMMA "%s", gpr[rd], sym);
    comt_ = COMMENT_BEGIN "rd <= sym";
}

RVCNAM(lla_l)(rid_t rd, cstr sym) : rv_inst(rd) {
    GENINST("lla" TAB "%s" COMMA "%s", gpr[rd], sym);
    comt_ = COMMENT_BEGIN "rd <= sym";
    type_ = NORM;
}

RVCNAM(lb_l)(rid_t rd, cstr sym) : rv_inst(rd) {
    GENINST("lb" TAB "%s" COMMA "%s", gpr[rd], sym);
    comt_ = COMMENT_BEGIN "rd <= byte ptr [sym]";
}

RVCNAM(lh_l)(rid_t rd, cstr sym) : rv_inst(rd) {
    GENINST("lh" TAB "%s" COMMA "%s", gpr[rd], sym);
    comt_ = COMMENT_BEGIN "rd <= word ptr [sym]";
}

RVCNAM(lw_l)(rid_t rd, cstr sym) : rv_inst(rd) {
    GENINST("lw" TAB "%s" COMMA "%s", gpr[rd], sym);
    comt_ = COMMENT_BEGIN "rd <= dword ptr [sym]";
    type_ = ACCMEM;
}

RVCNAM(ld_l)(rid_t rd, cstr sym) : rv_inst(rd) {
    GENINST("ld" TAB "%s" COMMA "%s", gpr[rd], sym);
    comt_ = COMMENT_BEGIN "rd <= qword ptr [sym]";
    type_ = ACCMEM;
}

RVCNAM(sb_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(riscv::zero, rd, rt) {
    GENINST("sb" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
    comt_ = COMMENT_BEGIN "byte ptr [sym] <= rd";
}

RVCNAM(sh_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(riscv::zero, rd, rt) {
    GENINST("sh" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
    comt_ = COMMENT_BEGIN "word ptr [sym] <= rd";
}

RVCNAM(sw_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(riscv::zero, rd, rt) {
    GENINST("sw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
    comt_ = COMMENT_BEGIN "dword ptr [sym] <= rd";
    type_ = ACCMEM;
}

RVCNAM(sd_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(riscv::zero, rd, rt) {
    GENINST("sd" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
    comt_ = COMMENT_BEGIN "qword ptr [sym] <= rd";
    type_ = ACCMEM;
}

RVCNAM(flw_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(rd, riscv::zero, rt) {
    GENINST("flw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
    type_ = ACCMEM;
}

RVCNAM(fld_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(rd, riscv::zero, rt) {
    GENINST("fld" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
}

RVCNAM(fsw_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(riscv::zero, rd, rt) {
    GENINST("fsw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
    type_ = ACCMEM;
}

RVCNAM(fsd_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(riscv::zero, rd, rt) {
    GENINST("fsd" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
}

//
// misc int op
//

RVCNAM(nop)() : rv_inst() { GENINST("nop"); }

RVCNAM(li)(rid_t rd, i64 imm) : rv_inst(rd) {
    GENINST("li" TAB "%s" COMMA "%" PRId64, gpr[rd], imm);
    imm_ = imm;
    type_ = NORM;
}

RVCNAM(mv)(rid_t rd, rid_t rs) : rv_inst(rd, rs) {
    GENINST("mv" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]);
    type_ = NORM;
}

RVCNAM(not )(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("not" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(neg)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("neg" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(negw)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("negw" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(sext_w)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("sext.w" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

//
// set flag
//

RVCNAM(seqz)(rid_t rd, rid_t rs) : rv_inst(rd, rs) {
    GENINST("seqz" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]);
    type_ = NORM;
}

RVCNAM(snez)(rid_t rd, rid_t rs) : rv_inst(rd, rs) {
    GENINST("snez" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]);
    type_ = NORM;
}

RVCNAM(sltz)(rid_t rd, rid_t rs) : rv_inst(rd, rs) {
    GENINST("sltz" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]);
    type_ = NORM;
}

RVCNAM(sgtz)(rid_t rd, rid_t rs) : rv_inst(rd, rs) {
    GENINST("sgtz" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]);
    type_ = NORM;
}

RVCNAM(slt)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("slt" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(slti)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("slti" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(sltiu)(rid_t rd, rid_t rs, u64 imm) : rv_inst(rd, rs) {
    GENINST("sltiu" TAB "%s" COMMA "%s" COMMA "%" PRIu64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(sgt)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("slt" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rt], gpr[rs]);
    type_ = NORM;
}

RVCNAM(sltu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sltu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(sgtu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sltu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rt], gpr[rs]);
    type_ = NORM;
}

//
// misc flt op
//

RVCNAM(fmv_s)(rid_t frd, rid_t frs) : rv_inst(frd, frs) {
    GENINST("fmv.s" TAB "%s" COMMA "%s", gpr[frd], gpr[frs]);
    type_ = FLTOP;
}

RVCNAM(fabs_s)(rid_t frd, rid_t frs) : rv_inst(frd, frs) { GENINST("fabs.s" TAB "%s" COMMA "%s", gpr[frd], gpr[frs]); }

RVCNAM(fneg_s)(rid_t frd, rid_t frs) : rv_inst(frd, frs) { GENINST("fneg.s" TAB "%s" COMMA "%s", gpr[frd], gpr[frs]); }

//
// branch with zero
//

RVCNAM(beqz)(rid_t rs, cstr off) : rv_inst(rs) {
    GENINST("beqz" TAB "%s" COMMA "%s", gpr[rs], off);
    type_ = BRJMP;
}

RVCNAM(bnez)(rid_t rs, cstr off) : rv_inst(rs) {
    GENINST("bnez" TAB "%s" COMMA "%s", gpr[rs], off);
    type_ = BRJMP;
}

RVCNAM(blez)(rid_t rs, cstr off) : rv_inst(rs) {
    GENINST("blez" TAB "%s" COMMA "%s", gpr[rs], off);
    type_ = BRJMP;
}

RVCNAM(bgez)(rid_t rs, cstr off) : rv_inst(rs) {
    GENINST("bgez" TAB "%s" COMMA "%s", gpr[rs], off);
    type_ = BRJMP;
}

RVCNAM(bltz)(rid_t rs, cstr off) : rv_inst(rs) {
    GENINST("bltz" TAB "%s" COMMA "%s", gpr[rs], off);
    type_ = BRJMP;
}

RVCNAM(bgtz)(rid_t rs, cstr off) : rv_inst(rs) {
    GENINST("bgtz" TAB "%s" COMMA "%s", gpr[rs], off);
    type_ = BRJMP;
}

//
// branch 2 op
//

RVCNAM(beq)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("beq" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(bne)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bne" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(blt)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("blt" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(bge)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bge" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(bgt)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bgt" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(ble)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("ble" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(bltu)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bltu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(bgeu)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bgeu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(bgtu)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bgtu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

RVCNAM(bleu)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bleu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
    type_ = BRJMP;
}

//
// j misc
//

RVCNAM(j)(cstr off) : rv_inst() {
    GENINST("j" TAB "%s", off);
    type_ = BRJMP;
}

RVCNAM(jal)(cstr off) : rv_inst(riscv::ra) {
    GENINST("jal" TAB "%s", off);
    type_ = BRJMP;
}

RVCNAM(jr)(rid_t rs) : rv_inst(riscv::zero, rs) {
    GENINST("jr" TAB "%s", gpr[rs]);
    type_ = BRJMP;
}

RVCNAM(jalr)(rid_t rs) : rv_inst(riscv::ra, rs) {
    GENINST("jalr" TAB "%s", gpr[rs]);
    type_ = BRJMP;
}

RVCNAM(ret)() : rv_inst() {
    GENINST("ret");
    type_ = BRJMP;
}

RVCNAM(call)(cstr off) : rv_inst(riscv::ra) {
    GENINST("call" TAB "%s", off);
    type_ = BRJMP;
}

RVCNAM(tail)(cstr off) : rv_inst(riscv::t1) {
    GENINST("tail" TAB "%s", off);
    type_ = BRJMP;
}

//
// RV64I
//

RVCNAM(lb)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lb" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(lh)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lh" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(lw)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lw" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(ld)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("ld" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(lbu)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lbu" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(lhu)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lhu" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(lwu)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lwu" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(sb)(rid_t rs, rid_t rt, i64 off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("sb" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rs], off, gpr[rt]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(sh)(rid_t rs, rid_t rt, i64 off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("sh" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rs], off, gpr[rt]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(sw)(rid_t rs, rid_t rt, i64 off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("sw" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rs], off, gpr[rt]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(sd)(rid_t rs, rid_t rt, i64 off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("sd" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rs], off, gpr[rt]);
    off_ = off;
    type_ = ACCMEM;
}

RVCNAM(addi)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("addi" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(xori)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("xori" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(ori)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("ori" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(andi)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("andi" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(slli)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("slli" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(srli)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("srli" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(srai)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("srai" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(add)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("add" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(sub)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sub" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(sll)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sll" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM (xor)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("xor" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(srl)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("srl" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(sra)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sra" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(or)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("or" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM (and)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("and" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(addiw)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("addiw" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(slliw)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("slliw" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(srliw)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("srliw" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(sraiw)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("sraiw" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
    type_ = NORM;
}

RVCNAM(addw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("addw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(subw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("subw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(sllw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sllw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(srlw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("srlw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

RVCNAM(sraw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sraw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = NORM;
}

//
// RV64M
//

RVCNAM(mul)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mul" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(mulh)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mulh" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(mulhsu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mulhsu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(mulhu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mulhu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(div)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("div" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(divu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("divu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(rem)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("rem" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(remu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("remu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(mulw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mulw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(divw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("divw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(divuw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("divuw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(remw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("remw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

RVCNAM(remuw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("remuw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
    type_ = INTMDR;
}

//
// RV64F
//

RVCNAM(flw)(rid_t frd, rid_t rs, i64 off) : rv_inst(frd, rs) {
    GENINST("flw" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[frd], off, gpr[rs]);
    type_ = ACCMEM;
}

RVCNAM(fsw)(rid_t frs, rid_t rt, i64 off) : rv_inst(riscv::zero, frs, rt) {
    GENINST("fsw" TAB TAB "%s" COMMA "%" PRId64 "(%s)", gpr[frs], off, gpr[rt]);
    type_ = ACCMEM;
}

RVCNAM(fmadd_s)(rid_t frd, rid_t frs, rid_t frt, rid_t fra) : rv_inst(frd, frs, frt, fra) {
    GENINST("fmadd.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt], gpr[fra]);
    type_ = FLTOP;
}

RVCNAM(fmsub_s)(rid_t frd, rid_t frs, rid_t frt, rid_t fra) : rv_inst(frd, frs, frt, fra) {
    GENINST("fmsub.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt], gpr[fra]);
    type_ = FLTOP;
}

RVCNAM(fnmadd_s)(rid_t frd, rid_t frs, rid_t frt, rid_t fra) : rv_inst(frd, frs, frt, fra) {
    GENINST("fnmadd.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt], gpr[fra]);
    type_ = FLTOP;
}

RVCNAM(fnmsub_s)(rid_t frd, rid_t frs, rid_t frt, rid_t fra) : rv_inst(frd, frs, frt, fra) {
    GENINST("fnmsub.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt], gpr[fra]);
    type_ = FLTOP;
}

RVCNAM(fadd_s)(rid_t frd, rid_t frs, rid_t frt) : rv_inst(frd, frs, frt) {
    GENINST("fadd.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt]);
    type_ = FLTOP;
}

RVCNAM(fsub_s)(rid_t frd, rid_t frs, rid_t frt) : rv_inst(frd, frs, frt) {
    GENINST("fsub.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt]);
    type_ = FLTOP;
}

RVCNAM(fmul_s)(rid_t frd, rid_t frs, rid_t frt) : rv_inst(frd, frs, frt) {
    GENINST("fmul.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt]);
    type_ = FLTOP;
}

RVCNAM(fdiv_s)(rid_t frd, rid_t frs, rid_t frt) : rv_inst(frd, frs, frt) {
    GENINST("fdiv.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt]);
    type_ = FLTOP;
}

// 目前固定舍入方式为 rtz

RVCNAM(fcvt_w_s)(rid_t rd, rid_t frs) : rv_inst(rd, frs) {
    GENINST("fcvt.w.s" TAB "%s" COMMA "%s" COMMA "rtz", gpr[rd], gpr[frs]);
    type_ = FLTOP;
}

RVCNAM(fcvt_s_w)(rid_t frd, rid_t rs) : rv_inst(frd, rs) {
    GENINST("fcvt.s.w" TAB "%s" COMMA "%s" COMMA "rtz", gpr[frd], gpr[rs]);
    type_ = FLTOP;
}

RVCNAM(feq)(rid_t rd, rid_t frs, rid_t frt) : rv_inst(rd, frs, frt) {
    GENINST("feq.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[frs], gpr[frt]);
    type_ = FLTOP;
}

RVCNAM(flt)(rid_t rd, rid_t frs, rid_t frt) : rv_inst(rd, frs, frt) {
    GENINST("flt.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[frs], gpr[frt]);
    type_ = FLTOP;
}

RVCNAM(fle)(rid_t rd, rid_t frs, rid_t frt) : rv_inst(rd, frs, frt) {
    GENINST("fle.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[frs], gpr[frt]);
    type_ = FLTOP;
}

RVCNAM(fmv_w_x)(rid_t frd, rid_t rs) : rv_inst(frd, rs) {
    GENINST("fmv.w.x" TAB "%s" COMMA "%s", gpr[frd], gpr[rs]);
    type_ = FLTOP;
}

RVCNAM(fmv_x_w)(rid_t rd, rid_t frs) : rv_inst(rd, frs) {
    GENINST("fmv.x.w" TAB "%s" COMMA "%s", gpr[rd], gpr[frs]);
    type_ = FLTOP;
}
