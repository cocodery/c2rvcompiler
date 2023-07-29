#include "3tle3wa/ir/function/structure/branch.hh"

#include <queue>
#include <stack>
#include <unordered_map>

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/controlFlowInst.hh"
#include "3tle3wa/ir/instruction/instruction.hh"

BranchBlocks Branch::GetCondBodyBlks() {
    BranchBlocks branch_cond_blks;

    std::queue<CfgNodePtr> queue;
    std::unordered_map<CtrlFlowGraphNode *, bool> visit;
    queue.push(cond_begin);
    while (!queue.empty()) {
        auto &&node = queue.front();
        queue.pop();
        if (!visit[node.get()]) {
            visit[node.get()] = true;
            branch_cond_blks.push_back(node);

            Instruction *last_inst = node->GetLastInst().get();
            if (last_inst->IsBranchInst()) {
                BranchInst *br_inst = static_cast<BranchInst *>(last_inst);
                auto &&lhs = br_inst->GetTrueTarget();
                auto &&rhs = br_inst->GetFalseTarget();

                if (lhs != iftrue_begin && lhs != iffalse_begin && !visit[lhs.get()]) {
                    queue.push(lhs);
                }
                if (rhs != iftrue_begin && rhs != iffalse_begin && !visit[rhs.get()]) {
                    queue.push(rhs);
                }
            }
        }
    }
    return branch_cond_blks;
}

BranchBlocks Branch::GetIftrueBlks() {
    BranchBlocks branch_true_blks;

    std::stack<CfgNodePtr> stack;
    std::unordered_map<CtrlFlowGraphNode *, bool> visit;
    stack.push(iftrue_begin);
    while (!stack.empty()) {
        auto &&node = stack.top();
        stack.pop();
        if (!visit[node.get()]) {
            visit[node.get()] = true;
            branch_true_blks.push_back(node);

            Instruction *last_inst = node->GetLastInst().get();
            if (last_inst->IsBranchInst()) {
                BranchInst *br_inst = static_cast<BranchInst *>(last_inst);
                auto &&lhs = br_inst->GetTrueTarget();
                auto &&rhs = br_inst->GetFalseTarget();

                if (rhs != branch_out && !visit[rhs.get()]) {
                    stack.push(rhs);
                }
                if (lhs != branch_out && !visit[lhs.get()]) {
                    stack.push(lhs);
                }
            } else if (last_inst->IsJumpInst()) {
                JumpInst *jump_inst = static_cast<JumpInst *>(last_inst);
                auto &&target = jump_inst->GetTarget();

                if (target != branch_out && !visit[target.get()]) {
                    stack.push(target);
                }
            }
        }
    }
    return branch_true_blks;
}

BranchBlocks Branch::GetIffalseBlks() {
    BranchBlocks branch_false_blks;

    std::stack<CfgNodePtr> stack;
    std::unordered_map<CtrlFlowGraphNode *, bool> visit;
    stack.push(iftrue_begin);
    while (!stack.empty()) {
        auto &&node = stack.top();
        stack.pop();
        if (!visit[node.get()]) {
            visit[node.get()] = true;
            branch_false_blks.push_back(node);

            Instruction *last_inst = node->GetLastInst().get();
            if (last_inst->IsBranchInst()) {
                BranchInst *br_inst = static_cast<BranchInst *>(last_inst);
                auto &&lhs = br_inst->GetTrueTarget();
                auto &&rhs = br_inst->GetFalseTarget();

                if (rhs != branch_out && !visit[rhs.get()]) {
                    stack.push(rhs);
                }
                if (lhs != branch_out && !visit[lhs.get()]) {
                    stack.push(lhs);
                }
            } else if (last_inst->IsJumpInst()) {
                JumpInst *jump_inst = static_cast<JumpInst *>(last_inst);
                auto &&target = jump_inst->GetTarget();

                if (target != branch_out && !visit[target.get()]) {
                    stack.push(target);
                }
            }
        }
    }
    return branch_false_blks;
}
