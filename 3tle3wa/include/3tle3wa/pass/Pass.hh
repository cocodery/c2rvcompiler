#pragma once

#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/pass/analysis/dominance/dominance.hh"
#include "3tle3wa/pass/interprocedural/dce/dce.hh"
#include "3tle3wa/pass/interprocedural/dvnt/dvnt.hh"
#include "3tle3wa/pass/interprocedural/ssa/ssa.hh"

class Optimization {
   private:
    CompilationUnit &comp_unit;

   public:
    Optimization(CompilationUnit &);
    void DoOptimization();
};
