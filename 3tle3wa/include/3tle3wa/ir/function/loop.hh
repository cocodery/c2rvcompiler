#pragma once

#include <cstddef>
#include <list>
#include <memory>

#include "3tle3wa/ir/function/cfgNode.hh"

typedef std::list<CtrlFlowGraphNode *> LoopBlks;

struct Loop {
    Loop *parent;

    CtrlFlowGraphNode *before_loop;

    CtrlFlowGraphNode *cond_begin;
    CtrlFlowGraphNode *cond_end;
    CtrlFlowGraphNode *loop_exit;

    std::list<Loop *> sub_loops;

    Loop(Loop *parent = nullptr, CtrlFlowGraphNode *begin = nullptr, CtrlFlowGraphNode *end = nullptr,
         CtrlFlowGraphNode *exit = nullptr)
        : parent(parent), cond_begin(begin), cond_end(end), loop_exit(exit) {}
};
