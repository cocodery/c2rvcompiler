#pragma once

#include "../asm/progress.hh"
#include "../ir/bbtype.hh"
#include "IR.hh"

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