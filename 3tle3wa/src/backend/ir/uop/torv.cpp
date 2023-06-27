#include "3tle3wa/backend/asm/glb_value.hh"
#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/ir/virt/register.hh"

static inline size_t lea(pblock *pb, virt_reg *onstk, rid_t dst) {
    if (not onstk->onstk()) {
        return onstk->rregid();
    }
    auto &&sinfo = onstk->sinfo();
    auto off = sinfo->off();

    if (onstk->stkpp_) {
        if (imm_within(12, off)) {
            auto rv0 = new rv_ld(dst, riscv::fp, off);
            pb->push(rv0);
        } else {
            auto rv0 = new rv_li(dst, off);
            pb->push(rv0);

            auto rv1 = new rv_add(dst, riscv::fp, dst);
            pb->push(rv1);

            auto rv2 = new rv_ld(dst, dst, 0);
            pb->push(rv2);
        }
        return dst;
    }

    if (imm_within(12, off)) {
        auto rv = new rv_addi(dst, riscv::fp, off);
        pb->push(rv);
    } else {
        auto rv0 = new rv_li(dst, off);
        pb->push(rv0);

        auto rv1 = new rv_add(dst, riscv::fp, dst);
        pb->push(rv1);
    }
    return dst;
}

void uop_ret::toasm(pblock *pb) {
    auto rv = new rv_ret();
    pb->push(rv);
}

void uop_set_iparam::toasm(pblock *pb) {
    spack spk;
    if (idx_ < 8) {
        auto arg = riscv::a0 + idx_;
        if (rs_->kind() == VREG_KIND::IMM) {
            auto rv = new rv_li(arg, rs_->value());
            pb->push(rv);
            return;
        }
        if (rs_->kind() == VREG_KIND::STK) {
            auto rv = new rv_ld(arg, riscv::fp, rs_->sinfo()->off());
            pb->push(rv);
            return;
        }
        auto act = rs_->load(pb, spk, arg);
        if (act != arg) {
            auto rv = new rv_mv(arg, act);
            pb->push(rv);
        }
        return;
    }
    if (rs_->kind() == VREG_KIND::STK) {
        auto rv0 = new rv_ld(riscv::t0, riscv::fp, rs_->sinfo()->off());
        pb->push(rv0);

        auto rv1 = new rv_sd(riscv::t0, riscv::sp, pstk_ * 8);
        pb->push(rv1);
        return;
    }
    auto act = rs_->load(pb, spk);
    auto rv = new rv_sd(act, riscv::sp, pstk_ * 8);
    pb->push(rv);
}

void uop_set_fparam::toasm(pblock *pb) {
    spack spk;
    if (idx_ < 8) {
        auto arg = riscv::fa0 + idx_;
        if (rs_->kind() == VREG_KIND::LOC) {
            auto rv = new rv_flw_l(arg, gen_loc_cst_label(rs_->value()).c_str(), riscv::t2);
            pb->push(rv);
            return;
        }
        if (rs_->kind() == VREG_KIND::STK) {
            auto rv = new rv_flw(arg, riscv::fp, rs_->sinfo()->off());
            pb->push(rv);
            return;
        }
        auto act = rs_->load(pb, spk, arg);
        if (act != arg) {
            auto rv = new rv_fmv_s(arg, act);
            pb->push(rv);
        }
        return;
    }
    if (rs_->kind() == VREG_KIND::STK) {
        auto rv0 = new rv_flw(riscv::fs0, riscv::fp, rs_->sinfo()->off());
        pb->push(rv0);

        auto rv1 = new rv_fsw(riscv::fs0, riscv::sp, pstk_ * 8);
        pb->push(rv1);
        return;
    }
    auto act = rs_->load(pb, spk);
    auto rv = new rv_fsw(act, riscv::sp, pstk_ * 8);
    pb->push(rv);
}

