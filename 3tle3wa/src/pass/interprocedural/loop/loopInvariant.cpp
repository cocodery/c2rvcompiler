#include "3tle3wa/pass/interprocedural/loop/loopInvariant.hh"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <queue>
#include <unordered_map>

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/function/loop.hh"
#include "3tle3wa/ir/value/use.hh"

void LoopInvariant::LoopInvariant(NormalFuncPtr func) {
    // PrintLoop(*(func->loops));
    assert(visit.size() == 0 && is_variant.size() == 0);

    InvariantMotion(func->loops);
    visit.clear();
    is_variant.clear();
}

void LoopInvariant::InvariantMotion(Loop *loop) {
    for (auto &&sub_loop : loop->sub_loops) {
        InvariantMotion(sub_loop);
    }
    if (loop->before_loop) {
        Invariants invariants = FindInvariant(loop);

        auto &&target_node = loop->before_loop;
        for (auto &&inst : invariants) {
            auto &&source_node = inst->GetParent();

            source_node->RemoveInst(inst);

            auto &&inst_list = target_node->GetInstList();
            auto &&end = inst_list.end();
            std::advance(end, -1);
            inst_list.insert(end, inst);

            inst->SetParent(std::make_shared<CtrlFlowGraphNode>(*target_node));
        }
    }
}

LoopInvariant::Invariants LoopInvariant::FindInvariant(Loop *loop) {
    Invariants invariants;

    LoopBlocks &&entire_loop = loop->GetEntireLoop();

    std::queue<Instruction *> variant;

    for (auto &&node : entire_loop) {
        if (visit[node] == false) {
            auto &&inst_list = node->GetInstList();

            for (auto &&inst : inst_list) {
                if (inst->IsPhiInst() || inst->IsCallInst() || inst->IsJumpInst() || inst->IsBranchInst() ||
                    inst->IsLoadInst()) {
                    variant.push(inst.get());       // assume phi, call, jump, branch, load are variant
                    is_variant[inst.get()] = true;  // tag as invariant
                } else {
                    invariants.push_back(inst);  // assume other inst are invariant temporarily
                }
            }
            visit[node] = true;  // tag node in `loop` is visited
        }
    }

    while (!variant.empty()) {
        auto &&inst = variant.front();
        variant.pop();
        auto &&result = inst->GetResult();

        if (result != nullptr) {  // exclude inst without result
            for (auto &&user : result->GetUserList()) {
                // if user is defined in loop and is not tagged as invariant
                if (visit[user->GetParent().get()] && !is_variant[user.get()]) {
                    invariants.remove(user);  // remove from invariant list

                    variant.push(user.get());       // push into queue to iterate
                    is_variant[user.get()] = true;  // tag as invariant
                }
            }
        }
    }

    return invariants;
}