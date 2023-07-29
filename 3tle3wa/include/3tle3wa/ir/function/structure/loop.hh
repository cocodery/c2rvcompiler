#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/instruction.hh"

typedef std::list<CfgNodePtr> LoopBlocks;

typedef int32_t loop_depth_t;

class NormalFunction;
using NormalFuncPtr = std::shared_ptr<NormalFunction>;

struct Loop {
    NormalFuncPtr func_parent;

    Loop *parent;
    loop_depth_t loop_depth;

    CfgNodePtr before_loop;

    CfgNodePtr cond_begin;
    CfgNodePtr cond_end;

    CfgNodePtr body_begin;
    CfgNodePtr body_end;

    CfgNodePtr loop_exit;

    std::list<Loop *> sub_loops;

    Loop(NormalFuncPtr _func_parent, Loop *_parent = nullptr, loop_depth_t _depth = 0)
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
