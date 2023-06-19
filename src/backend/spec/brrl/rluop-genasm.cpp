#include "../riscv/riscv-asm.hh"
#include "../riscv/riscv-cst.hh"
#include "rluop.hh"

void uop_ret::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto inst = std::make_unique<RV_RET>();
    lst->asminsts.push_back(std::move(inst));
}

void uop_set_param::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (onflt) {
        auto inst = std::make_unique<RV_FMV_S>(riscv::fa0 + idx, src->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else {
        auto inst = std::make_unique<RV_MV>(riscv::a0 + idx, src->inrreg);
        lst->asminsts.push_back(std::move(inst));
    }
}

void uop_call::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto inst = std::make_unique<RV_CALL$>(callee.c_str());
    lst->asminsts.push_back(std::move(inst));
}

void uop_li::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto inst = std::make_unique<RV_LI>(riscv::t0, imm);
    lst->asminsts.push_back(std::move(inst));
}

void uop_mv::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (src->kind == VREG_KIND::STK) {
        auto stkinfo = (virt_stkinf *)src->value;
        auto inst = std::make_unique<RV_ADDI>(dst->inrreg, riscv::fp, stkinfo->off);
        lst->asminsts.push_back(std::move(inst));
    } else {
        auto inst = std::make_unique<RV_MV>(dst->inrreg, src->inrreg);
        lst->asminsts.push_back(std::move(inst));
    }
}

void uop_cvts2w::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto inst = std::make_unique<RV_FCVT_W_S>(dst->inrreg, src->inrreg);
    lst->asminsts.push_back(std::move(inst));
}

void uop_cvtw2s::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto inst = std::make_unique<RV_FCVT_S_W>(dst->inrreg, src->inrreg);
    lst->asminsts.push_back(std::move(inst));
}

void uop_b::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto &&target = pfx + std::to_string(lbid);
    auto inst = std::make_unique<RV_BEQZ$>(cond->inrreg, target.c_str());
    lst->asminsts.push_back(std::move(inst));
}

void uop_cmp_b::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto &&target = pfx + std::to_string(lbid);
    if (kind == COMP_KIND::EQU) {
        auto inst = std::make_unique<RV_BEQ$>(lhs->inrreg, rhs->inrreg, target.c_str());
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == COMP_KIND::NEQ) {
        auto inst = std::make_unique<RV_BNE$>(lhs->inrreg, rhs->inrreg, target.c_str());
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == COMP_KIND::GEQ) {
        auto inst = std::make_unique<RV_BGE$>(lhs->inrreg, rhs->inrreg, target.c_str());
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == COMP_KIND::LTH) {
        auto inst = std::make_unique<RV_BLT$>(lhs->inrreg, rhs->inrreg, target.c_str());
        lst->asminsts.push_back(std::move(inst));
    } else {
        panic("unexpected");
    }
}

void uop_j::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto &&target = pfx + std::to_string(lbid);
    auto inst = std::make_unique<RV_J$>(target.c_str());
    lst->asminsts.push_back(std::move(inst));
}

void uop_la::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    auto inst = std::make_unique<RV_LA$>(dst->inrreg, src.c_str());
    lst->asminsts.push_back(std::move(inst));
}

void uop_load::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (dst->len == 4) {
        if (onflt) {
            auto inst = std::make_unique<RV_FLW>(dst->inrreg, base->inrreg, off);
            lst->asminsts.push_back(std::move(inst));
        } else {
            auto inst = std::make_unique<RV_LW>(dst->inrreg, base->inrreg, off);
            lst->asminsts.push_back(std::move(inst));
        }
    } else if (dst->len == 8 || dst->len == 0) {
        auto inst = std::make_unique<RV_LD>(dst->inrreg, base->inrreg, off);
        lst->asminsts.push_back(std::move(inst));
    } else {
        panic("unexpected");
    }
}

void uop_load_tag::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (onflt) {
        auto inst = std::make_unique<RV_FLW$>(dst->inrreg, src.c_str(), riscv::t2);
        lst->asminsts.push_back(std::move(inst));
    } else {
        auto inst = std::make_unique<RV_LW$>(dst->inrreg, src.c_str());
        lst->asminsts.push_back(std::move(inst));
    }
}

void uop_store::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (src->len == 4) {
        if (onflt) {
            auto inst = std::make_unique<RV_FSW>(src->inrreg, base->inrreg, off);
            lst->asminsts.push_back(std::move(inst));
        } else {
            auto inst = std::make_unique<RV_SW>(src->inrreg, base->inrreg, off);
            lst->asminsts.push_back(std::move(inst));
        }
    } else if (src->len == 8 || src->len == 0) {
        auto inst = std::make_unique<RV_SD>(src->inrreg, base->inrreg, off);
        lst->asminsts.push_back(std::move(inst));
    } else {
        panic("unexpected");
    }
}

void uop_store_tag::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (onflt) {
        auto inst = std::make_unique<RV_FSW$>(src->inrreg, dst.c_str(), riscv::t2);
        lst->asminsts.push_back(std::move(inst));
    } else {
        auto inst = std::make_unique<RV_SW$>(src->inrreg, dst.c_str(), riscv::t2);
        lst->asminsts.push_back(std::move(inst));
    }
}

void uop_cmp::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {}

