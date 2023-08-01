#include "3tle3wa/pass/analysis/structure/structure.hh"

#include <cassert>
#include <stack>

#include "3tle3wa/ir/function/basicblock.hh"
#include "3tle3wa/ir/function/structure/branch.hh"
#include "3tle3wa/ir/function/structure/loop.hh"
#include "3tle3wa/ir/function/structure/structure.hh"

void StructureAnalysis::StructureAnalysis(NormalFuncPtr &func) {
    std::stack<Structure *> stack;
    stack.push(nullptr);
    depth_t depth = 1;

    auto &&seq_nodes = func->GetSequentialNodes();
    for (auto &&node : seq_nodes) {
        auto &&top = stack.top();
        if (node->blk_attr.CheckBlkType(BlkAttr::LoopTag)) {
            assert(node->blk_attr.before_blk);
            Loop *loop = new Loop(top, depth++);
            loop->before_blk = node;
            stack.push(loop);

            if (func->structure == nullptr) func->structure = loop;
        } else if (node->blk_attr.CheckBlkType(BlkAttr::BranchTag)) {
            assert(node->blk_attr.before_blk);
            Branch_ *branch = new Branch_(top, depth++);
            branch->before_blk = node;
            stack.push(branch);

            if (func->structure == nullptr) func->structure = branch;
        } else if (node->blk_attr.cond_begin) {
            if (top->IsLoop()) {
                static_cast<Loop *>(top)->cond_begin = node;
            } else {
                static_cast<Branch_ *>(top)->cond_begin = node;
            }
        } else if (node->blk_attr.cond_end) {
            if (top->IsLoop()) {
                static_cast<Loop *>(top)->cond_end = node;
            } else {
                static_cast<Branch_ *>(top)->cond_end = node;
            }
        } else if (node->blk_attr.body_begin) {
            assert(top->IsLoop());
            static_cast<Loop *>(top)->body_begin = node;
        } else if (node->blk_attr.body_end) {
            assert(top->IsLoop());
            static_cast<Loop *>(top)->body_end = node;
        } else if (node->blk_attr.iftrue_begin) {
            assert(top->IsBranch());
            static_cast<Branch_ *>(top)->iftrue_begin = node;
        } else if (node->blk_attr.iftrue_end) {
            assert(top->IsBranch());
            static_cast<Branch_ *>(top)->iftrue_end = node;
        } else if (node->blk_attr.iffalse_begin) {
            assert(top->IsBranch());
            static_cast<Branch_ *>(top)->iffalse_begin = node;
        } else if (node->blk_attr.iffalse_end) {
            assert(top->IsBranch());
            static_cast<Branch_ *>(top)->iffalse_end = node;
        } else if (node->blk_attr.structure_out) {
            if (top->IsLoop()) {
                static_cast<Loop *>(top)->loop_exit = node;
            } else {
                static_cast<Branch_ *>(top)->branch_out = node;
            }
            stack.pop();
            depth -= 1;
        }
    }
}