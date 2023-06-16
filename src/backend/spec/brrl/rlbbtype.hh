#pragma once

#include "rluop.hh"
#include "rlvtype.hh"

#include <vector>

struct rl_namevalue {
    bool isGlb;
    std::string tag;
    size_t value;
};

struct rl_basicblock {
    std::string tag;
    size_t lbid;

    std::list<std::shared_ptr<uop_general>> ops;
};

struct rl_progress {
    virt_reg_allocor vreg_alloc;
    virt_stkinf_allocor vski_alloc;

    std::string pnm;
    std::vector<virt_reg *> params;

    struct rlbb_rcd {
        rl_basicblock *bbp = nullptr;
        std::list<uop_general *> refs;
    };
    std::unordered_map<size_t, rlbb_rcd> lbmap;

    std::list<std::shared_ptr<rl_basicblock>> bbs;
};