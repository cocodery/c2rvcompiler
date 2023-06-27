#pragma once

#include <cinttypes>

#include "3tle3wa/backend/asm/inst.hh"
#include "3tle3wa/backend/asm/riscv/def.hh"
#include "3tle3wa/utils/Logs.hh"

class rv_inst : public asm_inst {
   public:
    rv_inst(rid_t rd = 0, rid_t rs = 0, rid_t rt = 0, rid_t ra = 0);
    ~rv_inst();

    std::string_view to_string() final;
    std::string_view comment() final;
    void set_comment(cstr comt) final;

   public:
    const rid_t rd_;
    const rid_t rs_;
    const rid_t rt_;
    const rid_t ra_;
    i64 off_{};
    i64 imm_{};

   protected:
    char *inst_;
    cstr comt_;
};

#define RVINST(name, ...)              \
    class rv_##name : public rv_inst { \
       public:                         \
        rv_##name(__VA_ARGS__);        \
    };

//
// asm with label
//

RVINST(la_l, rid_t rd, cstr sym);
RVINST(lla_l, rid_t rd, cstr sym);
RVINST(lb_l, rid_t rd, cstr sym);
RVINST(lh_l, rid_t rd, cstr sym);
RVINST(lw_l, rid_t rd, cstr sym);
RVINST(ld_l, rid_t rd, cstr sym);
RVINST(sb_l, rid_t rd, cstr sym, rid_t rt);
RVINST(sh_l, rid_t rd, cstr sym, rid_t rt);
RVINST(sw_l, rid_t rd, cstr sym, rid_t rt);
RVINST(sd_l, rid_t rd, cstr sym, rid_t rt);
RVINST(flw_l, rid_t rd, cstr sym, rid_t rt);
RVINST(fld_l, rid_t rd, cstr sym, rid_t rt);
RVINST(fsw_l, rid_t rd, cstr sym, rid_t rt);
RVINST(fsd_l, rid_t rd, cstr sym, rid_t rt);

//
// misc int op
//

RVINST(nop);
RVINST(li, rid_t rd, i64 imm);
RVINST(mv, rid_t rd, rid_t rs);
RVINST(not, rid_t rd, rid_t rs);
RVINST(neg, rid_t rd, rid_t rs);
RVINST(negw, rid_t rd, rid_t rs);
RVINST(sext_w, rid_t rd, rid_t rs);

//
// set flag
//

RVINST(seqz, rid_t rd, rid_t rs);
RVINST(snez, rid_t rd, rid_t rs);
RVINST(sltz, rid_t rd, rid_t rs);
RVINST(sgtz, rid_t rd, rid_t rs);
RVINST(slt, rid_t rd, rid_t rs, rid_t rt);
RVINST(slti, rid_t rd, rid_t rs, i64 imm);
RVINST(sltiu, rid_t rd, rid_t rs, u64 imm);
RVINST(sgt, rid_t rd, rid_t rs, rid_t rt);
RVINST(sltu, rid_t rd, rid_t rs, rid_t rt);
RVINST(sgtu, rid_t rd, rid_t rs, rid_t rt);

//
// misc flt op
//

RVINST(fmv_s, rid_t frd, rid_t frs);
RVINST(fabs_s, rid_t frd, rid_t frs);
RVINST(fneg_s, rid_t frd, rid_t frs);

//
// branch with zero
//

RVINST(beqz, rid_t rs, cstr off);
RVINST(bnez, rid_t rs, cstr off);
RVINST(blez, rid_t rs, cstr off);
RVINST(bgez, rid_t rs, cstr off);
RVINST(bltz, rid_t rs, cstr off);
RVINST(bgtz, rid_t rs, cstr off);

//
// branch 2 op
//

RVINST(beq, rid_t rs, rid_t rt, cstr off);
RVINST(bne, rid_t rs, rid_t rt, cstr off);
RVINST(blt, rid_t rs, rid_t rt, cstr off);
RVINST(bge, rid_t rs, rid_t rt, cstr off);
RVINST(bgt, rid_t rs, rid_t rt, cstr off);
RVINST(ble, rid_t rs, rid_t rt, cstr off);
RVINST(bltu, rid_t rs, rid_t rt, cstr off);
RVINST(bgeu, rid_t rs, rid_t rt, cstr off);
RVINST(bgtu, rid_t rs, rid_t rt, cstr off);
RVINST(bleu, rid_t rs, rid_t rt, cstr off);

//
// j misc
//

RVINST(j, cstr off);
RVINST(jal, cstr off);
RVINST(jr, rid_t rs);
RVINST(jalr, rid_t rs);
RVINST(ret, );
RVINST(call, cstr off);
RVINST(tail, cstr off);

//
// RV64I
//

