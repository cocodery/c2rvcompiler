#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/ir/virt/register.hh"

void uop_set_iparam::trace_inst() { rs_->add_ref(this); }

void uop_set_fparam::trace_inst() { rs_->add_ref(this); }

void uop_call::trace_inst() {
    if (retval_) retval_->set_from(this);
}

void uop_li::trace_inst() { rd_->set_from(this); }

void uop_mv::trace_inst() {
    rd_->set_from(this);
    rs_->add_ref(this);
}

void uop_fmv::trace_inst() {
    rd_->set_from(this);
    rs_->add_ref(this);
}

void uop_cvts2w::trace_inst() {
    rd_->set_from(this);
    rs_->add_ref(this);
}

void uop_cvtw2s::trace_inst() {
    rd_->set_from(this);
    rs_->add_ref(this);
}

void uop_b::trace_inst() { cond_->add_ref(this); }

void uop_icmp_b::trace_inst() {
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_la::trace_inst() { rd_->set_from(this); }

void uop_lla::trace_inst() { rd_->set_from(this); }

void uop_ld::trace_inst() {
    rd_->set_from(this);
    rb_->add_ref(this);
}

void uop_st::trace_inst() {
    rd_->add_ref(this);
    rb_->add_ref(this);
}

void uop_ld_l::trace_inst() { rd_->set_from(this); }

void uop_st_l::trace_inst() { rd_->add_ref(this); }

void uop_fld::trace_inst() {
    rd_->set_from(this);
    rb_->add_ref(this);
}

void uop_fst::trace_inst() {
    rd_->add_ref(this);
    rb_->add_ref(this);
}

void uop_fld_l::trace_inst() { rd_->set_from(this); }

void uop_fst_l::trace_inst() { rd_->add_ref(this); }

void uop_fld_ll::trace_inst() { rd_->set_from(this); }

void uop_icmp::trace_inst() {
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_fcmp::trace_inst() {
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_bin::trace_inst() {
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_fbin::trace_inst() {
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
}

void uop_ftri::trace_inst() {
    rd_->set_from(this);
    lhs_->add_ref(this);
    rhs_->add_ref(this);
    ahs_->add_ref(this);
}