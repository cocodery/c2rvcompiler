#include "rassign.hh"

#include "Logs.hh"

void cross_internal_manager::rassign() {
    rl_pgrs_.valc_.plan_reg();
    rl_pgrs_.valc_.plan_stack();
}