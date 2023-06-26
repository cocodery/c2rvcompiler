#include "3tle3wa/backend/xin/rassign.hh"

void cross_internal_manager::rassign() {
    rl_pgrs_.valc_.plan_reg(rl_pgrs_);
    rl_pgrs_.valc_.plan_stack();
}