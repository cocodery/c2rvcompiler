#include "uop.hh"

void uop_set_iparam::trace_call() { rs_->set_param_pos(idx_); }

void uop_set_fparam::trace_call() { rs_->set_param_pos(idx_); }

void uop_call::trace_call() {
    if (retval_) retval_->set_is_retval(true);
}