void uop_call::toasm(pblock *pb) {
    auto rv = new rv_call(callee_);
    pb->push(rv);
}

void uop_li::toasm(pblock *pb) { panic("unexpected now"); }

void uop_mv::toasm(pblock *pb) {
    spack spk;
    auto to = rd_->store_where();
    if (rs_->kind() == VREG_KIND::STK) {
        lea(pb, rs_, to);
    } else if (rs_->kind() == VREG_KIND::IMM and not rd_->onstk() and rd_->kind() == VREG_KIND::REG) {
        auto rv = new rv_li(to, rs_->value());
        pb->push(rv);
    } else if (rs_->kind() == VREG_KIND::SPEC) {
        if (rs_->value() < 32) {
            auto act = rs_->load(pb, spk, to);
            if (act != to) {
                auto rv = new rv_mv(to, act);
                pb->push(rv);
            }
        } else {
            auto act = rs_->load(pb, spk, to);
            if (act != to) {
                auto rv = new rv_fmv_s(to, act);
                pb->push(rv);
            }
        }
    } else {
        auto act = rs_->load(pb, spk, to);
        if (act != to) {
            auto rv = new rv_mv(to, act);
            pb->push(rv);
        }
    }
    rd_->store(pb, to);
}

void uop_fmv::toasm(pblock *pb) {
    spack spk;
    auto to = rd_->store_where();
    if (rs_->kind() == VREG_KIND::STK) {
        lea(pb, rs_, to);
    } else {
        auto act = rs_->load(pb, spk, to);
        if (act != to) {
            auto rv = new rv_fmv_s(to, act);
            pb->push(rv);
        }
    }
    rd_->store(pb, to);
}

void uop_cvts2w::toasm(pblock *pb) {
    spack spk0;
    auto single = rs_->load(pb, spk0);

    auto to = rd_->store_where();

    auto rv = new rv_fcvt_w_s(to, single);
    pb->push(rv);

    rd_->store(pb, to);
}

void uop_cvtw2s::toasm(pblock *pb) {
    spack spk0;
    auto integer = rs_->load(pb, spk0);

    auto to = rd_->store_where();

    auto rv = new rv_fcvt_s_w(to, integer);
    pb->push(rv);

    rd_->store(pb, to);
}

void uop_b::toasm(pblock *pb) {
    spack spk;
    auto act = cond_->load(pb, spk);
    if (ontrue_) {
        auto rv = new rv_bnez(act, gen_pblk_label(lbid_).c_str());
        pb->push(rv);
        return;
    }
    auto rv = new rv_beqz(act, gen_pblk_label(lbid_).c_str());
    pb->push(rv);
}

void uop_icmp_b::toasm(pblock *pb) {
    spack spk;
    auto lhs = lhs_->load(pb, spk);
    auto rhs = rhs_->load(pb, spk);

    switch (kind_) {
        case COMP_KIND::EQU: {
            auto rv = new rv_beq(lhs, rhs, gen_pblk_label(lbid_).c_str());
            pb->push(rv);
        } break;

        case COMP_KIND::NEQ: {
            auto rv = new rv_bne(lhs, rhs, gen_pblk_label(lbid_).c_str());
            pb->push(rv);
        } break;

        case COMP_KIND::LTH: {
            auto rv = new rv_blt(lhs, rhs, gen_pblk_label(lbid_).c_str());
            pb->push(rv);
        } break;

        case COMP_KIND::GTH: {
            auto rv = new rv_bgt(lhs, rhs, gen_pblk_label(lbid_).c_str());
            pb->push(rv);
        } break;

        case COMP_KIND::LEQ: {
            auto rv = new rv_ble(lhs, rhs, gen_pblk_label(lbid_).c_str());
            pb->push(rv);
        } break;

        case COMP_KIND::GEQ: {
            auto rv = new rv_bge(lhs, rhs, gen_pblk_label(lbid_).c_str());
            pb->push(rv);
        } break;
    }
}

