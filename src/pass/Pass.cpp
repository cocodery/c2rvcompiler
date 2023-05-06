#include "Pass.hh"

Optimization::Optimization(CompilationUnit &_comp_unit) : comp_unit(_comp_unit) {}

void Optimization::DoOptimization() {
    for (auto &&normal_func : comp_unit.GetNormalFuncTable()) {
        CfgNodePtr entry = normal_func->GetEntryNode();
        CfgNodePtr exit = normal_func->GetExitNode();
        CfgNodeList allNodes = normal_func->TopoSortFromEntry();

        Variable::SetVarIdx(normal_func->GetVarIdx());
        BasicBlock::SetBlkIdx(normal_func->GetBlkIdx());

        DeadCodeElimination::EliminateUnreachableCode(exit, allNodes);

        StaticSingleAssignment::SSAConstruction(entry, allNodes);

        DeadCodeElimination::EliminateUselessCode(allNodes);

        DeadCodeElimination::EliminateUselessControlFlow(normal_func);
    }
}