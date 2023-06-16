#pragma once

#include <unordered_map>
#include <variant>

#include "../abb/abb-gattr.hh"
#include "../abb/abb-gvalue.hh"
#include "../abb/abb-prog.hh"
#include "../codegen.hh"

void FunctoProg(NormalFunction *func, ABBProg *prog);