void uop_j::toasm(pblock *pb) {
    auto rv = new rv_j(gen_pblk_label(lbid_).c_str());
    pb->push(rv);
}

void uop_la::toasm(pblock *pb) { panic("unexpected now"); }

void uop_lla::toasm(pblock *pb) {
    auto act = rd_->store_where();

    auto ginfo = pb->father_->ginfo(glb_idx_);

    if (ginfo->onheap()) {
        auto rv = new rv_ld_l(act, ginfo->name().c_str());
        pb->push(rv);
        rd_->store(pb, act);
    }

    auto rv = new rv_lla_l(act, ginfo->name().c_str());
    pb->push(rv);
    rd_->store(pb, act);
}

void uop_ld::toasm(pblock *pb) {
    auto to = rd_->store_where();
    auto rb = lea(pb, rb_, riscv::t2);

    switch (rd_->type()) {
        case VREG_TYPE::INT: {
            if (imm_within(12, off_)) {
                auto rv = new rv_lw(to, rb, off_);
                pb->push(rv);
                rd_->store(pb, to);
            } else {
                auto rv0 = new rv_li(riscv::t0, off_);
                auto rv1 = new rv_add(riscv::t0, rb, riscv::t0);
                auto rv2 = new rv_lw(to, riscv::t0, 0);
                pb->push(rv0);
                pb->push(rv1);
                pb->push(rv2);
                rd_->store(pb, to);
            }
        } break;

        case VREG_TYPE::PTR: {
            if (imm_within(12, off_)) {
                auto rv = new rv_ld(to, rb, off_);
                pb->push(rv);
                rd_->store(pb, to);
            } else {
                auto rv0 = new rv_li(riscv::t0, off_);
                auto rv1 = new rv_add(riscv::t0, rb, riscv::t0);
                auto rv2 = new rv_ld(to, riscv::t0, 0);
                pb->push(rv0);
                pb->push(rv1);
                pb->push(rv2);
                rd_->store(pb, to);
            }
        } break;
        default:
            panic("unexpected");
    }
}

void uop_st::toasm(pblock *pb) {
    spack spk;
    auto src = rd_->load(pb, spk);
    auto rb = lea(pb, rb_, riscv::t2);

    switch (rd_->type()) {
        case VREG_TYPE::INT: {
            if (imm_within(12, off_)) {
                auto rv = new rv_sw(src, rb, off_);
                pb->push(rv);
            } else {
                if (src == riscv::t0) {
                    auto rv0 = new rv_li(riscv::t1, off_);
                    auto rv1 = new rv_add(riscv::t1, rb, riscv::t1);
                    auto rv2 = new rv_sw(src, riscv::t1, 0);
                    pb->push(rv0);
                    pb->push(rv1);
                    pb->push(rv2);
                } else {
                    auto rv0 = new rv_li(riscv::t0, off_);
                    auto rv1 = new rv_add(riscv::t0, rb, riscv::t0);
                    auto rv2 = new rv_sw(src, riscv::t0, 0);
                    pb->push(rv0);
                    pb->push(rv1);
                    pb->push(rv2);
                }
            }
        } break;

        case VREG_TYPE::PTR: {
            if (imm_within(12, off_)) {
                auto rv = new rv_sd(src, rb, off_);
                pb->push(rv);
            } else {
                if (src == riscv::t0) {
                    auto rv0 = new rv_li(riscv::t1, off_);
                    auto rv1 = new rv_add(riscv::t1, rb, riscv::t1);
                    auto rv2 = new rv_sd(src, riscv::t1, 0);
                    pb->push(rv0);
                    pb->push(rv1);
                    pb->push(rv2);
                } else {
                    auto rv0 = new rv_li(riscv::t0, off_);
                    auto rv1 = new rv_add(riscv::t0, rb, riscv::t0);
                    auto rv2 = new rv_sd(src, riscv::t0, 0);
                    pb->push(rv0);
                    pb->push(rv1);
                    pb->push(rv2);
                }
            }
        } break;

        default:
            panic("unexpected");
    }
}

