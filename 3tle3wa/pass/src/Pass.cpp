#include "3tle3wa/pass/Pass.hh"

Optimization::Optimization(CompilationUnit &_comp_unit) : comp_unit(_comp_unit) {}

void Optimization::DoOptimization() {
    for (auto &&func : comp_unit.GetNormalFuncTable()) {
        Variable::SetVarIdx(func->GetVarIdx());
        BasicBlock::SetBlkIdx(func->GetBlkIdx());

        Dominance::DominanceAnalysis(func);

        SSA::SSAConstruction(func);

        DCE::EliminateUselessCode(func);

        GVN::DVNT(func);

        // DCE::EliminateUselessControlFlow(func);

        SSA::SSADestruction(func);
    }
}