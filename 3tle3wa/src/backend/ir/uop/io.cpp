#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/ir/virt/register.hh"

void uop_ret::format_str(FILE *fp) {
    // 返回值应当在返回前设置到 a0
    // 该操作交给 set param
    fprintf(fp, "ret");
}

void uop_set_iparam::format_str(FILE *fp) {
    //
    fprintf(fp, "mv a%lu, %s", idx_, rs_->c_str());
}

void uop_set_fparam::format_str(FILE *fp) {
    //
    fprintf(fp, "fmv.s fa%lu, %s", idx_, rs_->c_str());
}

void uop_call::format_str(FILE *fp) {
    //
    if (retval_) {
        fprintf(fp, "call %s => %s", callee_, retval_->c_str());
    } else {
        fprintf(fp, "call %s", callee_);
    }
}

void uop_li::format_str(FILE *fp) {
    //
    fprintf(fp, "li %s, %d", rd_->c_str(), imm_);
}

void uop_mv::format_str(FILE *fp) {
    if (rs_->kind() == VREG_KIND::STK) {
        fprintf(fp, "addi %s, fp, %lld # load %s", rd_->c_str(), rs_->sinfo()->off(), rs_->c_str());
        return;
    }
    fprintf(fp, "mv %s, %s", rd_->c_str(), rs_->c_str());
}

void uop_fmv::format_str(FILE *fp) {
    //
    fprintf(fp, "fmv.s %s, %s", rd_->c_str(), rs_->c_str());
}

void uop_cvts2w::format_str(FILE *fp) {
    //
    fprintf(fp, "fcvt.w.s %s, %s", rd_->c_str(), rs_->c_str());
}

void uop_cvtw2s::format_str(FILE *fp) {
    //
    fprintf(fp, "fcvt.s.w %s, %s", rd_->c_str(), rs_->c_str());
}

void uop_b::format_str(FILE *fp) {
    if (ontrue_) {
        fprintf(fp, "bnez %s, %s", cond_->c_str(), gen_pblk_label(lbid_).c_str());
        return;
    }
    fprintf(fp, "beqz %s, %s", cond_->c_str(), gen_pblk_label(lbid_).c_str());
}

void uop_icmp_b::format_str(FILE *fp) {
    switch (kind_) {
        case COMP_KIND::EQU:
            fprintf(fp, "beq %s, %s, %s", lhs_->c_str(), rhs_->c_str(), gen_pblk_label(lbid_).c_str());
            break;
        case COMP_KIND::NEQ:
            fprintf(fp, "bne %s, %s, %s", lhs_->c_str(), rhs_->c_str(), gen_pblk_label(lbid_).c_str());
            break;
        case COMP_KIND::LEQ:
            fprintf(fp, "ble %s, %s, %s", lhs_->c_str(), rhs_->c_str(), gen_pblk_label(lbid_).c_str());
            break;
        case COMP_KIND::GEQ:
            fprintf(fp, "bge %s, %s, %s", lhs_->c_str(), rhs_->c_str(), gen_pblk_label(lbid_).c_str());
            break;
        case COMP_KIND::LTH:
            fprintf(fp, "blt %s, %s, %s", lhs_->c_str(), rhs_->c_str(), gen_pblk_label(lbid_).c_str());
            break;
        case COMP_KIND::GTH:
            fprintf(fp, "bgt %s, %s, %s", lhs_->c_str(), rhs_->c_str(), gen_pblk_label(lbid_).c_str());
            break;
    }
}

void uop_j::format_str(FILE *fp) {
    //
    fprintf(fp, "j %s", gen_pblk_label(lbid_).c_str());
}

void uop_la::format_str(FILE *fp) {
    //
    fprintf(fp, "la %s, %s", rd_->c_str(), gen_glb_val_label(glb_idx_).c_str());
}

void uop_lla::format_str(FILE *fp) {
    //
    fprintf(fp, "lla %s, %s", rd_->c_str(), gen_glb_val_label(glb_idx_).c_str());
}

void uop_ld::format_str(FILE *fp) {
    switch (rd_->type()) {
        case VREG_TYPE::INT:
            fprintf(fp, "lw %s, %lld(%s)", rd_->c_str(), off_, rb_->c_str());
            break;
        case VREG_TYPE::PTR:
            fprintf(fp, "ld %s, %lld(%s)", rd_->c_str(), off_, rb_->c_str());
            break;
        default:
            panic("unexpected");
    }
}

void uop_st::format_str(FILE *fp) {
    switch (rd_->type()) {
        case VREG_TYPE::INT:
            fprintf(fp, "sw %s, %lld(%s)", rd_->c_str(), off_, rb_->c_str());
            break;
        case VREG_TYPE::PTR:
            fprintf(fp, "sd %s, %lld(%s)", rd_->c_str(), off_, rb_->c_str());
            break;
        default:
            panic("unexpected");
    }
}

