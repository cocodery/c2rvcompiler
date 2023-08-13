#pragma once

#include <set>
#include <variant>

#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/function/structure/loop.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/value/baseValue.hh"
#include "3tle3wa/ir/value/use.hh"
#include "3tle3wa/pass/analysis/structure/structure.hh"

typedef std::vector<BaseValuePtr> Operands;
namespace LoopUnrolling {

static bool init_flag;

static std::set<BaseValuePtr> phi_results;
static std::set<BaseValuePtr> loop_variants;
static std::set<InstPtr> phi_to_update;
static std::unordered_map<BaseValuePtr, BaseValuePtr> phi_source_defined_in_loop;
static std::unordered_map<BaseValuePtr, BaseValuePtr> phi_source_defined_out_loop;
static std::unordered_map<BaseValuePtr, BaseValuePtr> phi_source_updated;
static std::unordered_map<BaseValuePtr, BaseValuePtr> old_to_new;  // old variant mapping to updated one
static std::unordered_map<CfgNodePtr, CfgNodePtr> block_mapping;

void LoopUnrolling(NormalFuncPtr);
bool ExpandLoop(Loop *);
void FullyExpand(int, Loop *);
void FullyExpand_mul_blks(int, Loop *);
void AddJmpInst(CfgNodePtr &);
void AddBranchInst(CfgNodePtr &);
void AddPhiInst();
int LoopTime(Loop *);
int ConstCheck(InstPtr);
OpCode FlipComparisonOp(OpCode);
Operands InstOperandsInVector(InstPtr);
BaseValuePtr InstCopy(InstPtr &, CfgNodePtr &);
BaseValuePtr OperandUpdate(const BaseValuePtr);
CfgNodePtr CfgNodeUpdate(const CfgNodePtr);
}  // namespace LoopUnrolling