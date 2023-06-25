#include "backend/ir/bbtype.hh"
#include "backend/ir/uop/uop.hh"
#include "backend/ir/virt/register.hh"

void uop_set_iparam::live_info() { rs_->set_end(uop_idx_); }

void uop_set_fparam::live_info() { rs_->set_end(uop_idx_); }

void uop_call::live_info() {
    if (retval_) retval_->set_begin(uop_idx_);
}

void uop_li::live_info() { rd_->set_begin(uop_idx_); }

void uop_mv::live_info() {
    rd_->set_begin(uop_idx_);
    rs_->set_end(uop_idx_);
}

void uop_fmv::live_info() {
    rd_->set_begin(uop_idx_);
    rs_->set_end(uop_idx_);
}

void uop_cvts2w::live_info() {
    rd_->set_begin(uop_idx_);
    rs_->set_end(uop_idx_);
}

void uop_cvtw2s::live_info() {
    rd_->set_begin(uop_idx_);
    rs_->set_end(uop_idx_);
}

void uop_b::live_info() { cond_->set_end(uop_idx_); }

void uop_icmp_b::live_info() {
    lhs_->set_end(uop_idx_);
    rhs_->set_end(uop_idx_);
}

void uop_la::live_info() { rd_->set_begin(uop_idx_); }

void uop_lla::live_info() { rd_->set_begin(uop_idx_); }

void uop_ld::live_info() {
    rd_->set_begin(uop_idx_);
    rb_->set_end(uop_idx_);
}

void uop_st::live_info() {
    rd_->set_end(uop_idx_);
    rb_->set_end(uop_idx_);
}

void uop_ld_l::live_info() { rd_->set_begin(uop_idx_); }

void uop_st_l::live_info() { rd_->set_end(uop_idx_); }

void uop_fld::live_info() {
    rd_->set_begin(uop_idx_);
    rb_->set_end(uop_idx_);
}

void uop_fst::live_info() {
    rd_->set_end(uop_idx_);
    rb_->set_end(uop_idx_);
}

void uop_fld_l::live_info() { rd_->set_begin(uop_idx_); }

void uop_fst_l::live_info() { rd_->set_end(uop_idx_); }

void uop_fld_ll::live_info() { rd_->set_begin(uop_idx_); }

void uop_icmp::live_info() {
    rd_->set_begin(uop_idx_);
    lhs_->set_end(uop_idx_);
    rhs_->set_end(uop_idx_);
}

void uop_fcmp::live_info() {
    rd_->set_begin(uop_idx_);
    lhs_->set_end(uop_idx_);
    rhs_->set_end(uop_idx_);
}

void uop_bin::live_info() {
    rd_->set_begin(uop_idx_);
    lhs_->set_end(uop_idx_);
    rhs_->set_end(uop_idx_);
}

void uop_fbin::live_info() {
    rd_->set_begin(uop_idx_);
    lhs_->set_end(uop_idx_);
    rhs_->set_end(uop_idx_);
}

void uop_ftri::live_info() {
    rd_->set_begin(uop_idx_);
    lhs_->set_end(uop_idx_);
    rhs_->set_end(uop_idx_);
    ahs_->set_end(uop_idx_);
}

void uop_set_iparam::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rs_->kind() == VREG_KIND::REG) {
        auto idx = rs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_set_fparam::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rs_->kind() == VREG_KIND::REG) {
        auto idx = rs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_call::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    if (retval_ and retval_->kind() == VREG_KIND::REG) {
        auto idx = retval_->value();
        VarKill.insert(idx);
    }
}

void uop_li::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
}

void uop_mv::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (rs_->kind() == VREG_KIND::REG) {
        auto idx = rs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_fmv::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (rs_->kind() == VREG_KIND::REG) {
        auto idx = rs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_cvts2w::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (rs_->kind() == VREG_KIND::REG) {
        auto idx = rs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_cvtw2s::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (rs_->kind() == VREG_KIND::REG) {
        auto idx = rs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_b::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (cond_->kind() == VREG_KIND::REG) {
        auto idx = cond_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_icmp_b::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (lhs_->kind() == VREG_KIND::REG) {
        auto idx = lhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (rhs_->kind() == VREG_KIND::REG) {
        auto idx = rhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_la::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
}

void uop_lla::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
}

void uop_ld::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (rb_->kind() == VREG_KIND::REG) {
        auto idx = rb_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_st::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (rb_->kind() == VREG_KIND::REG) {
        auto idx = rb_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_ld_l::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
}

void uop_st_l::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_fld::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (rb_->kind() == VREG_KIND::REG) {
        auto idx = rb_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_fst::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (rb_->kind() == VREG_KIND::REG) {
        auto idx = rb_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_fld_l::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
}

void uop_fst_l::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_fld_ll::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
}

void uop_icmp::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (lhs_->kind() == VREG_KIND::REG) {
        auto idx = lhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (rhs_->kind() == VREG_KIND::REG) {
        auto idx = rhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_fcmp::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (lhs_->kind() == VREG_KIND::REG) {
        auto idx = lhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (rhs_->kind() == VREG_KIND::REG) {
        auto idx = rhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_bin::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (lhs_->kind() == VREG_KIND::REG) {
        auto idx = lhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (rhs_->kind() == VREG_KIND::REG) {
        auto idx = rhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_fbin::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (lhs_->kind() == VREG_KIND::REG) {
        auto idx = lhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (rhs_->kind() == VREG_KIND::REG) {
        auto idx = rhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}

void uop_ftri::calcu_lvif(detailed_live_info &dli) {
    auto &&VarKill = dli.var_kill;
    auto &&UEVar = dli.ue_var;
    if (rd_->kind() == VREG_KIND::REG) {
        auto idx = rd_->value();
        VarKill.insert(idx);
    }
    if (lhs_->kind() == VREG_KIND::REG) {
        auto idx = lhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (rhs_->kind() == VREG_KIND::REG) {
        auto idx = rhs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
    if (ahs_->kind() == VREG_KIND::REG) {
        auto idx = ahs_->value();
        if (VarKill.find(idx) == VarKill.end()) {
            UEVar.insert(idx);
        }
    }
}