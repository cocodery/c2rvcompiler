#pragma once

#include <unordered_map>
#include <variant>

#include "../abb/abb-gattr.hh"
#include "../abb/abb-gvalue.hh"
#include "../abb/abb-prog.hh"
#include "../codegen.hh"

using allocplan = std::unordered_map<size_t, size_t>;

allocplan linear_plan(NormalFuncPtr &func);

std::unique_ptr<ABBProg::ABBlock> make_asm(CfgNodePtr &cfgnptr, allocplan &plan, NormalFuncPtr &func);

void reg_nvmap(NameValueMap &gvalues);