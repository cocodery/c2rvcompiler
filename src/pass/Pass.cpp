#include "Pass.hh"

Optimization::Optimization(CompilationUnit &_comp_unit) : comp_unit(_comp_unit) {}

void Optimization::DoOptimization() {
    for (auto &&func : comp_unit.GetNormalFuncTable()) {
        // Variable::SetVarIdx(func->GetVarIdx());
        // BasicBlock::SetBlkIdx(func->GetBlkIdx());

        // Dominance::DominanceAnalysis(func);

        // SSA::SSAConstruction(func);

        // GVN::DVNT(func);

        // DCE::EliminateUselessCode(func);

        // DCE::EliminateUselessControlFlow(func);
    }
}