void uop_ld_stk::toasm(pblock *pb) {
    Assert(rb_->onstk(), "logic error");
    if (rd_ == riscv::zero) {
        return;
    }

    auto off = rb_->sinfo()->off();

    if (rd_ > 31) {
        if (imm_within(12, off)) {
            auto rv = new rv_flw(rd_, riscv::fp, off);
            pb->push(rv);
        } else {
            auto rv0 = new rv_li(riscv::t0, off);
            pb->push(rv0);

            auto rv1 = new rv_add(riscv::t0, riscv::fp, riscv::t0);
            pb->push(rv1);

            auto rv2 = new rv_flw(rd_, riscv::t0, 0);
            pb->push(rv2);
        }
        return;
    }

    if (imm_within(12, off)) {
        auto rv = new rv_ld(rd_, riscv::fp, off);
        pb->push(rv);
    } else {
        auto rv0 = new rv_li(riscv::t0, off);
        pb->push(rv0);

        auto rv1 = new rv_add(riscv::t0, riscv::fp, riscv::t0);
        pb->push(rv1);

        auto rv2 = new rv_ld(rd_, riscv::t0, 0);
        pb->push(rv2);
    }
}

void uop_st_stk::toasm(pblock *pb) {
    Assert(rb_->onstk(), "logic error");
    if (rd_ == riscv::zero) {
        return;
    }

    auto off = rb_->sinfo()->off();

    if (rd_ > 31) {
        if (imm_within(12, off)) {
            auto rv = new rv_fsw(rd_, riscv::fp, off);
            pb->push(rv);
        } else {
            auto rv0 = new rv_li(riscv::t0, off);
            pb->push(rv0);

            auto rv1 = new rv_add(riscv::t0, riscv::fp, riscv::t0);
            pb->push(rv1);

            auto rv2 = new rv_fsw(rd_, riscv::t0, 0);
            pb->push(rv2);
        }
        return;
    }

    if (imm_within(12, off)) {
        auto rv = new rv_sd(rd_, riscv::fp, off);
        pb->push(rv);
    } else {
        auto rv0 = new rv_li(riscv::t0, off);
        pb->push(rv0);

        auto rv1 = new rv_add(riscv::t0, riscv::fp, riscv::t0);
        pb->push(rv1);

        auto rv2 = new rv_sd(rd_, riscv::t0, 0);
        pb->push(rv2);
    }
}

void uop_ld_l::toasm(pblock *pb) {
    auto to = rd_->store_where();

    auto ginfo = pb->father_->ginfo(glb_idx_);
    Assert(ginfo->onheap() == false, "can't change heap value via ld/st");

    auto rv = new rv_lw_l(to, ginfo->name().c_str());
    pb->push(rv);

    rd_->store(pb, to);
}

void uop_st_l::toasm(pblock *pb) {
    spack spk;
    auto src = rd_->load(pb, spk);

    auto ginfo = pb->father_->ginfo(glb_idx_);
    Assert(ginfo->onheap() == false, "can't change heap value via ld/st");

    auto rv = new rv_sw_l(src, ginfo->name().c_str(), riscv::t2);
    pb->push(rv);
}

void uop_fld::toasm(pblock *pb) {
    auto to = rd_->store_where();
    auto rb = lea(pb, rb_, riscv::t2);

    switch (rd_->type()) {
        case VREG_TYPE::FLT: {
            if (imm_within(12, off_)) {
                auto rv = new rv_flw(to, rb, off_);
                pb->push(rv);
                rd_->store(pb, to);
            } else {
                auto rv0 = new rv_li(riscv::t0, off_);
                auto rv1 = new rv_add(riscv::t0, rb, riscv::t0);
                auto rv2 = new rv_flw(to, riscv::t0, 0);
                pb->push(rv0);
                pb->push(rv1);
                pb->push(rv2);
                rd_->store(pb, to);
            }
        } break;

        default:
            panic("unexpected");
    }
}

