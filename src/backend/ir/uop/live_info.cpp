#include "uop.hh"

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