RVINST(lb, rid_t rd, rid_t rs, i64 off);
RVINST(lh, rid_t rd, rid_t rs, i64 off);
RVINST(lw, rid_t rd, rid_t rs, i64 off);
RVINST(ld, rid_t rd, rid_t rs, i64 off);
RVINST(lbu, rid_t rd, rid_t rs, i64 off);
RVINST(lhu, rid_t rd, rid_t rs, i64 off);
RVINST(lwu, rid_t rd, rid_t rs, i64 off);
RVINST(sb, rid_t rs, rid_t rt, i64 off);
RVINST(sh, rid_t rs, rid_t rt, i64 off);
RVINST(sw, rid_t rs, rid_t rt, i64 off);
RVINST(sd, rid_t rs, rid_t rt, i64 off);
RVINST(addi, rid_t rd, rid_t rs, i64 imm);
RVINST(xori, rid_t rd, rid_t rs, i64 imm);
RVINST(ori, rid_t rd, rid_t rs, i64 imm);
RVINST(andi, rid_t rd, rid_t rs, i64 imm);
RVINST(slli, rid_t rd, rid_t rs, i64 imm);
RVINST(srli, rid_t rd, rid_t rs, i64 imm);
RVINST(srai, rid_t rd, rid_t rs, i64 imm);
RVINST(add, rid_t rd, rid_t rs, rid_t rt);
RVINST(sub, rid_t rd, rid_t rs, rid_t rt);
RVINST(sll, rid_t rd, rid_t rs, rid_t rt);
RVINST(xor, rid_t rd, rid_t rs, rid_t rt);
RVINST(srl, rid_t rd, rid_t rs, rid_t rt);
RVINST(sra, rid_t rd, rid_t rs, rid_t rt);
RVINST(or, rid_t rd, rid_t rs, rid_t rt);
RVINST(and, rid_t rd, rid_t rs, rid_t rt);
RVINST(addiw, rid_t rd, rid_t rs, i64 imm);
RVINST(slliw, rid_t rd, rid_t rs, i64 imm);
RVINST(srliw, rid_t rd, rid_t rs, i64 imm);
RVINST(sraiw, rid_t rd, rid_t rs, i64 imm);
RVINST(addw, rid_t rd, rid_t rs, rid_t rt);
RVINST(subw, rid_t rd, rid_t rs, rid_t rt);
RVINST(sllw, rid_t rd, rid_t rs, rid_t rt);
RVINST(srlw, rid_t rd, rid_t rs, rid_t rt);
RVINST(sraw, rid_t rd, rid_t rs, rid_t rt);

//
// RV64M
//

RVINST(mul, rid_t rd, rid_t rs, rid_t rt);
RVINST(mulh, rid_t rd, rid_t rs, rid_t rt);
RVINST(mulhsu, rid_t rd, rid_t rs, rid_t rt);
RVINST(mulhu, rid_t rd, rid_t rs, rid_t rt);
RVINST(div, rid_t rd, rid_t rs, rid_t rt);
RVINST(divu, rid_t rd, rid_t rs, rid_t rt);
RVINST(rem, rid_t rd, rid_t rs, rid_t rt);
RVINST(remu, rid_t rd, rid_t rs, rid_t rt);
RVINST(mulw, rid_t rd, rid_t rs, rid_t rt);
RVINST(divw, rid_t rd, rid_t rs, rid_t rt);
RVINST(divuw, rid_t rd, rid_t rs, rid_t rt);
RVINST(remw, rid_t rd, rid_t rs, rid_t rt);
RVINST(remuw, rid_t rd, rid_t rs, rid_t rt);

//
// RV64F
//

RVINST(flw, rid_t frd, rid_t rs, i64 off);
RVINST(fsw, rid_t frs, rid_t rt, i64 off);
RVINST(fmadd_s, rid_t frd, rid_t frs, rid_t frt, rid_t fra);
RVINST(fmsub_s, rid_t frd, rid_t frs, rid_t frt, rid_t fra);
RVINST(fnmadd_s, rid_t frd, rid_t frs, rid_t frt, rid_t fra);
RVINST(fnmsub_s, rid_t frd, rid_t frs, rid_t frt, rid_t fra);
RVINST(fadd_s, rid_t frd, rid_t frs, rid_t frt);
RVINST(fsub_s, rid_t frd, rid_t frs, rid_t frt);
RVINST(fmul_s, rid_t frd, rid_t frs, rid_t frt);
RVINST(fdiv_s, rid_t frd, rid_t frs, rid_t frt);
RVINST(fcvt_w_s, rid_t rd, rid_t frs);
RVINST(fcvt_s_w, rid_t frd, rid_t rs);
RVINST(feq, rid_t rd, rid_t frs, rid_t frt);
RVINST(flt, rid_t rd, rid_t frs, rid_t frt);
RVINST(fle, rid_t rd, rid_t frs, rid_t frt);
RVINST(fmv_w_s, rid_t frd, rid_t rs);
RVINST(fmv_s_w, rid_t rd, rid_t frs);
