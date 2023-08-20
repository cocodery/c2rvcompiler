#include "3tle3wa/pass/Pass.hh"

Optimization::Optimization(CompilationUnit &_comp_unit) : comp_unit(_comp_unit) {}

void Optimization::DoOptimization() {
    GVL::GlbValueLocalization(comp_unit);

    for (auto &&func : comp_unit.GetNormalFuncTable()) {
        SideEffect::SideEffectAnalysis(comp_unit, func);

        Variable::SetVarIdx(func->GetVarIdx());
        BasicBlock::SetBlkIdx(func->GetBlkIdx());

        Dominance::DominanceAnalysis(func);

        SSA::SSAConstruction(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        StructureAnalysis::LoopAnalysis(func);

        LoopInvariant::LoopInvariant(func);

        LoopMemset::LoopMemset(func, comp_unit);

        PeepHole::PeepHole4Gep(func, comp_unit.getGlbTable());

        // LoopUnrolling::LoopUnrolling(func);

        SCCP::SCCP(func);

        DCE::DCE(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        PeepHole::PeepHoleOpt(func);

        InstComb::InstCombine(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        // SSA::SSADestruction(func);

        DCE::DCE(func);

        TailCall::TailCallAnalysis(func);

        RecursionOpt::DoRecursionOpt(func, comp_unit.getGlbTable());

        func->SetVarIdx(Variable::GetVarIdx());
        func->SetBlkIdx(BasicBlock::GetBlkIdx());

        EDP::EliminateDeadParameter(func);
    }
}