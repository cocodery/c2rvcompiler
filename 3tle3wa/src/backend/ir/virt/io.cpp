#include "3tle3wa/backend/ir/virt/register.hh"

void virt_reg::format_str(FILE *fp) {
    static const char *tynm[4];
    tynm[(size_t)VREG_TYPE::INT] = "int";
    tynm[(size_t)VREG_TYPE::FLT] = "flt";
    tynm[(size_t)VREG_TYPE::ARR] = "arr";
    tynm[(size_t)VREG_TYPE::PTR] = "ptr";

    switch (kind_) {
        case VREG_KIND::IMM:
            fprintf(fp, "%" PRId64, (xlen_t)value_);
            break;

        case VREG_KIND::LOC:
            union {
                float f;
                uint32_t iu32;
            } reintp;

            reintp.iu32 = value_;
            fprintf(fp, "%.3f", reintp.f);
            break;

        case VREG_KIND::PRM: {
            if (onstk_) {
                fprintf(fp, "%" PRId64 "(fp)", pstk_ * 8);
            } else {
                fprintf(fp, "%s", gpr[rregid_]);
            }
        } break;

        case VREG_KIND::REG:
            if (confirm_) {
                fprintf(fp, "%%r%" PRIu64 "<%s>", value_, gpr[rregid_]);
            } else {
                fprintf(fp, "%%r%" PRIu64, value_);
            }
            break;

        case VREG_KIND::SPEC: {
            fprintf(fp, "%s", gpr[value_]);
        } break;

        case VREG_KIND::STK: {
            if (type_ == VREG_TYPE::ARR) {
                fprintf(fp, "STK: (%s, %lu, %ld)", tynm[(size_t)type_], sinfo_->slen() * 4, sinfo_->off());
                break;
            }
            fprintf(fp, "STK: (%s, %d, %ld)", tynm[(size_t)type_], (type_ == VREG_TYPE::PTR ? 8 : 4), sinfo_->off());
        } break;

        case VREG_KIND::ZERO: {
            fprintf(fp, "%s", gpr[0]);
        } break;

        default:
            panic("unexpected");
            break;
    }
}

void vr_allocor::prinfo(std::fstream &fs) {
    fs << std::endl;
    fs << "=============== stack info ===============" << std::endl;

    fs << "Stack Size: " << total_stk_len << std::endl;
}

static inline void popout(pblock *pb, virt_reg *onstk, rid_t dst) {
    Assert(onstk->onstk(), "must on stack");
    auto &&sinfo = onstk->sinfo();
    auto off = sinfo->off();
    if (imm_within(12, off)) {
        if (onstk->type() == VREG_TYPE::FLT) {
            auto rv = new rv_flw(dst, riscv::fp, off);
            pb->push(rv);
        } else if (onstk->type() == VREG_TYPE::INT) {
            auto rv = new rv_lw(dst, riscv::fp, off);
            pb->push(rv);
        } else if (onstk->type() == VREG_TYPE::PTR) {
            auto rv = new rv_ld(dst, riscv::fp, off);
            pb->push(rv);
        }
    } else if (onstk->type() == VREG_TYPE::FLT) {
        auto rv0 = new rv_li(riscv::t0, off);
        pb->push(rv0);

        auto rv1 = new rv_add(riscv::t0, riscv::fp, riscv::t0);
        pb->push(rv1);

        auto rv2 = new rv_flw(dst, riscv::t0, 0);
        pb->push(rv2);
    } else {
        auto rv0 = new rv_li(dst, off);
        pb->push(rv0);

        auto rv1 = new rv_add(dst, riscv::fp, dst);
        pb->push(rv1);

        if (onstk->type() == VREG_TYPE::INT) {
            auto rv2 = new rv_lw(dst, dst, 0);
            pb->push(rv2);
        } else if (onstk->type() == VREG_TYPE::PTR) {
            auto rv2 = new rv_ld(dst, dst, 0);
            pb->push(rv2);
        }
    }
}

static inline void pushin(pblock *pb, virt_reg *onstk, rid_t src) {
    Assert(onstk->onstk(), "must on stack");
    auto &&sinfo = onstk->sinfo();
    auto off = sinfo->off();
    if (imm_within(12, off)) {
        if (onstk->type() == VREG_TYPE::FLT) {
            auto rv = new rv_fsw(src, riscv::fp, off);
            pb->push(rv);
        } else if (onstk->type() == VREG_TYPE::INT) {
            auto rv = new rv_sw(src, riscv::fp, off);
            pb->push(rv);
        } else if (onstk->type() == VREG_TYPE::PTR) {
            auto rv = new rv_sd(src, riscv::fp, off);
            pb->push(rv);
        } else {
            panic("unexpected");
        }
    } else {
        auto rv0 = new rv_li(riscv::t0, off);
        pb->push(rv0);

        auto rv1 = new rv_add(riscv::t0, riscv::fp, riscv::t0);
        pb->push(rv1);

        if (onstk->type() == VREG_TYPE::FLT) {
            auto rv2 = new rv_fsw(src, riscv::t0, 0);
            pb->push(rv2);
        } else if (onstk->type() == VREG_TYPE::INT) {
            auto rv2 = new rv_sw(src, riscv::t0, 0);
            pb->push(rv2);
        } else if (onstk->type() == VREG_TYPE::PTR) {
            auto rv2 = new rv_sd(src, riscv::t0, 0);
            pb->push(rv2);
        } else {
            panic("unexpected");
        }
    }
}

