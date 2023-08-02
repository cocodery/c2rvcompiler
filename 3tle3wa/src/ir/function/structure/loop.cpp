#include "3tle3wa/ir/function/structure/loop.hh"

#include <cstddef>
#include <memory>
#include <queue>
#include <sstream>
#include <stack>
#include <unordered_map>

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/function/structure/structure.hh"
#include "3tle3wa/ir/instruction/controlFlowInst.hh"
#include "3tle3wa/ir/instruction/instruction.hh"

CfgNodeList Loop::GetCondBodyBlks() const {
    CfgNodeList loop_cond_blks;

    std::queue<CfgNodePtr> queue;
    std::unordered_map<CtrlFlowGraphNode *, bool> visit;
    queue.push(cond_begin);
    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();
        if (!visit[node.get()]) {
            visit[node.get()] = true;
            loop_cond_blks.push_back(node);

            Instruction *last_inst = node->GetLastInst().get();
            if (last_inst->IsBranchInst()) {
                BranchInst *br_inst = static_cast<BranchInst *>(last_inst);
                auto &&lhs = br_inst->GetTrueTarget();
                auto &&rhs = br_inst->GetFalseTarget();

                if (lhs != body_begin && lhs != loop_exit && !visit[lhs.get()]) {
                    queue.push(lhs);
                }
                if (rhs != body_begin && rhs != loop_exit && !visit[rhs.get()]) {
                    queue.push(rhs);
                }
            }
        }
    }
    return loop_cond_blks;
}

CfgNodeList Loop::GetLoopBodyBlks() const {
    CfgNodeList loop_body_blks;

    std::stack<CfgNodePtr> stack;
    std::unordered_map<CfgNodePtr, bool> visit;
    stack.push(body_begin);
    while (!stack.empty()) {
        auto node = stack.top();
        stack.pop();
        if (!visit[node]) {
            visit[node] = true;
            loop_body_blks.push_back(node);

            Instruction *last_inst = node->GetLastInst().get();
            if (last_inst->IsBranchInst()) {
                BranchInst *br_inst = static_cast<BranchInst *>(last_inst);
                auto &&lhs = br_inst->GetTrueTarget();
                auto &&rhs = br_inst->GetFalseTarget();

                if (rhs != cond_begin && rhs != loop_exit && !visit[rhs]) {
                    stack.push(rhs);
                }
                if (lhs != cond_begin && lhs != loop_exit && !visit[lhs]) {
                    stack.push(lhs);
                }
            } else if (last_inst->IsJumpInst()) {
                JumpInst *jump_inst = static_cast<JumpInst *>(last_inst);
                auto &&target = jump_inst->GetTarget();

                if (target != cond_begin && target != loop_exit && !visit[target]) {
                    stack.push(target);
                }
            }
        }
    }

    return loop_body_blks;
}

CfgNodeList Loop::GetEntireStructure() const {
    auto &&loop_blks = GetCondBodyBlks();
    auto &&loop_body = GetLoopBodyBlks();

    loop_blks.insert(loop_blks.end(), loop_body.begin(), loop_body.end());

    return loop_blks;
}

void Loop::PrintStructure() const {
    auto &&PrintTab = [](depth_t depth) {
        std::stringstream ss;
        for (size_t idx = 0; idx < depth; ++idx) {
            ss << "\t";
        }
        return ss.str();
    };

    if (before_blk) {
        cout << PrintTab(depth) << "\b\bLoop_" << depth << endl;
        cout << PrintTab(depth) << "Before-Loop: Block_" << before_blk->GetBlockIdx() << endl;
        cout << PrintTab(depth) << "Cond-Begin : Block_" << cond_begin->GetBlockIdx() << endl;
        cout << PrintTab(depth) << "Cond-End   : Block_" << cond_end->GetBlockIdx() << endl;
        cout << PrintTab(depth) << "Body-Begin : Block_" << body_begin->GetBlockIdx() << endl;
        cout << PrintTab(depth) << "Body-End   : Block_" << body_end->GetBlockIdx() << endl;
        cout << PrintTab(depth) << "Loop-Exit  : Block_" << loop_exit->GetBlockIdx() << endl;

        cout << PrintTab(depth) << "  Conditions  :";
        for (auto cond : GetCondBodyBlks()) {
            cout << " Block_" << cond->GetBlockIdx();
        }
        cout << endl;

        cout << PrintTab(depth) << "  Loop-Bodys  :";
        for (auto body : GetLoopBodyBlks()) {
            cout << " Block_" << body->GetBlockIdx();
        }
        cout << endl;
    }
    for (auto &&sub_structure : sub_structures) {
        sub_structure->PrintStructure();
    }
}
