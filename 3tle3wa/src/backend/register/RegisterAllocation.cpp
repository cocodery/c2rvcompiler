#include <algorithm>
#include <deque>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "3tle3wa/backend/asm/AsmAbi.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/rl/InterferenceGraph.hh"
#include "3tle3wa/backend/rl/InternalTranslation.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLGen.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/instruction/opCode.hh"

void RLPlanner::UseRealRegister(size_t rridx) { used_real_regs_.insert(rridx); }

bool InterferenceGraph::allocaColor(IGNode *node, std::set<size_t> &all, std::set<size_t> &args,
                                    std::set<size_t> &callers, std::set<size_t> &callees) {
    auto &&intfer = node->InterferWith(nodes_);
    std::set<size_t> candidates;

    if (auto argidx = node->PreferWhichArg(); argidx != (size_t)-1 and intfer.find(argidx) == intfer.end()) {
        node->SetColor(argidx);
        return true;
    }

    if (node->PreferCalleeSave()) {
        std::set_difference(callees.begin(), callees.end(), intfer.begin(), intfer.end(),
                            std::inserter(candidates, candidates.end()));
    } else if (node->PreferCallerSave()) {
        std::set_difference(callers.begin(), callers.end(), intfer.begin(), intfer.end(),
                            std::inserter(candidates, candidates.end()));
    } else {
        std::set_difference(args.begin(), args.end(), intfer.begin(), intfer.end(),
                            std::inserter(candidates, candidates.end()));
    }

    if (not candidates.empty()) {
        auto color = *candidates.begin();
        node->SetColor(color);
        planner_->UseRealRegister(color);
        return true;
    }

    std::set_difference(all.begin(), all.end(), intfer.begin(), intfer.end(),
                        std::inserter(candidates, candidates.end()));

    if (not candidates.empty()) {
        auto color = *candidates.begin();
        node->SetColor(color);
        planner_->UseRealRegister(color);
        return true;
    }

    if (not node->CanSpill()) {
        panic("allocate %" PRIu64 "failed", node->GetRef()->GetVRIdx());
    }

    return false;
}

bool InterferenceGraph::KempeOptimistic(std::set<size_t> &args, std::set<size_t> &callers, std::set<size_t> &callees) {
    struct pack {
        IGNode *node;

        bool operator<(const pack &other) const { return *node < *other.node; }
        bool operator>(const pack &other) const { return *node > *other.node; }
    };

    std::set<size_t> all;

    all.insert(callers.begin(), callers.end());
    all.insert(callees.begin(), callees.end());

    std::priority_queue<pack, std::vector<pack>, std::greater<>> pq;
    std::vector<IGNode *> spills;

    for (auto &&node : nodes_) {
        pack pk = {.node = node.second.get()};
        pq.push(pk);
    }

    while (not pq.empty()) {
        auto node = pq.top().node;
        pq.pop();

        if (node->GetRef()->IsAllocated()) {
            continue;
        }

        auto success = allocaColor(node, all, args, callers, callees);

        if (not success) {
            spills.push_back(node);
        }
    }

    printf("spill:");
    for (auto &&vr : spills) {
        printf(" %" PRIu64, vr->GetRef()->GetVRIdx());
    }
    printf("\n");

    return spills.empty();
}

