#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>

#include "3tle3wa/ir/function/cfgNode.hh"

typedef std::list<CtrlFlowGraphNode *> LoopBlocks;

typedef int32_t loop_depth_t;

class NormalFunction;

struct Loop {
    NormalFunction *func_parent;

    Loop *parent;
    loop_depth_t loop_depth;

    CtrlFlowGraphNode *before_loop;

    CtrlFlowGraphNode *cond_begin;
    CtrlFlowGraphNode *cond_end;

    CtrlFlowGraphNode *body_begin;
    CtrlFlowGraphNode *body_end;

    CtrlFlowGraphNode *loop_exit;

    std::list<Loop *> sub_loops;

    Loop(NormalFunction *_func_parent, Loop *_parent = nullptr, loop_depth_t _depth = 0)
        : func_parent(_func_parent),
          parent(_parent),
          loop_depth(_depth),
          before_loop(nullptr),
          cond_begin(nullptr),
          cond_end(nullptr),
          body_begin(nullptr),
          body_end(nullptr),
          loop_exit(nullptr) {}

    LoopBlocks GetCondBodyBlks();
    LoopBlocks GetLoopBodyBlks();

    LoopBlocks GetEntireLoop();
};

void PrintLoop(Loop &);
