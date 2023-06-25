#include "backend/ir/uop/uop.hh"
#include "backend/xin/xin.hh"

void cross_internal_manager::irpass() {
    for (auto &&lrbb : rl_pgrs_.bbs_) {
        for (auto &&uop : lrbb->ops_) {
            uop->trace_call();
            uop->trace_inst();
        }
    }
}