void uop_fst::toasm(pblock *pb) {
    spack spk;
    auto src = rd_->load(pb, spk);
    auto rb = lea(pb, rb_, riscv::t2);
    switch (rd_->type()) {
        case VREG_TYPE::FLT: {
            if (imm_within(12, off_)) {
                auto rv = new rv_fsw(src, rb, off_);
                pb->push(rv);
            } else {
                auto rv0 = new rv_li(riscv::t0, off_);
                auto rv1 = new rv_add(riscv::t0, rb, riscv::t0);
                auto rv2 = new rv_fsw(src, riscv::t0, 0);
                pb->push(rv0);
                pb->push(rv1);
                pb->push(rv2);
            }
        } break;

        default:
            panic("unexpected");
    }
}

void uop_fld_l::toasm(pblock *pb) {
    auto to = rd_->store_where();

    auto ginfo = pb->father_->ginfo(glb_idx_);
    Assert(ginfo->onheap() == false, "can't change heap value via ld/st");

    auto rv = new rv_fsw_l(to, ginfo->name().c_str(), riscv::t2);
    pb->push(rv);

    rd_->store(pb, to);
}

void uop_fst_l::toasm(pblock *pb) {
    spack spk;
    auto src = rd_->load(pb, spk);

    auto ginfo = pb->father_->ginfo(glb_idx_);
    Assert(ginfo->onheap() == false, "can't change heap value via ld/st");

    auto rv = new rv_fsw_l(src, ginfo->name().c_str(), riscv::t2);
    pb->push(rv);
}

void uop_fld_ll::toasm(pblock *pb) {
    auto to = rd_->store_where();

    auto rv = new rv_fld_l(to, gen_loc_cst_label(loc_idx_).c_str(), riscv::t2);
    pb->push(rv);

    rd_->store(pb, to);
}

