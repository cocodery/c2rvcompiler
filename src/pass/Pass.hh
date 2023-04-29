#pragma once

#include "IR.hh"
#include "analysis/dominance/domAnalysis.hh"
#include "interprocedural/deadCodeElim/deadCodeElim.hh"
#include "interprocedural/ssa/ssa.hh"

class Optimization {
   private:
    CompilationUnit &comp_unit;

   public:
    Optimization(CompilationUnit &);
    void DoOptimization();
};
