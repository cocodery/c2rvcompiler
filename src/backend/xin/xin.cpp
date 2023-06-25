#include "backend/xin/xin.hh"

#include "backend/ir/uop/uop.hh"

cross_internal_manager::cross_internal_manager(NormalFuncPtr &fptr, std::unordered_set<uint32_t> &lc_pool)
    : fptr_(fptr), lc_pool_(lc_pool), rl_pgrs_(), apg_(nullptr) {
    rl_pgrs_.label_ = fptr_->GetFuncName();
}

void cross_internal_manager::operator()() {
    nftoir();
    irpass();
    rassign();
    irtorv();
}