void uop_icmp::toasm(pblock *pb) {
    spack spk;
    auto dst = rd_->store_where();
    auto lhs = lhs_->load(pb, spk);
    auto rhs = rhs_->load(pb, spk);

    switch (kind_) {
        case COMP_KIND::EQU: {
            auto rv0 = new rv_xor(dst, lhs, rhs);
            pb->push(rv0);

            auto rv1 = new rv_seqz(dst, dst);
            pb->push(rv1);
        } break;

        case COMP_KIND::NEQ: {
            auto rv0 = new rv_xor(dst, lhs, rhs);
            pb->push(rv0);

            auto rv1 = new rv_snez(dst, dst);
            pb->push(rv1);
        } break;

        case COMP_KIND::LTH: {
            auto rv = new rv_slt(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case COMP_KIND::GTH: {
            auto rv = new rv_slt(dst, rhs, lhs);
            pb->push(rv);
        } break;

        case COMP_KIND::LEQ: {
            auto rv0 = new rv_slt(dst, rhs, lhs);
            pb->push(rv0);

            auto rv1 = new rv_seqz(dst, dst);
            pb->push(rv1);
        } break;

        case COMP_KIND::GEQ: {
            auto rv0 = new rv_slt(dst, lhs, rhs);
            pb->push(rv0);

            auto rv1 = new rv_seqz(dst, dst);
            pb->push(rv1);
        } break;
    }

    rd_->store(pb, dst);
}

void uop_fcmp::toasm(pblock *pb) {
    spack spk;
    auto dst = rd_->store_where();
    auto lhs = lhs_->load(pb, spk);
    auto rhs = rhs_->load(pb, spk);

    switch (kind_) {
        case COMP_KIND::EQU: {
            auto rv = new rv_feq(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case COMP_KIND::NEQ: {
            auto rv0 = new rv_feq(dst, lhs, rhs);
            pb->push(rv0);

            auto rv1 = new rv_seqz(dst, dst);
            pb->push(rv1);
        } break;

        case COMP_KIND::LTH: {
            auto rv = new rv_flt(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case COMP_KIND::GTH: {
            auto rv = new rv_flt(dst, rhs, lhs);
            pb->push(rv);
        } break;

        case COMP_KIND::LEQ: {
            auto rv = new rv_fle(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case COMP_KIND::GEQ: {
            auto rv = new rv_fle(dst, rhs, lhs);
            pb->push(rv);
        } break;
    }

    rd_->store(pb, dst);
}

void uop_bin::toasm(pblock *pb) {
    spack spk;

    auto dst = rd_->store_where();

    if (rd_->type() == VREG_TYPE::PTR) {
        switch (kind_) {
            case IBIN_KIND::ADD: {
                if (rd_ == lhs_ or rd_ == rhs_) {
                    if (lhs_->kind() == VREG_KIND::IMM and lhs_->value() == 0) return;
                    if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 0) return;
                }

                if (rhs_->kind() == VREG_KIND::IMM and imm_within(12, rhs_->value())) {
                    auto lhs = lhs_->load(pb, spk);
                    auto rv = new rv_addi(dst, lhs, (int32_t)rhs_->value());
                    pb->push(rv);
                    rd_->store(pb);
                    return;
                }

                if (lhs_->kind() == VREG_KIND::IMM and imm_within(12, lhs_->value())) {
                    auto rhs = rhs_->load(pb, spk);
                    auto rv = new rv_addi(dst, rhs, (int32_t)lhs_->value());
                    pb->push(rv);
                    rd_->store(pb);
                    return;
                }
            } break;

            case IBIN_KIND::SUB: {
                if (rd_ == lhs_) {
                    if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 0) return;
                }

                if (rhs_->kind() == VREG_KIND::IMM and imm_within(12, rhs_->value())) {
                    auto lhs = lhs_->load(pb, spk);
                    auto rv = new rv_addi(dst, lhs, -(int32_t)rhs_->value());
                    pb->push(rv);
                    rd_->store(pb);
                    return;
                }
            } break;

            case IBIN_KIND::MUL: {
                if (rd_ == lhs_ or rd_ == rhs_) {
                    if (lhs_->kind() == VREG_KIND::IMM and lhs_->value() == 1) return;
                    if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 1) return;
                }
            } break;

            case IBIN_KIND::SLL: {
                if (rd_ == lhs_) {
                    if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 0) return;
                }

                if (rhs_->kind() == VREG_KIND::IMM and imm_within(12, rhs_->value())) {
                    auto lhs = lhs_->load(pb, spk);
                    auto rv = new rv_slli(dst, lhs, rhs_->value());
                    pb->push(rv);
                    rd_->store(pb);
                    return;
                }
            } break;

            default:
                break;
        }
    }

    switch (kind_) {
        case IBIN_KIND::ADD: {
            if (rd_ == lhs_ or rd_ == rhs_) {
                if (lhs_->kind() == VREG_KIND::IMM and lhs_->value() == 0) return;
                if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 0) return;
            }

            if (rhs_->kind() == VREG_KIND::IMM and imm_within(12, rhs_->value())) {
                auto lhs = lhs_->load(pb, spk);
                auto rv = new rv_addiw(dst, lhs, (int32_t)rhs_->value());
                pb->push(rv);
                rd_->store(pb);
                return;
            }

            if (lhs_->kind() == VREG_KIND::IMM and imm_within(12, lhs_->value())) {
                auto rhs = rhs_->load(pb, spk);
                auto rv = new rv_addiw(dst, rhs, (int32_t)lhs_->value());
                pb->push(rv);
                rd_->store(pb);
                return;
            }
        } break;

        case IBIN_KIND::SUB: {
            if (rd_ == lhs_) {
                if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 0) return;
            }

            if (rhs_->kind() == VREG_KIND::IMM and imm_within(12, rhs_->value())) {
                auto lhs = lhs_->load(pb, spk);
                auto rv = new rv_addiw(dst, lhs, -(int32_t)rhs_->value());
                pb->push(rv);
                rd_->store(pb);
                return;
            }
        } break;

        case IBIN_KIND::MUL: {
            if (rd_ == lhs_ or rd_ == rhs_) {
                if (lhs_->kind() == VREG_KIND::IMM and lhs_->value() == 1) return;
                if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 1) return;
            }
            if (rhs_->kind() == VREG_KIND::IMM and __builtin_popcount(rhs_->value()) == 1) {
                auto lhs = lhs_->load(pb, spk);
                auto rv = new rv_slliw(dst, lhs, __builtin_ctz(rhs_->value()));
                pb->push(rv);
                rd_->store(pb);
                return;
            }
            if (lhs_->kind() == VREG_KIND::IMM and __builtin_popcount(lhs_->value()) == 1) {
                auto rhs = rhs_->load(pb, spk);
                auto rv = new rv_slliw(dst, rhs, __builtin_ctz(lhs_->value()));
                pb->push(rv);
                rd_->store(pb);
                return;
            }
        } break;

        case IBIN_KIND::DIV: {
            if (rd_ == lhs_) {
                if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 1) return;
            }
            if (rhs_->kind() == VREG_KIND::IMM and __builtin_popcount(rhs_->value()) == 1) {
                auto lhs = lhs_->load(pb, spk);
                auto ctz = __builtin_ctz(rhs_->value());

                auto sraiw_op = new rv_sraiw(riscv::t2, lhs, 31);
                pb->push(sraiw_op);

                auto srliw_op = new rv_srliw(riscv::t2, riscv::t2, 32 - ctz);
                pb->push(srliw_op);

                auto addw_op = new rv_addw(riscv::t2, riscv::t2, lhs);
                pb->push(addw_op);

                auto rv = new rv_sraiw(dst, riscv::t2, ctz);
                pb->push(rv);

                rd_->store(pb);
                return;
            }
        } break;

        case IBIN_KIND::REM: {
            if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 1) {
                auto rv = new rv_li(dst, 0);
                pb->push(rv);
                rd_->store(pb);
                return;
            }
            if (rhs_->kind() == VREG_KIND::IMM and __builtin_popcount(rhs_->value()) == 1) {
                auto lhs = lhs_->load(pb, spk);
                auto ctz = __builtin_ctz(rhs_->value());

                auto sraiw_op = new rv_sraiw(riscv::t0, lhs, 31);
                pb->push(sraiw_op);

                auto srliw_op = new rv_srliw(riscv::t0, riscv::t0, 32 - ctz);
                pb->push(srliw_op);

                auto addw_op = new rv_addw(dst, riscv::t0, lhs);
                pb->push(addw_op);

                auto slli_op = new rv_slli(dst, dst, 64 - ctz);
                pb->push(slli_op);

                auto srli_op = new rv_srli(dst, dst, 64 - ctz);
                pb->push(srli_op);

                auto subw_op = new rv_subw(dst, dst, riscv::t0);
                pb->push(subw_op);

                rd_->store(pb);
                return;
            }
        } break;

        case IBIN_KIND::SLL: {
            if (rd_ == lhs_) {
                if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 0) return;
            }

            if (rhs_->kind() == VREG_KIND::IMM and imm_within(12, rhs_->value())) {
                auto lhs = lhs_->load(pb, spk);
                auto rv = new rv_slliw(dst, lhs, rhs_->value());
                pb->push(rv);
                rd_->store(pb);
                return;
            }
        } break;

        case IBIN_KIND::SRA: {
            if (rd_ == lhs_) {
                if (rhs_->kind() == VREG_KIND::IMM and rhs_->value() == 0) return;
            }

            if (rhs_->kind() == VREG_KIND::IMM and imm_within(12, rhs_->value())) {
                auto lhs = lhs_->load(pb, spk);
                auto rv = new rv_sraiw(dst, lhs, rhs_->value());
                pb->push(rv);
                rd_->store(pb);
                return;
            }
        } break;

        default:
            break;
    }

    auto lhs = lhs_->load(pb, spk);
    auto rhs = rhs_->load(pb, spk);

    if (rd_->type() == VREG_TYPE::PTR) {
        if (lhs_->type() == VREG_TYPE::INT) {
            auto rv = new rv_sext_w(lhs, lhs);
            pb->push(rv);
        }
        if (rhs_->type() == VREG_TYPE::INT) {
            auto rv = new rv_sext_w(rhs, rhs);
            pb->push(rv);
        }
        switch (kind_) {
            case IBIN_KIND::ADD: {
                auto rv = new rv_add(dst, lhs, rhs);
                pb->push(rv);
            } break;

            case IBIN_KIND::SUB: {
                auto rv = new rv_sub(dst, lhs, rhs);
                pb->push(rv);
            } break;

            case IBIN_KIND::SLL: {
                auto rv = new rv_sll(dst, lhs, rhs);
                pb->push(rv);
            } break;

            case IBIN_KIND::MUL: {
                auto rv = new rv_mul(dst, lhs, rhs);
                pb->push(rv);
            } break;

            default:
                panic("unexpected");
        }

        rd_->store(pb, dst);
        return;
    }

    switch (kind_) {
        case IBIN_KIND::ADD: {
            auto rv = new rv_addw(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case IBIN_KIND::SUB: {
            auto rv = new rv_subw(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case IBIN_KIND::MUL: {
            auto rv = new rv_mulw(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case IBIN_KIND::DIV: {
            auto rv = new rv_divw(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case IBIN_KIND::REM: {
            auto rv = new rv_remw(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case IBIN_KIND::SLL: {
            auto rv = new rv_sllw(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case IBIN_KIND::SRA: {
            auto rv = new rv_sraw(dst, lhs, rhs);
            pb->push(rv);
        } break;
    }

    rd_->store(pb, dst);
}

void uop_fbin::toasm(pblock *pb) {
    spack spk;
    auto dst = rd_->store_where();
    auto lhs = lhs_->load(pb, spk);
    auto rhs = rhs_->load(pb, spk);

    switch (kind_) {
        case FBIN_KIND::ADD: {
            auto rv = new rv_fadd_s(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case FBIN_KIND::SUB: {
            auto rv = new rv_fsub_s(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case FBIN_KIND::MUL: {
            auto rv = new rv_fmul_s(dst, lhs, rhs);
            pb->push(rv);
        } break;

        case FBIN_KIND::DIV: {
            auto rv = new rv_fdiv_s(dst, lhs, rhs);
            pb->push(rv);
        } break;
    }

    rd_->store(pb, dst);
}

void uop_ftri::toasm(pblock *pb) {
    spack spk;
    auto dst = rd_->store_where();
    auto lhs = lhs_->load(pb, spk);
    auto rhs = rhs_->load(pb, spk);
    auto ahs = ahs_->load(pb, spk);

    switch (kind_) {
        case FTRI_KIND::MADD: {
            auto rv = new rv_fmadd_s(dst, lhs, rhs, ahs);
            pb->push(rv);
        } break;

        case FTRI_KIND::MSUB: {
            auto rv = new rv_fmsub_s(dst, lhs, rhs, ahs);
            pb->push(rv);
        } break;

        case FTRI_KIND::NMADD: {
            auto rv = new rv_fnmadd_s(dst, lhs, rhs, ahs);
            pb->push(rv);
        } break;

        case FTRI_KIND::NMSUB: {
            auto rv = new rv_fnmsub_s(dst, lhs, rhs, ahs);
            pb->push(rv);
        } break;
    }

    rd_->store(pb, dst);
}
