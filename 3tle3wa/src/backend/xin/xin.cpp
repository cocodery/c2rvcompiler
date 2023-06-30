#include "3tle3wa/backend/xin/xin.hh"

#include "3tle3wa/backend/ir/uop/uop.hh"

class glb_value;

cross_internal_manager::cross_internal_manager(NormalFuncPtr &fptr, std::unordered_set<uint32_t> &lc_pool,
                                               const std::unordered_map<size_t, glb_value *> &gname_map)
    : fptr_(fptr), lc_pool_(lc_pool), gname_map_(gname_map), rl_pgrs_(), apg_(nullptr) {
    rl_pgrs_.label_ = fptr_->GetFuncName();
}

void cross_internal_manager::do_compile() {
    nftoir();
    irpass();
    rassign();
    irtorv();
}