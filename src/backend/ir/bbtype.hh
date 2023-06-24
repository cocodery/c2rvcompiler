#pragma once

#include <vector>

#include "uop/uop.hh"
#include "virt/register.hh"

class rl_basicblock {
    GETTER_SETTER(lbid, size_t);

   public:
    detailed_live_info dli;

    std::unordered_set<size_t> successer;

    std::unordered_set<size_t> dominator;

    std::list<std::unique_ptr<uop_general>> ops_;
};

class rl_progress {
   public:
    vr_allocor valc_;

    std::string_view label_;
    std::vector<virt_reg *> params_;

    struct rlbb_rcd {
        rl_basicblock *bbp_ = nullptr;
        std::list<uop_general *> refs_;
    };
    std::unordered_map<size_t, rlbb_rcd> lbmap_;

    std::list<std::unique_ptr<rl_basicblock>> bbs_;
};