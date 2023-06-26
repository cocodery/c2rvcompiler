#pragma once

#include <unordered_map>

#include "3tle3wa/ir/IR.hh"

namespace Inline {

static std::unordered_map<CfgNodePtr, CfgNodePtr> block_map;
static std::unordered_map<BaseValuePtr, BaseValuePtr> value_map;

BaseValuePtr InstCopy(InstPtr, CfgNodePtr);
std::pair<BaseValuePtr, CfgNodePtr> Inline(NormalFuncPtr, NormalFuncPtr, ParamList &, NameValueMap &, CfgNodePtr, bool,
                                           CfgNodePtr);
}  // namespace Inline