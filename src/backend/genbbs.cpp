#include <codegen.hh>
#include <unordered_map>
#include <unordered_set>

namespace backend {

struct liveinfo {
    std::unordered_set<size_t> UEVar;
    std::unordered_set<size_t> VarKill;
    std::unordered_set<size_t> liveout;
};

using vliveinfo = std::unordered_map<size_t, liveinfo>;

namespace {

auto isImm(const BaseValuePtr &vptr) -> bool { return vptr->IsConstant() && !vptr->GetBaseType()->IsArray(); }

auto lv_init(CfgNodePtr &cfgnptr) -> liveinfo {
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
}

auto recompute(vliveinfo &glvif, CfgNodePtr &curcfgnd) -> bool {
    auto &&succs = curcfgnd->GetSuccessors();
    std::unordered_set<size_t> result;
    for (auto &&succ : succs) {
        auto idx = succ->GetBlockIdx();
        auto &&lvif = glvif[idx];
        std::unordered_set<size_t> diff;
        std::set_difference(lvif.liveout.begin(), lvif.liveout.end(), lvif.VarKill.begin(), lvif.VarKill.end(),
                            std::back_insert_iterator(diff));
        for (auto &&v : diff) {
            result.insert(v);
        }
        for (auto &&v : lvif.UEVar) {
            result.insert(v);
        }
    }
    auto curidx = curcfgnd->GetBlockIdx();
    auto &&curlvif = glvif[curidx];
    for (auto &&v : result) {
        if (curlvif.liveout.find(v) == curlvif.liveout.end()) {
            curlvif.liveout = result;
            return true;
        }
    }
    return false;
}

auto construct_interference_graph(CfgNodeList &topo, vliveinfo &liveness)
    -> std::unordered_map<size_t, std::unordered_set<size_t>> {
    std::unordered_map<size_t, std::unordered_set<size_t>> intg;

    for (auto &&blk : topo) {
        auto idx = blk->GetBlockIdx();

        // (alert) this may change liveout
        auto &&livenow = liveness[idx].liveout;
        auto &&insts = blk->GetInstList();
        for (auto &&inst : insts) {
            auto &&rhss = inst->GetOprands();
            auto &&lhs = inst->GetResult();

            if (!isImm(lhs)) {
                auto guidx = lhs->guidx();
                for (auto &&p: livenow) {
                    intg[p].insert(guidx);
                    intg[guidx].insert(p);
                }
                livenow.erase(guidx);
            }

            for (auto &&rhs : rhss) {
                if (isImm(rhs)) continue;

                livenow.insert(rhs->guidx());
            }
        }
    }

    return intg;
}

}  // namespace

auto CodeGen::GenBB() -> void {
    auto &&gvals = comp_unit.getGlbTable();
    GValToBBs(gvals);
    for (auto &&func : comp_unit.GetNormalFuncTable()) {
        FuncToBBs(func, gvals);
    }
}

auto CodeGen::GValToBBs(SymbolTable &gvals) -> std::shared_ptr<ASMBasic> { return nullptr; }

auto CodeGen::FuncToBBs(NormalFuncPtr &func, SymbolTable &gvals) -> std::shared_ptr<ASMBasic> {
    auto &&topo = func->TopoSortFromEntry();
    auto &&total = topo.size();
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

    // assuming de-phi and mem2reg is done

    // construct interference graph
    auto intg = construct_interference_graph(topo, liveness);

    return nullptr;
}

}  // namespace backend