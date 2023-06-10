#pragma once

#include <queue>
#include <unordered_set>
#include <unordered_map>

#include "../genabb.hh"

struct reindex_inst {
    enum { LABEL, INST } type;
    InstPtr instptr;
    size_t label_idx = -1;
};

struct livespan {
    size_t regidx = 0;
    size_t begin = 0;
    size_t end = 0;

    bool undefined = true;
    bool is_phisrc = false;
    size_t phioff = 0;
};

constexpr size_t PHI_TAG = (size_t)-1;

class LinearScan {
    // reindex label so that inst reindex can find the label original pos
    std::unordered_map<size_t, uint64_t> label_reindex;

    // reindex instructions for true live interval
    std::vector<reindex_inst> inst_reindex;

    // record variables live interval
    std::unordered_map<size_t, livespan> live_interval;
    
    // target function
    NormalFuncPtr &func;

    // global values
    NameValueMap &nvmap;

    void reindex();

    void generate_intervals();

    void generate_asm(ABBProg &prog);

   public:
    LinearScan(NormalFuncPtr &xfunc, NameValueMap &xnvmap);

    void plan(ABBProg *prog);
};