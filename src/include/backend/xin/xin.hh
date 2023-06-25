#pragma once

#include "backend/asm/riscv/asm.hh"
#include "backend/asm/riscv/def.hh"
#include "backend/ir/bbtype.hh"
#include "backend/utils.hh"
#include "debug/Logs.hh"
#include "ir/IR.hh"

class progress;

// cross internal manager
class cross_internal_manager {
    NormalFuncPtr &fptr_;

    std::unordered_set<uint32_t> &lc_pool_;

    rl_progress rl_pgrs_;

    void nftoir();

    void irpass();

    void rassign();

    void irtorv();

    void pir();

   public:
    cross_internal_manager(NormalFuncPtr &fptr, std::unordered_set<uint32_t> &lc_pool);

    void operator()();

    std::unique_ptr<progress> apg_;
};