#pragma once

#include <vector>

#include "3tle3wa/backend/ir/virt/register.hh"

class uop_general;

struct detailed_live_info {
    std::unordered_set<size_t> live_out;
    std::unordered_set<size_t> ue_var;
    std::unordered_set<size_t> var_kill;
};

class rl_basicblock {
    size_t lbid_{};

   public:
    void set_lbid(size_t inp) { lbid_ = inp; }
    size_t &get_lbid() { return lbid_; }

    detailed_live_info dli;

    std::unordered_set<size_t> successer;

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