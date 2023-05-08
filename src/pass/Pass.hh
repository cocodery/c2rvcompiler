#pragma once

#include "IR.hh"
#include "analysis/dominance/dominance.hh"
#include "interprocedural/dce/dce.hh"
#include "interprocedural/gvngcm/gvngcm.hh"
#include "interprocedural/ssa/ssa.hh"

class Optimization {
   private:
    CompilationUnit &comp_unit;

   public:
    Optimization(CompilationUnit &);
    void DoOptimization();
};
