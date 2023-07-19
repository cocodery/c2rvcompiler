#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>

#include "3tle3wa/ir/function/cfgNode.hh"

typedef std::list<CtrlFlowGraphNode *> LoopBlks;

typedef int32_t loop_depth_t;

struct Loop {
    Loop *parent;

    CtrlFlowGraphNode *before_loop;

    CtrlFlowGraphNode *cond_begin;
    CtrlFlowGraphNode *cond_end;
    CtrlFlowGraphNode *loop_exit;

    loop_depth_t loop_depth;

    std::list<Loop *> sub_loops;

    Loop(Loop *parent = nullptr, CtrlFlowGraphNode *begin = nullptr, CtrlFlowGraphNode *end = nullptr,
         CtrlFlowGraphNode *exit = nullptr, loop_depth_t _depth = 0)
        : parent(parent), cond_begin(begin), cond_end(end), loop_exit(exit), loop_depth(_depth) {}
};

void PrintLoop(Loop &);
