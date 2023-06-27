#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/ir/virt/register.hh"

void uop_ret::trace_inst(size_t fa_idx) { fa_idx_ = fa_idx; }

void uop_set_iparam::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rs_->add_ref(this);
}

void uop_set_fparam::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rs_->add_ref(this);
}

void uop_call::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    if (retval_) retval_->set_from(this);
}

void uop_li::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
}

void uop_mv::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    rs_->add_ref(this);
}

void uop_fmv::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    rs_->add_ref(this);
}

void uop_cvts2w::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    rs_->add_ref(this);
}

void uop_cvtw2s::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    rs_->add_ref(this);
}

void uop_b::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    cond_->add_ref(this);
}

void uop_icmp_b::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_j::trace_inst(size_t fa_idx) { fa_idx_ = fa_idx; }

void uop_la::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
}

void uop_lla::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
}

void uop_ld::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    rb_->add_ref(this);
}

void uop_st::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->add_ref(this);
    rb_->add_ref(this);
}

void uop_ld_l::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
}

void uop_st_l::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->add_ref(this);
}

void uop_fld::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    rb_->add_ref(this);
}

void uop_fst::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->add_ref(this);
    rb_->add_ref(this);
}

void uop_fld_l::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
}

void uop_fst_l::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->add_ref(this);
}

void uop_fld_ll::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
}

void uop_icmp::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_fcmp::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_bin::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_fbin::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_ftri::trace_inst(size_t fa_idx) {
    fa_idx_ = fa_idx;
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
    ahs_->add_ref(this);
}