bool RLPlanner::GraphAllocation() {
    InterferenceGraph ig4i(this), ig4f(this);

    struct pack {
        const Segment *seg;

        bool operator<(const pack &other) const { return seg->GetBegin() <= other.seg->GetBegin(); }
        bool operator>(const pack &other) const { return seg->GetBegin() > other.seg->GetBegin(); }
    };

    struct rpack {
        const Segment *seg;

        bool operator<(const rpack &other) const { return seg->GetEnd() <= other.seg->GetEnd(); }
        bool operator>(const rpack &other) const { return seg->GetEnd() > other.seg->GetEnd(); }
    };

    std::priority_queue<pack, std::vector<pack>, std::greater<>> fpq;
    std::priority_queue<pack, std::vector<pack>, std::greater<>> ipq;

    for (auto &&vr : vr_storage_) {
        if (vr->IsOnStk()) {
            continue;
        }

        if (vr->UseFGPR()) {
            ig4f.RegisterIGNode(vr.get());
            auto &&segs = vr->GetLiveSegs();

            for (auto &&seg : segs) {
                fpq.push(pack{.seg = &seg});
            }
        } else if (vr->UseIGPR()) {
            ig4i.RegisterIGNode(vr.get());
            auto &&segs = vr->GetLiveSegs();

            for (auto &&seg : segs) {
                ipq.push(pack{.seg = &seg});
            }
        } else {
            panic("unexpected");
        }
    }

    std::set<rpack> frpq;
    std::set<rpack> irpq;

    while (not ipq.empty()) {
        auto seg = ipq.top().seg;
        ipq.pop();

        auto cur_time = seg->GetBegin();

        for (auto it = irpq.begin(); it != irpq.end(); ++it) {
            if (it->seg->GetEnd() <= cur_time) {
                it = irpq.erase(it);
            }
            break;
        }

        auto vridx = seg->GetOwner()->GetVRIdx();
        for (auto &&pk : irpq) {
            ig4i.Connect(pk.seg->GetOwner()->GetVRIdx(), vridx);
        }

        irpq.insert(rpack{.seg = seg});
    }

    while (not fpq.empty()) {
        auto seg = fpq.top().seg;
        fpq.pop();

        auto cur_time = seg->GetBegin();

        for (auto it = frpq.begin(); it != frpq.end(); ++it) {
            if (it->seg->GetEnd() <= cur_time) {
                it = frpq.erase(it);
            }
            break;
        }

        auto vridx = seg->GetOwner()->GetVRIdx();
        for (auto &&pk : frpq) {
            ig4i.Connect(pk.seg->GetOwner()->GetVRIdx(), vridx);
        }

        frpq.insert(rpack{.seg = seg});
    }

    ig4i.KempeOptimistic(abi_reg_info.i.arg, abi_reg_info.i.caller_save, abi_reg_info.i.callee_save);
    ig4f.KempeOptimistic(abi_reg_info.f.arg, abi_reg_info.f.caller_save, abi_reg_info.f.callee_save);

    return false;
}

bool RLProgress::registerAllocation() { return planner_->GraphAllocation(); }

void RLPlanner::GenerateStackInfo() {
    // ra
    size_t save_cnt = 1;

    for (auto &&v : used_real_regs_) {
        if (abi_reg_info.i.callee_save.count(v) or abi_reg_info.f.callee_save.count(v)) {
            save_cnt += 1;
        }
    }

    struct pack {
        StackInfo *si;

        bool operator<(const pack &other) const { return si->GetStackIdx() < other.si->GetStackIdx(); }
        bool operator>(const pack &other) const { return si->GetStackIdx() > other.si->GetStackIdx(); }
    };

    std::priority_queue<pack> spq;
    std::deque<StackInfo *> allocas;
    std::deque<StackInfo *> onstk_params;

    for (auto &&si : stk_storage_) {
        if (si->IsParam()) {
            onstk_params.push_back(si.get());
        } else if (si->GetStackLength() > 8) {
            allocas.push_back(si.get());
        } else {
            spq.push(pack{.si = si.get()});
        }
    }

    size_t spoff = 0;

    while (not spq.empty()) {
        auto top = spq.top().si;
        spq.pop();

        auto siz = top->GetStackLength();

        if (siz == 8) {
            spoff = RoundUp(8, spoff);
        }

        top->SetOff(spoff);
        spoff += siz;
    }

    while (not allocas.empty()) {
        auto stk = allocas.front();
        allocas.pop_front();

        auto siz = stk->GetStackLength();

        stk->SetOff(spoff);
        spoff += siz;
    }

    spoff = RoundUp(16, spoff + save_cnt * 8);

    total_stack_size_ = spoff;

    for (auto &&p : onstk_params) {
        p->SetOff(p->GetParamStackOff() + total_stack_size_);
    }
}