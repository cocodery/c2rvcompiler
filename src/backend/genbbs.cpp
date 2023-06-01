#include <codegen.hh>
#include <queue>
#include <riscv-asm.hh>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace backend {

namespace {

struct liveinfo {
    std::unordered_set<size_t> UEVar;
    std::unordered_set<size_t> VarKill;
    std::unordered_set<size_t> liveout;
};

struct Node {
    size_t freq;
    std::unordered_set<size_t> another;
};

using Graph = std::unordered_map<size_t, Node>;

using vliveinfo = std::unordered_map<size_t, liveinfo>;

auto genLabelName(std::string funcname, size_t idx) -> std::string {
    return std::string("label.") + funcname + "." + std::to_string(idx);
}

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
}

auto recompute(vliveinfo &glvif, const CfgNodePtr &curcfgnd) -> bool {
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

auto construct_interference_graph(const CfgNodeList &topo, vliveinfo &liveness) -> Graph {
    Graph intg;

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
                for (auto &&p : livenow) {
                    intg[p].another.insert(guidx);
                    intg[guidx].another.insert(p);
                }
                livenow.erase(guidx);
            }

            for (auto &&rhs : rhss) {
                if (isImm(rhs)) continue;
                intg[rhs->guidx()].freq += 1;
                livenow.insert(rhs->guidx());
            }
        }
    }

    return intg;
}

auto choose_to_release(const Graph &intg) -> size_t {
    /**
     * @brief pq store cost index pair
     */
    std::priority_queue<std::pair<double, size_t>> pq;
    for (auto &&p : intg) {
        double cost = (double)p.second.freq / (p.second.another.size() * p.second.another.size());
        pq.push({cost, p.first});
    }
    return pq.top().second;
}

/**
 * @brief using buttom to top algorithm
 */
auto color_graph(Graph &intg, const size_t k) -> std::unordered_map<size_t, size_t> {
    auto intg_copy = intg;
    std::stack<size_t> stk;
    bool found = false;

    // stage one
    while (!intg_copy.empty()) {
        for (auto &&p : intg_copy) {
            if (p.second.another.size() < k) {
                found = true;
                for (auto &&anop : p.second.another) {
                    intg_copy[anop].another.erase(p.first);
                }
                stk.push(p.first);
                intg_copy.erase(p.first);
                break;
            }
        }
        if (found) {
            continue;
        }
        auto release = choose_to_release(intg_copy);
        auto node = intg_copy.find(release);
        for (auto &&anop : node->second.another) {
            intg_copy[anop].another.erase(node->first);
        }
        stk.push(node->first);
        intg_copy.erase(node->first);
    }

    // stage two
    // std::unordered_set<size_t> spill;
    std::unordered_map<size_t, size_t> plan;

    while (!stk.empty()) {
        auto node = stk.top();
        stk.pop();
        intg_copy[node] = intg[node];
        size_t colored[k] = {0};
        for (auto &&p : intg_copy[node].another) {
            if (intg_copy.find(p) != intg_copy.end() && plan[node] != -1) {
                colored[plan[p]] = p;
            }
        }
        plan[node] = -1;
        for (size_t i = 0; i < k; ++i) {
            if (colored[i] == 0) {
                plan[node] = i;
                break;
            }
        }

        // just skip for spill
        if (plan[node] == -1) {
            return {};
            // // spill by freq
            // std::priority_queue<std::pair<size_t, std::pair<size_t, size_t>>> pq;
            // for (size_t i = 0; i < k; ++i) {
            //     if (colored[i] != 0) {
            //         auto freq = intg_copy[colored[i]].freq;
            //         pq.push({freq, {colored[i], i}});
            //     }
            // }
            // auto &&[idx, color] = pq.top().second;
            // spill.insert(idx);
            // plan[idx] = -1;
            // plan[node] = color;
        }
    }

    return plan;
}

auto InsttoASM(const InstPtr &inst, const std::unordered_map<size_t, size_t> &plan) -> std::shared_ptr<ASMInst> {
    // TODO: fix it
    switch (inst->GetOpCode()) {
        case None: {
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Ret: {
            panic("not implement yet");
            auto &&Iret = std::dynamic_pointer_cast<ReturnInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Jump: {
            panic("not implement yet");
            auto &&Ijmp = std::dynamic_pointer_cast<JumpInst>(inst);
            auto asminst = std::make_unique<RV_J$>();
            return asminst;
        } break;
        case Branch: {
            panic("not implement yet");
            auto &&Ibranch = std::dynamic_pointer_cast<BranchInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Alloca: {
            panic("not implement yet");
            auto &&Ialloc = std::dynamic_pointer_cast<AllocaInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Store: {
            panic("not implement yet");
            auto &&Istore = std::dynamic_pointer_cast<StoreInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Gep: {
            panic("not implement yet");
            auto &&Igep = std::dynamic_pointer_cast<GetElementPtrInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Call: {
            panic("not implement yet");
            auto &&Icall = std::dynamic_pointer_cast<CallInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Phi: {
            panic("not implement yet");
            auto &&Iphi = std::dynamic_pointer_cast<PhiInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Load: {
            panic("not implement yet");
            auto &&Iload = std::dynamic_pointer_cast<LoadInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case BitCast: {
            panic("not implement yet");
            auto &&Ibcast = std::dynamic_pointer_cast<BitCastInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case SiToFp: {
            panic("not implement yet");
            auto &&Ii2f = std::dynamic_pointer_cast<SitoFpInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case FpToSi: {
            panic("not implement yet");
            auto &&If2i = std::dynamic_pointer_cast<FptoSiInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        case Zext: {
            panic("not implement yet");
            auto &&Izext = std::dynamic_pointer_cast<ZextInst>(inst);
            auto asminst = std::make_unique<RV_NOP>();
            return asminst;
        } break;
        default:
            panic("Invalid");
    }
}

auto CFGtoASM(const CfgNodePtr &cfgnode, const std::unordered_map<size_t, size_t> &plan)
    -> std::list<std::shared_ptr<ASMInst>> {
    auto &&insts = cfgnode->GetInstList();
    std::list<std::shared_ptr<ASMInst>> lst;
    for (auto &&inst : insts) {
        lst.push_back(InsttoASM(inst, plan));
    }
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
    std::unordered_map<size_t, size_t> plan;
    auto &&topo = func->TopoSortFromEntry();
    auto &&total = topo.size();
    {
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
        auto &&intg = construct_interference_graph(topo, liveness);

        plan = color_graph(intg, 27);

        if (plan.size() == 0) {
            panic("register spill and linear scan not implement yet");
        }
    }

    auto progress = std::make_unique<Progress>();
    progress->name = func->GetFuncName();
    for (auto &&cfgnode : topo) {
        auto asmbb = std::make_unique<ASMBasicBlock>();
        asmbb->tagname = std::string("label.") + func->GetFuncName() + "." + std::to_string(cfgnode->GetBlockIdx());
        asmbb->asms = CFGtoASM(cfgnode, plan);
        progress->abbs.push_back(std::move(asmbb));
    }

    return progress;
}

}  // namespace backend