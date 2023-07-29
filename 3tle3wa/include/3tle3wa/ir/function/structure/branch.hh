#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/instruction.hh"

typedef std::list<CfgNodePtr> BranchBlocks;

typedef int32_t branch_depth_t;

class NormalFunction;
using NormalFuncPtr = std::shared_ptr<NormalFunction>;

struct Branch {
    NormalFuncPtr func_parent;

    Branch *parent;
    branch_depth_t branch_depth;

    CfgNodePtr cond_begin;
    CfgNodePtr cond_end;

    CfgNodePtr iftrue_begin;
    CfgNodePtr iftrue_end;

    CfgNodePtr iffalse_begin;
    CfgNodePtr iffalse_end;

    CfgNodePtr branch_out;

    std::list<Branch *> sub_branches;

    Branch(NormalFuncPtr _func_parent, Branch *_parent = nullptr, branch_depth_t _depth = 0)
        : func_parent(_func_parent),
          parent(_parent),
          branch_depth(_depth),
          cond_begin(nullptr),
          cond_end(nullptr),
          iftrue_begin(nullptr),
          iftrue_end(nullptr),
          iffalse_begin(nullptr),
          iffalse_end(nullptr),
          branch_out(nullptr) {}

    BranchBlocks GetCondBodyBlks();

    BranchBlocks GetIftrueBlks();
    BranchBlocks GetIffalseBlks();
};
