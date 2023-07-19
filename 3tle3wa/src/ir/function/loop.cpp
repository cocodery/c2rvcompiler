#include "3tle3wa/ir/function/loop.hh"

#include <memory>
#include <queue>
#include <sstream>
#include <unordered_map>

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/controlFlowInst.hh"
#include "3tle3wa/ir/instruction/instruction.hh"

LoopBlocks Loop::GetCondBodyBlks() {
    LoopBlocks loop_cond_blks;
    // if (cond_begin == cond_end) {
    //     loop_cond_blks.push_back(cond_begin);
    //     return loop_cond_blks;
    // }

    std::queue<CtrlFlowGraphNode *> queue;
    std::unordered_map<CtrlFlowGraphNode *, bool> visit;
    queue.push(cond_begin);
    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();
        if (!visit[node]) {
            visit[node] = true;
            loop_cond_blks.push_back(node);

            Instruction *last_inst = node->GetLastInst().get();
            if (last_inst->IsBranchInst()) {
                BranchInst *br_inst = static_cast<BranchInst *>(last_inst);
                CtrlFlowGraphNode *lhs = br_inst->GetTrueTarget().get();
                CtrlFlowGraphNode *rhs = br_inst->GetFalseTarget().get();

                if (lhs != body_begin && lhs != loop_exit && !visit[lhs]) {
                    queue.push(lhs);
                }
                if (rhs != body_begin && rhs != loop_exit && !visit[rhs]) {
                    queue.push(rhs);
                }
            }
        }
    }
    return loop_cond_blks;
}

LoopBlocks Loop::GetLoopBodyBlks() {
    LoopBlocks loop_body_blks;

    return loop_body_blks;
}

void PrintLoop(Loop &loop) {
    auto &&PrintTab = [](loop_depth_t depth) {
        std::stringstream ss;
        for (int idx = 0; idx < depth; ++idx) {
            ss << "\t";
        }
        return ss.str();
    };

    if (loop.before_loop) {
        cout << PrintTab(loop.loop_depth) << "\b\bLoop_" << loop.loop_depth << endl;
        cout << PrintTab(loop.loop_depth) << "Before-Loop: Block_" << loop.before_loop->GetBlockIdx() << endl;
        cout << PrintTab(loop.loop_depth) << "Cond-Begin : Block_" << loop.cond_begin->GetBlockIdx() << endl;
        cout << PrintTab(loop.loop_depth) << "Cond-End   : Block_" << loop.cond_end->GetBlockIdx() << endl;
        cout << PrintTab(loop.loop_depth) << "Body-Begin : Block_" << loop.body_begin->GetBlockIdx() << endl;
        cout << PrintTab(loop.loop_depth) << "Body-End   : Block_" << loop.body_end->GetBlockIdx() << endl;
        cout << PrintTab(loop.loop_depth) << "Loop-Exit  : Block_" << loop.loop_exit->GetBlockIdx() << endl;

        cout << PrintTab(loop.loop_depth) << "  Conditions :";
        for (auto cond : loop.GetCondBodyBlks()) {
            cout << " Block_" << cond->GetBlockIdx();
        }
        cout << endl;

        cout << PrintTab(loop.loop_depth) << "  Loop-Bodys :";
        for (auto cond : loop.GetLoopBodyBlks()) {
            cout << " Block_" << cond->GetBlockIdx();
        }
        cout << endl;
    }
    for (auto &&sub_loop : loop.sub_loops) {
        PrintLoop(*sub_loop);
    }
}