void uop_bin::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (kind == IBIN_KIND::ADD) {
        if (rhs->kind == VREG_KIND::IMM) {
            if (dst->len == 4) {
                auto inst = std::make_unique<RV_ADDIW>(dst->inrreg, lhs->inrreg, rhs->value);
                lst->asminsts.push_back(std::move(inst));
            } else {
                auto inst = std::make_unique<RV_ADDI>(dst->inrreg, lhs->inrreg, rhs->value);
                lst->asminsts.push_back(std::move(inst));
            }
        } else {
            if (dst->len == 4) {
                auto inst = std::make_unique<RV_ADDW>(dst->inrreg, lhs->inrreg, rhs->inrreg);
                lst->asminsts.push_back(std::move(inst));
            } else {
                auto inst = std::make_unique<RV_ADD>(dst->inrreg, lhs->inrreg, rhs->inrreg);
                lst->asminsts.push_back(std::move(inst));
            }
        }
    } else if (kind == IBIN_KIND::SUB) {
        if (rhs->kind == VREG_KIND::IMM) {
            if (dst->len == 4) {
                auto inst = std::make_unique<RV_ADDIW>(dst->inrreg, lhs->inrreg, -(int64_t)(rhs->value));
                lst->asminsts.push_back(std::move(inst));
            } else {
                auto inst = std::make_unique<RV_ADDI>(dst->inrreg, lhs->inrreg, -(int64_t)(rhs->value));
                lst->asminsts.push_back(std::move(inst));
            }
        } else {
            if (dst->len == 4) {
                auto inst = std::make_unique<RV_SUBW>(dst->inrreg, lhs->inrreg, rhs->inrreg);
                lst->asminsts.push_back(std::move(inst));
            } else {
                auto inst = std::make_unique<RV_SUB>(dst->inrreg, lhs->inrreg, rhs->inrreg);
                lst->asminsts.push_back(std::move(inst));
            }
        }
    } else if (kind == IBIN_KIND::MUL) {
        auto inst = std::make_unique<RV_MULW>(dst->inrreg, lhs->inrreg, rhs->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == IBIN_KIND::DIV) {
        auto inst = std::make_unique<RV_DIVW>(dst->inrreg, lhs->inrreg, rhs->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == IBIN_KIND::REM) {
        auto inst = std::make_unique<RV_REMW>(dst->inrreg, lhs->inrreg, rhs->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == IBIN_KIND::SLL) {
        if (rhs->kind == VREG_KIND::IMM) {
            if (dst->len == 4) {
                auto inst = std::make_unique<RV_SLLIW>(dst->inrreg, lhs->inrreg, rhs->value);
                lst->asminsts.push_back(std::move(inst));
            } else {
                auto inst = std::make_unique<RV_SLLI>(dst->inrreg, lhs->inrreg, rhs->value);
                lst->asminsts.push_back(std::move(inst));
            }
        } else {
            auto inst = std::make_unique<RV_SLLW>(dst->inrreg, lhs->inrreg, rhs->inrreg);
            lst->asminsts.push_back(std::move(inst));
        }
    } else if (kind == IBIN_KIND::SRA) {
        if (rhs->kind == VREG_KIND::IMM) {
            if (dst->len == 4) {
                auto inst = std::make_unique<RV_SRAIW>(dst->inrreg, lhs->inrreg, rhs->value);
                lst->asminsts.push_back(std::move(inst));
            } else {
                auto inst = std::make_unique<RV_SRAI>(dst->inrreg, lhs->inrreg, rhs->value);
                lst->asminsts.push_back(std::move(inst));
            }
        } else {
            auto inst = std::make_unique<RV_SRAW>(dst->inrreg, lhs->inrreg, rhs->inrreg);
            lst->asminsts.push_back(std::move(inst));
        }
    } else {
        panic("unexpected");
    }
}

void uop_fbin::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (kind == FBIN_KIND::ADD) {
        auto inst = std::make_unique<RV_FADD_S>(dst->inrreg, lhs->inrreg, rhs->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == FBIN_KIND::SUB) {
        auto inst = std::make_unique<RV_FSUB_S>(dst->inrreg, lhs->inrreg, rhs->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == FBIN_KIND::MUL) {
        auto inst = std::make_unique<RV_FMUL_S>(dst->inrreg, lhs->inrreg, rhs->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == FBIN_KIND::DIV) {
        auto inst = std::make_unique<RV_FDIV_S>(dst->inrreg, lhs->inrreg, rhs->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else {
        panic("unexpected");
    }
}

void uop_ftri::gen_asm(std::unique_ptr<ABBProg::ABBlock> &lst) {
    if (kind == FTRI_KIND::MADD) {
        auto inst = std::make_unique<RV_FMADD_S>(dst->inrreg, op1->inrreg, op2->inrreg, op3->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == FTRI_KIND::MSUB) {
        auto inst = std::make_unique<RV_FMSUB_S>(dst->inrreg, op1->inrreg, op2->inrreg, op3->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == FTRI_KIND::NMADD) {
        auto inst = std::make_unique<RV_FNMADD_S>(dst->inrreg, op1->inrreg, op2->inrreg, op3->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else if (kind == FTRI_KIND::NMSUB) {
        auto inst = std::make_unique<RV_FNMSUB_S>(dst->inrreg, op1->inrreg, op2->inrreg, op3->inrreg);
        lst->asminsts.push_back(std::move(inst));
    } else {
        panic("unexpected");
    }
}