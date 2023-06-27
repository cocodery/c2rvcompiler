#pragma once

#include <mutex>

#include "3tle3wa/backend/asm/riscv/asm.hh"
#include "3tle3wa/backend/asm/riscv/def.hh"
#include "3tle3wa/backend/ir/bbtype.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/utils/Logs.hh"

class progress;

// cross internal manager
class cross_internal_manager {
    NormalFuncPtr &fptr_;

    std::unordered_set<uint32_t> &lc_pool_;

    rl_progress rl_pgrs_;

    void nftoir();

    void rassign();

    void irtorv();

    void pir();

    // backend ir pass

    void irpass();

    void irpass_delete_single_jump();

    void irpass_combine_fallthrough();
    
    void irpass_gen_cmpb();
    
    void irpass_gen_fmas();

   public:
    cross_internal_manager(NormalFuncPtr &fptr, std::unordered_set<uint32_t> &lc_pool);

    void do_compile();

    std::unique_ptr<progress> apg_;
};