void uop_ld_stk::format_str(FILE *fp) { fprintf(fp, "ld %s, %s", gpr[rd_], rb_->c_str()); }

void uop_st_stk::format_str(FILE *fp) { fprintf(fp, "sd %s, %s", gpr[rd_], rb_->c_str()); }

void uop_ld_l::format_str(FILE *fp) {
    // switch (rd_->type()) {
    //     case VREG_TYPE::INT:
    fprintf(fp, "lw %s, %s", rd_->c_str(), gen_glb_val_label(glb_idx_).c_str());
    //         break;
    // }
}

void uop_st_l::format_str(FILE *fp) {
    // switch (rd_->type()) {
    //     case VREG_TYPE::INT:
    fprintf(fp, "sw %s, %s, t2", rd_->c_str(), gen_glb_val_label(glb_idx_).c_str());
    //         break;
    // }
}

void uop_fld::format_str(FILE *fp) {
    // switch (rd_->type()) {
    //     case VREG_TYPE::FLT:
    fprintf(fp, "flw %s, %lld(%s)", rd_->c_str(), off_, rb_->c_str());
    //         break;
    // }
}

void uop_fst::format_str(FILE *fp) {
    // switch (rd_->type()) {
    //     case VREG_TYPE::FLT:
    fprintf(fp, "fsw %s, %lld(%s)", rd_->c_str(), off_, rb_->c_str());
    //         break;
    // }
}

void uop_fld_l::format_str(FILE *fp) {
    // switch (rd_->type()) {
    //     case VREG_TYPE::FLT:
    fprintf(fp, "flw %s, %s, t2", rd_->c_str(), gen_glb_val_label(glb_idx_).c_str());
    //         break;
    // }
}

void uop_fst_l::format_str(FILE *fp) {
    // switch (rd_->type()) {
    //     case VREG_TYPE::FLT:
    fprintf(fp, "fsw %s, %s, t2", rd_->c_str(), gen_glb_val_label(glb_idx_).c_str());
    //         break;
    // }
}

void uop_fld_ll::format_str(FILE *fp) {
    // switch (rd_->type()) {
    //     case VREG_TYPE::FLT:
    fprintf(fp, "flw %s, %s, %s", rd_->c_str(), gen_loc_cst_label(loc_idx_).c_str(), rt_->c_str());
    //         break;
    // }
}

void uop_icmp::format_str(FILE *fp) {
    switch (kind_) {
        case COMP_KIND::EQU:
            fprintf(fp, "seq %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::NEQ:
            fprintf(fp, "sne %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::LEQ:
            fprintf(fp, "sle %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::GEQ:
            fprintf(fp, "sge %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::LTH:
            fprintf(fp, "slt %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::GTH:
            fprintf(fp, "sgt %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
    }
}

void uop_fcmp::format_str(FILE *fp) {
    switch (kind_) {
        case COMP_KIND::EQU:
            fprintf(fp, "feq %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::NEQ:
            fprintf(fp, "fne %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::LEQ:
            fprintf(fp, "fle %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::GEQ:
            fprintf(fp, "fge %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::LTH:
            fprintf(fp, "flt %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case COMP_KIND::GTH:
            fprintf(fp, "fgt %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
    }
}

void uop_bin::format_str(FILE *fp) {
    switch (kind_) {
        case IBIN_KIND::ADD:
            fprintf(fp, "add %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case IBIN_KIND::SUB:
            fprintf(fp, "sub %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case IBIN_KIND::MUL:
            fprintf(fp, "mul %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case IBIN_KIND::DIV:
            fprintf(fp, "div %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case IBIN_KIND::REM:
            fprintf(fp, "rem %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case IBIN_KIND::SLL:
            fprintf(fp, "sll %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case IBIN_KIND::SRA:
            fprintf(fp, "sra %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
    }
}

void uop_fbin::format_str(FILE *fp) {
    switch (kind_) {
        case FBIN_KIND::ADD:
            fprintf(fp, "fadd.s %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case FBIN_KIND::SUB:
            fprintf(fp, "fsub.s %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case FBIN_KIND::MUL:
            fprintf(fp, "fmul.s %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
        case FBIN_KIND::DIV:
            fprintf(fp, "fdiv.s %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str());
            break;
    }
}

void uop_ftri::format_str(FILE *fp) {
    switch (kind_) {
        case FTRI_KIND::MADD:
            fprintf(fp, "fmadd.s %s, %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str(), ahs_->c_str());
            break;
        case FTRI_KIND::MSUB:
            fprintf(fp, "fmsub.s %s, %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str(), ahs_->c_str());
            break;
        case FTRI_KIND::NMADD:
            fprintf(fp, "fnmadd.s %s, %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str(), ahs_->c_str());
            break;
        case FTRI_KIND::NMSUB:
            fprintf(fp, "fnmsub.s %s, %s, %s, %s", rd_->c_str(), lhs_->c_str(), rhs_->c_str(), ahs_->c_str());
            break;
    }
}
