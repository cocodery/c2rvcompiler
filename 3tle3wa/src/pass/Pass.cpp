#include "3tle3wa/pass/Pass.hh"

#include <memory>
#include <queue>

#include "3tle3wa/ir/value/globalvalue.hh"
#include "3tle3wa/ir/value/variable.hh"
#include "3tle3wa/pass/interprocedural/dce/dce.hh"

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

        LoopUnrolling::LoopUnrolling(func);

        SCCP::SCCP(func);

        DCE::DCE(func);

        PeepHole::PeepHoleOpt(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        InstComb::InstCombine(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        // SSA::SSADestruction(func);

        DCE::DCE(func);

        func->SetVarIdx(Variable::GetVarIdx());
        func->SetBlkIdx(BasicBlock::GetBlkIdx());

        EDP::EliminateDeadParameter(func);
    }
}