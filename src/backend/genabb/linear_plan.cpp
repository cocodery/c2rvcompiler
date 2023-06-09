#include <unordered_set>

#include "genabb.hh"

struct liveinfo {
    std::unordered_set<size_t> UEVar;
    std::unordered_set<size_t> VarKill;
    std::unordered_set<size_t> liveout;
};

struct opinfo {
    bool is_retvalue;
    bool is_retrecver;

    size_t param_idx;
};

using vliveinfo = std::unordered_map<size_t, liveinfo>;

auto isImm(const BaseValuePtr &vptr) -> bool {
    return vptr == nullptr ? true : vptr->IsConstant() && !vptr->GetBaseType()->IsArray();
}

auto lv_init(const CfgNodePtr &cfgnptr) -> liveinfo {
    liveinfo lvif;
    auto &&UEVar = lvif.UEVar;
    auto &&VarKill = lvif.VarKill;

    auto &&insts = cfgnptr->GetInstList();
    for (auto &&inst : insts) {
        auto &&rhss = inst->GetOprands();
        auto &&lhs = inst->GetResult();

        for (auto &&rhs : rhss) {
            if (isImm(rhs)) continue;

            if (VarKill.count(rhs->guidx()) == 0) {
                UEVar.insert(rhs->guidx());
            }
        }

        if (!isImm(lhs)) {
            VarKill.insert(lhs->guidx());
        }
    }
    return lvif;
}

auto recompute(vliveinfo &glvif, const CfgNodePtr &curcfgnd) -> bool {
    auto &&succs = curcfgnd->GetSuccessors();
    auto curidx = curcfgnd->GetBlockIdx();
    auto &&curlvif = glvif[curidx];

    std::unordered_set<size_t> result = curlvif.liveout;
    for (auto &&succ : succs) {
        auto idx = succ->GetBlockIdx();
        auto &&lvif = glvif[idx];
        std::vector<size_t> diff;
        std::set_difference(lvif.liveout.begin(), lvif.liveout.end(), lvif.VarKill.begin(), lvif.VarKill.end(),
                            std::back_insert_iterator(diff));
        for (auto &&v : diff) {
            result.insert(v);
        }
        for (auto &&v : lvif.UEVar) {
            result.insert(v);
        }
    }
    for (auto &&v : result) {
        if (curlvif.liveout.find(v) == curlvif.liveout.end()) {
            curlvif.liveout = result;
            return true;
        }
    }
    return false;
}

allocplan linear_plan(NormalFuncPtr &func) {
    allocplan plan;
    auto &&topo = func->TopoSortFromEntry();

    // live info calculation ref EaC

    vliveinfo liveness;

    // init live info
    for (auto &&blk : topo) {
        auto idx = blk->GetBlockIdx();
        liveness[idx] = lv_init(blk);
    }

    bool changed = true;

    // calculate live info
    while (changed) {
        changed = false;
        for (auto &&blk : topo) {
            // dangerous
            changed = recompute(liveness, blk) || changed;
        }
    }

    char *buf;
    size_t buflen;
    FILE *fp = open_memstream(&buf, &buflen);
    for (auto &&info : liveness) {
        fprintf(fp, "%%Block_%04lu: ", info.first);
        auto iter = info.second.liveout.begin();
        if (iter == info.second.liveout.end()) {
            fprintf(fp, "\n");
            continue;
        }
        fprintf(fp, "%%R_%04lu", *iter);
        while (++iter != info.second.liveout.end()) {
            fprintf(fp, ", %%R_%04lu", *iter);
        }
        fprintf(fp, "\n");
    }
    fflush(fp);
    fclose(fp);
    Log("\n%s", buf);
    free(buf);
    buflen = 0;

    // linear scan

    return plan;
}