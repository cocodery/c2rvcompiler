#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>

#include "3tle3wa/ir/function/cfgNode.hh"

typedef std::list<CtrlFlowGraphNode *> LoopBlocks;

typedef int32_t loop_depth_t;

struct Loop {
    Loop *parent;
    loop_depth_t loop_depth;

    CtrlFlowGraphNode *before_loop;

    CtrlFlowGraphNode *cond_begin;
    CtrlFlowGraphNode *cond_end;

    CtrlFlowGraphNode *body_begin;
    CtrlFlowGraphNode *body_end;

    CtrlFlowGraphNode *loop_exit;

    std::list<Loop *> sub_loops;

    Loop(Loop *parent = nullptr, loop_depth_t _depth = 0) : parent(parent), loop_depth(_depth) {}

    LoopBlocks GetCondBodyBlks();
    LoopBlocks GetLoopBodyBlks();

    LoopBlocks GetEntireLoop();
};

void PrintLoop(Loop &);
