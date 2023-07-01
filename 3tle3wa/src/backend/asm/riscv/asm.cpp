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
}

RVCNAM(ld_l)(rid_t rd, cstr sym) : rv_inst(rd) {
    GENINST("ld" TAB "%s" COMMA "%s", gpr[rd], sym);
    comt_ = COMMENT_BEGIN "rd <= qword ptr [sym]";
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
}

RVCNAM(sd_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(riscv::zero, rd, rt) {
    GENINST("sd" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
    comt_ = COMMENT_BEGIN "qword ptr [sym] <= rd";
}

RVCNAM(flw_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(rd, riscv::zero, rt) {
    GENINST("flw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
}

RVCNAM(fld_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(rd, riscv::zero, rt) {
    GENINST("fld" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
}

RVCNAM(fsw_l)(rid_t rd, cstr sym, rid_t rt) : rv_inst(riscv::zero, rd, rt) {
    GENINST("fsw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], sym, gpr[rt]);
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
}

RVCNAM(mv)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("mv" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(not )(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("not" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(neg)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("neg" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(negw)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("negw" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(sext_w)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("sext.w" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

//
// set flag
//

RVCNAM(seqz)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("seqz" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(snez)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("snez" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(sltz)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("sltz" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(sgtz)(rid_t rd, rid_t rs) : rv_inst(rd, rs) { GENINST("sgtz" TAB "%s" COMMA "%s", gpr[rd], gpr[rs]); }

RVCNAM(slt)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("slt" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(slti)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("slti" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(sltiu)(rid_t rd, rid_t rs, u64 imm) : rv_inst(rd, rs) {
    GENINST("sltiu" TAB "%s" COMMA "%s" COMMA "%" PRIu64, gpr[rd], gpr[rs], imm);
}

RVCNAM(sgt)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("slt" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rt], gpr[rs]);
}

RVCNAM(sltu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sltu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(sgtu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sltu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rt], gpr[rs]);
}

//
// misc flt op
//

RVCNAM(fmv_s)(rid_t frd, rid_t frs) : rv_inst(frd, frs) { GENINST("fmv.s" TAB "%s" COMMA "%s", gpr[frd], gpr[frs]); }

RVCNAM(fabs_s)(rid_t frd, rid_t frs) : rv_inst(frd, frs) { GENINST("fabs.s" TAB "%s" COMMA "%s", gpr[frd], gpr[frs]); }

RVCNAM(fneg_s)(rid_t frd, rid_t frs) : rv_inst(frd, frs) { GENINST("fneg.s" TAB "%s" COMMA "%s", gpr[frd], gpr[frs]); }

//
// branch with zero
//

RVCNAM(beqz)(rid_t rs, cstr off) : rv_inst(rs) { GENINST("beqz" TAB "%s" COMMA "%s", gpr[rs], off); }

RVCNAM(bnez)(rid_t rs, cstr off) : rv_inst(rs) { GENINST("bnez" TAB "%s" COMMA "%s", gpr[rs], off); }

RVCNAM(blez)(rid_t rs, cstr off) : rv_inst(rs) { GENINST("blez" TAB "%s" COMMA "%s", gpr[rs], off); }

RVCNAM(bgez)(rid_t rs, cstr off) : rv_inst(rs) { GENINST("bgez" TAB "%s" COMMA "%s", gpr[rs], off); }

RVCNAM(bltz)(rid_t rs, cstr off) : rv_inst(rs) { GENINST("bltz" TAB "%s" COMMA "%s", gpr[rs], off); }

RVCNAM(bgtz)(rid_t rs, cstr off) : rv_inst(rs) { GENINST("bgtz" TAB "%s" COMMA "%s", gpr[rs], off); }

//
// branch 2 op
//

RVCNAM(beq)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("beq" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(bne)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bne" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(blt)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("blt" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(bge)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bge" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(bgt)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bgt" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(ble)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("ble" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(bltu)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bltu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(bgeu)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bgeu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(bgtu)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bgtu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

RVCNAM(bleu)(rid_t rs, rid_t rt, cstr off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("bleu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rs], gpr[rt], off);
}

//
// j misc
//

RVCNAM(j)(cstr off) : rv_inst() { GENINST("j" TAB "%s", off); }

RVCNAM(jal)(cstr off) : rv_inst(riscv::ra) { GENINST("jal" TAB "%s", off); }

RVCNAM(jr)(rid_t rs) : rv_inst(riscv::zero, rs) { GENINST("jr" TAB "%s", gpr[rs]); }

RVCNAM(jalr)(rid_t rs) : rv_inst(riscv::ra, rs) { GENINST("jalr" TAB "%s", gpr[rs]); }

RVCNAM(ret)() : rv_inst() { GENINST("ret"); }

RVCNAM(call)(cstr off) : rv_inst(riscv::ra) { GENINST("call" TAB "%s", off); }

RVCNAM(tail)(cstr off) : rv_inst(riscv::t1) { GENINST("tail" TAB "%s", off); }

//
// RV64I
//

RVCNAM(lb)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lb" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
}

RVCNAM(lh)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lh" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
}

RVCNAM(lw)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lw" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
}

RVCNAM(ld)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("ld" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
}

RVCNAM(lbu)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lbu" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
}

RVCNAM(lhu)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lhu" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
}

RVCNAM(lwu)(rid_t rd, rid_t rs, i64 off) : rv_inst(rd, rs) {
    GENINST("lwu" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rd], off, gpr[rs]);
    off_ = off;
}

RVCNAM(sb)(rid_t rs, rid_t rt, i64 off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("sb" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rs], off, gpr[rt]);
    off_ = off;
}

RVCNAM(sh)(rid_t rs, rid_t rt, i64 off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("sh" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rs], off, gpr[rt]);
    off_ = off;
}

RVCNAM(sw)(rid_t rs, rid_t rt, i64 off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("sw" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rs], off, gpr[rt]);
    off_ = off;
}

RVCNAM(sd)(rid_t rs, rid_t rt, i64 off) : rv_inst(riscv::zero, rs, rt) {
    GENINST("sd" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[rs], off, gpr[rt]);
    off_ = off;
}

RVCNAM(addi)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("addi" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(xori)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("xori" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(ori)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("ori" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(andi)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("andi" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(slli)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("slli" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(srli)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("srli" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(srai)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("srai" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(add)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("add" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(sub)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sub" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(sll)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sll" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM (xor)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("xor" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(srl)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("srl" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(sra)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sra" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(or)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("or" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM (and)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("and" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(addiw)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("addiw" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(slliw)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("slliw" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(srliw)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("srliw" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(sraiw)(rid_t rd, rid_t rs, i64 imm) : rv_inst(rd, rs) {
    GENINST("sraiw" TAB "%s" COMMA "%s" COMMA "%" PRId64, gpr[rd], gpr[rs], imm);
}

RVCNAM(addw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("addw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(subw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("subw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(sllw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sllw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(srlw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("srlw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(sraw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("sraw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

//
// RV64M
//

RVCNAM(mul)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mul" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(mulh)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mulh" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(mulhsu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mulhsu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(mulhu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mulhu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(div)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("div" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(divu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("divu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(rem)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("rem" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(remu)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("remu" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(mulw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("mulw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(divw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("divw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(divuw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("divuw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(remw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("remw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

RVCNAM(remuw)(rid_t rd, rid_t rs, rid_t rt) : rv_inst(rd, rs, rt) {
    GENINST("remuw" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[rs], gpr[rt]);
}

//
// RV64F
//

RVCNAM(flw)(rid_t frd, rid_t rs, i64 off) : rv_inst(frd, rs) {
    GENINST("flw" TAB "%s" COMMA "%" PRId64 "(%s)", gpr[frd], off, gpr[rs]);
}

RVCNAM(fsw)(rid_t frs, rid_t rt, i64 off) : rv_inst(riscv::zero, frs, rt) {
    GENINST("fsw" TAB TAB "%s" COMMA "%" PRId64 "(%s)", gpr[frs], off, gpr[rt]);
}

RVCNAM(fmadd_s)(rid_t frd, rid_t frs, rid_t frt, rid_t fra) : rv_inst(frd, frs, frt, fra) {
    GENINST("fmadd.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt], gpr[fra]);
}

RVCNAM(fmsub_s)(rid_t frd, rid_t frs, rid_t frt, rid_t fra) : rv_inst(frd, frs, frt, fra) {
    GENINST("fmsub.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt], gpr[fra]);
}

RVCNAM(fnmadd_s)(rid_t frd, rid_t frs, rid_t frt, rid_t fra) : rv_inst(frd, frs, frt, fra) {
    GENINST("fnmadd.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt], gpr[fra]);
}

RVCNAM(fnmsub_s)(rid_t frd, rid_t frs, rid_t frt, rid_t fra) : rv_inst(frd, frs, frt, fra) {
    GENINST("fnmsub.s" TAB "%s" COMMA "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt], gpr[fra]);
}

RVCNAM(fadd_s)(rid_t frd, rid_t frs, rid_t frt) : rv_inst(frd, frs, frt) {
    GENINST("fadd.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt]);
}

RVCNAM(fsub_s)(rid_t frd, rid_t frs, rid_t frt) : rv_inst(frd, frs, frt) {
    GENINST("fsub.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt]);
}

RVCNAM(fmul_s)(rid_t frd, rid_t frs, rid_t frt) : rv_inst(frd, frs, frt) {
    GENINST("fmul.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt]);
}

RVCNAM(fdiv_s)(rid_t frd, rid_t frs, rid_t frt) : rv_inst(frd, frs, frt) {
    GENINST("fdiv.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[frd], gpr[frs], gpr[frt]);
}

// 目前固定舍入方式为 rtz

RVCNAM(fcvt_w_s)(rid_t rd, rid_t frs) : rv_inst(rd, frs) {
    GENINST("fcvt.w.s" TAB "%s" COMMA "%s" COMMA "rtz", gpr[rd], gpr[frs]);
}

RVCNAM(fcvt_s_w)(rid_t frd, rid_t rs) : rv_inst(frd, rs) {
    GENINST("fcvt.s.w" TAB "%s" COMMA "%s" COMMA "rtz", gpr[frd], gpr[rs]);
}

RVCNAM(feq)(rid_t rd, rid_t frs, rid_t frt) : rv_inst(rd, frs, frt) {
    GENINST("feq.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[frs], gpr[frt]);
}

RVCNAM(flt)(rid_t rd, rid_t frs, rid_t frt) : rv_inst(rd, frs, frt) {
    GENINST("flt.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[frs], gpr[frt]);
}

RVCNAM(fle)(rid_t rd, rid_t frs, rid_t frt) : rv_inst(rd, frs, frt) {
    GENINST("fle.s" TAB "%s" COMMA "%s" COMMA "%s", gpr[rd], gpr[frs], gpr[frt]);
}

RVCNAM(fmv_w_x)(rid_t frd, rid_t rs) : rv_inst(frd, rs) { GENINST("fmv.w.x" TAB "%s" COMMA "%s", gpr[frd], gpr[rs]); }

RVCNAM(fmv_x_w)(rid_t rd, rid_t frs) : rv_inst(rd, frs) { GENINST("fmv.x.w" TAB "%s" COMMA "%s", gpr[rd], gpr[frs]); }
