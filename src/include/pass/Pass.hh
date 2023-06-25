#pragma once

#include "analysis/dominance/dominance.hh"
#include "interprocedural/dce/dce.hh"
#include "interprocedural/dvnt/dvnt.hh"
#include "interprocedural/ssa/ssa.hh"
#include "ir/IR.hh"

class Optimization {
   private:
    CompilationUnit &comp_unit;

   public:
    Optimization(CompilationUnit &);
    void DoOptimization();
};
