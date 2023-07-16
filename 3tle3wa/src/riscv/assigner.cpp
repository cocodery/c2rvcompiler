#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLGen.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/instruction/opCode.hh"
#include "3tle3wa/riscv/spec.hh"

size_t abi_arg_reg = 8;

// riscv::t0 for li use

// if call requently

size_t i_gpr_callee_first[] = {
    riscv::s1, riscv::s2,  riscv::s3,  riscv::s4, riscv::s5, riscv::s6, riscv::s7, riscv::s8,
    riscv::s9, riscv::s10, riscv::s11, riscv::t2, riscv::t3, riscv::t4, riscv::t5, riscv::t6,
    riscv::a7, riscv::a6,  riscv::a5,  riscv::a4, riscv::a3, riscv::a2, riscv::a1, riscv::a0,
};

constexpr size_t i_gpr_callee_first_len = sizeof(i_gpr_callee_first) / sizeof(*i_gpr_callee_first);

size_t f_gpr_callee_first[] = {
    riscv::fs0, riscv::fs1, riscv::fs2,  riscv::fs3,  riscv::fs4,  riscv::fs5,  riscv::fs6, riscv::fs7,
    riscv::fs8, riscv::fs9, riscv::fs10, riscv::fs11, riscv::ft2,  riscv::ft3,  riscv::ft4, riscv::ft5,
    riscv::ft6, riscv::ft7, riscv::ft8,  riscv::ft9,  riscv::ft10, riscv::ft11, riscv::fa7, riscv::fa6,
    riscv::fa5, riscv::fa4, riscv::fa3,  riscv::fa2,  riscv::fa1,  riscv::fa0,
};

constexpr size_t f_gpr_callee_first_len = sizeof(f_gpr_callee_first) / sizeof(*f_gpr_callee_first);

// if no much call

size_t i_gpr_caller_first[] = {riscv::t2, riscv::t3, riscv::t4, riscv::t5, riscv::t6,  riscv::a7,
                               riscv::a6, riscv::a5, riscv::a4, riscv::a3, riscv::a2,  riscv::a1,
                               riscv::a0, riscv::s1, riscv::s2, riscv::s3, riscv::s4,  riscv::s5,
                               riscv::s6, riscv::s7, riscv::s8, riscv::s9, riscv::s10, riscv::s11};

constexpr size_t i_gpr_caller_first_len = sizeof(i_gpr_caller_first) / sizeof(*i_gpr_caller_first);

size_t f_gpr_caller_first[] = {
    riscv::ft2,  riscv::ft3,  riscv::ft4, riscv::ft5, riscv::ft6,  riscv::ft7,  riscv::ft8, riscv::ft9,
    riscv::ft10, riscv::ft11, riscv::fa7, riscv::fa6, riscv::fa5,  riscv::fa4,  riscv::fa3, riscv::fa2,
    riscv::fa1,  riscv::fa0,  riscv::fs0, riscv::fs1, riscv::fs2,  riscv::fs3,  riscv::fs4, riscv::fs5,
    riscv::fs6,  riscv::fs7,  riscv::fs8, riscv::fs9, riscv::fs10, riscv::fs11,

};

std::unordered_set<size_t> caller_save{
    riscv::a0,  riscv::a1,  riscv::a2,  riscv::a3,  riscv::a4,  riscv::a5,  riscv::a6,   riscv::a7,
    riscv::t1,  riscv::t2,  riscv::t3,  riscv::t4,  riscv::t5,  riscv::t6,  riscv::fa0,  riscv::fa1,
    riscv::fa2, riscv::fa3, riscv::fa4, riscv::fa5, riscv::fa6, riscv::fa7, riscv::ft2,  riscv::ft3,
    riscv::ft4, riscv::ft5, riscv::ft6, riscv::ft7, riscv::ft8, riscv::ft9, riscv::ft10, riscv::ft11,
};

std::unordered_set<size_t> callee_save{
    riscv::s0,  riscv::s1,  riscv::s2,  riscv::s3,  riscv::s4,  riscv::s5,  riscv::s6,   riscv::s7,
    riscv::s8,  riscv::s9,  riscv::s10, riscv::s11, riscv::fs0, riscv::fs1, riscv::fs2,  riscv::fs3,
    riscv::fs4, riscv::fs5, riscv::fs6, riscv::fs7, riscv::fs8, riscv::fs9, riscv::fs10, riscv::fs11,
};

std::unordered_set<size_t> arg_register{
    riscv::a0,  riscv::a1,  riscv::a2,  riscv::a3,  riscv::a4,  riscv::a5,  riscv::a6,  riscv::a7,
    riscv::fa0, riscv::fa1, riscv::fa2, riscv::fa3, riscv::fa4, riscv::fa5, riscv::fa6, riscv::fa7,
};

constexpr size_t f_gpr_caller_first_len = sizeof(f_gpr_caller_first) / sizeof(*f_gpr_caller_first);

void UopCall::BroadCastCall(size_t lbidx) {
    auto maxlen = std::min(abi_arg_reg, params_.size());

    for (size_t i = 0; i < maxlen; ++i) {
        params_[i]->CallAt(lbidx, i);
    }
}

void RLProgress::rvAssigner() {
    if (has_call_other_ or has_lib_call_) {
        planner_->PlanRegistersGreedy(i_gpr_callee_first, i_gpr_callee_first_len, f_gpr_callee_first, f_gpr_callee_first_len);
    } else {
        planner_->PlanRegistersGreedy(i_gpr_caller_first, i_gpr_caller_first_len, f_gpr_caller_first, f_gpr_caller_first_len);
    }

    planner_->PlanStackSpace();
}