static inline size_t grob(spack &spk, virt_reg *vr) {
    size_t saver = riscv::zero;
    if (vr->type() == VREG_TYPE::INT or vr->type() == VREG_TYPE::PTR) {
        if (spk.t1 == false) {
            spk.t1 = true;
            saver = riscv::t1;
        } else if (spk.t2 == false) {
            spk.t2 = true;
            saver = riscv::t2;
        }
    } else if (vr->type() == VREG_TYPE::FLT) {
        if (spk.t1 == false) {
            spk.t1 = true;
            saver = riscv::fs0;
        } else if (spk.t2 == false) {
            spk.t2 = true;
            saver = riscv::fs1;
        } else if (spk.s2 == false) {
            spk.s2 = true;
            saver = riscv::fs2;
        }
    }
    return saver;
}

size_t virt_reg::load(pblock *pb, spack &spk, size_t to) {
    switch (kind_) {
        case VREG_KIND::ZERO:
            return riscv::zero;

        case VREG_KIND::IMM: {
            if (value_ == 0) {
                return riscv::zero;
            }
            auto rv = new rv_li(riscv::t0, value_);
            pb->push(rv);
            return riscv::t0;
        }

        case VREG_KIND::REG: {
            if (not confirm_) {
                return riscv::zero;
            }
            if (onstk_) {
                if (to) {
                    popout(pb, this, to);
                    return to;
                }
                auto saver = grob(spk, this);
                popout(pb, this, saver);
                return saver;
            }
            return rregid_;
        }

        case VREG_KIND::STK:
            panic("do not use on stk type");

        case VREG_KIND::LOC: {
            if (to) {
                auto rv0 = new rv_flw_l(to, gen_loc_cst_label(value_).c_str(), riscv::t2);
                pb->push(rv0);
                return to;
            }
            auto saver = grob(spk, this);
            auto rv0 = new rv_flw_l(saver, gen_loc_cst_label(value_).c_str(), riscv::t2);
            pb->push(rv0);
            return saver;
        }

        case VREG_KIND::PRM: {
            size_t saver;
            if (to) {
                saver = to;
            } else {
                saver = grob(spk, this);
            }

            if (type_ == VREG_TYPE::FLT) {
                if (rregid_  - riscv::fa0 < 8) {
                    return rregid_;
                }
                auto rv = new rv_flw(saver, riscv::fp, pstk_ * 8);
                pb->push(rv);
                return saver;
            } else {
                if (rregid_  - riscv::a0 < 8) {
                    return rregid_;
                }
                auto rv = new rv_ld(saver, riscv::fp, pstk_ * 8);
                pb->push(rv);
                return saver;
            }
        }

        case VREG_KIND::SPEC: {
            return value_;
        }
    }
    return riscv::zero;
}

size_t virt_reg::store_where() {
    switch (kind_) {
        case VREG_KIND::ZERO:
            panic("do not use on zero type");

        case VREG_KIND::IMM:
            panic("do not use on imm type");

        case VREG_KIND::STK:
            panic("do not use on stk type");

        case VREG_KIND::LOC:
            panic("do not use on loc type");

        case VREG_KIND::REG: {
            if (not confirm_) {
                return riscv::zero;
            }
            if (onstk_) {
                if (type_ == VREG_TYPE::FLT) {
                    return riscv::fs0;
                } else {
                    return riscv::t1;
                }
            }
            return rregid_;
        }

        case VREG_KIND::PRM: {
            if (type_ == VREG_TYPE::FLT) {
                if (rregid_  - riscv::fa0 < 8) {
                    return rregid_;
                }
                return riscv::fs0;
            } else {
                if (rregid_  - riscv::a0 < 8) {
                    return rregid_;
                }
                return riscv::t2;
            }
        }

        case VREG_KIND::SPEC: {
            return value_;
        }
    }
    return riscv::zero;
}

void virt_reg::store(pblock *pb, size_t to) {
    switch (kind_) {
        case VREG_KIND::ZERO:
        case VREG_KIND::IMM:
        case VREG_KIND::STK:
        case VREG_KIND::LOC:
            panic("unexpected");

        case VREG_KIND::REG: {
            if (not confirm_) {
                return;
            }
            if (onstk_) {
                if (to) {
                    pushin(pb, this, to);
                } else if (type_ == VREG_TYPE::FLT) {
                    pushin(pb, this, riscv::fs0);
                } else {
                    pushin(pb, this, riscv::t1);
                }
            }
        } break;

        case VREG_KIND::PRM: {
            if (type_ == VREG_TYPE::FLT) {
                if (rregid_ - riscv::fa0 < 8) {
                    return;
                }
                rv_fsw *rv;
                if (to) {
                    rv = new rv_fsw(to, riscv::fp, pstk_ * 8);
                } else {
                    rv = new rv_fsw(riscv::fs0, riscv::fp, pstk_ * 8);
                }
                pb->push(rv);
            } else {
                if (rregid_ - riscv::a0 < 8) {
                    return;
                }
                rv_sd *rv;
                if (to) {
                    rv = new rv_sd(to, riscv::fp, pstk_ * 8);
                } else {
                    rv = new rv_sd(riscv::t1, riscv::fp, pstk_ * 8);
                }
                pb->push(rv);
            }
        } break;

        default:
            break;
    }
}