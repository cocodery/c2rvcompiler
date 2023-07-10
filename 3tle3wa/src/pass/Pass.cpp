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

        GVN::DVNT(func);

        SSA::SSADestruction(func);

        GVN::DVNT(func);

        func->SetVarIdx(Variable::GetVarIdx());
        func->SetBlkIdx(BasicBlock::GetBlkIdx());
    }
}