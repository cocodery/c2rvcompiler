#include "3tle3wa/pass/Pass.hh"

Optimization::Optimization(CompilationUnit &_comp_unit) : comp_unit(_comp_unit) {}

void Optimization::DoOptimization() {
    for (auto &&func : comp_unit.GetNormalFuncTable()) {
        Variable::SetVarIdx(func->GetVarIdx());
        BasicBlock::SetBlkIdx(func->GetBlkIdx());

        Dominance::DominanceAnalysis(func);

        SSA::SSAConstruction(func);

        DCE::EliminateUselessCode(func);

        SCCP::SCCP(func);
        DCE::EliminateUnreachableCode(func);
        DCE::DCE(func);

        InstComb::InstCombine(func);

        DCE::EliminateUselessCode(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        SSA::SSADestruction(func);

        DCE::DCE(func);

        func->SetVarIdx(Variable::GetVarIdx());
        func->SetBlkIdx(BasicBlock::GetBlkIdx());
    }
}