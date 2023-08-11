#include "3tle3wa/pass/Pass.hh"

#include <memory>
#include <queue>

#include "3tle3wa/ir/value/globalvalue.hh"
#include "3tle3wa/ir/value/variable.hh"

Optimization::Optimization(CompilationUnit &_comp_unit) : comp_unit(_comp_unit) {}

void Optimization::DoOptimization() {
    GVL::GlbValueLocalization(comp_unit);

    auto AnalysisSideEffect = [](CompilationUnit &comp_unit, NormalFuncPtr &func) {
        for (auto &&callee : func->GetCallWho()) {
            if (callee->GetSideEffect()) {
                return true;
            }
        }
        for (auto &&[_, value] : comp_unit.getGlbTable().GetNameValueMap()) {
            if (value->IsGlobalValue()) {
                auto &&glb_value = std::static_pointer_cast<GlobalValue>(value);
                auto &&definers = glb_value->GetDefineIn();
                if (definers.find(func.get()) != definers.end()) {
                    return true;
                }
            }
        }
        for (auto &&param : func->GetParamList()) {
            if (param->IsVariable() && param->GetBaseType()->IsPointer()) {
                auto &&parameter = std::static_pointer_cast<Variable>(param);
                std::queue<VariablePtr> queue;
                queue.push(parameter);
                while (!queue.empty()) {
                    auto &&front = queue.front();
                    queue.pop();
                    for (auto &&user : front->GetUserList()) {
                        if (user->IsStoreInst()) {
                            return true;
                        }
                        if (user->GetResult() != nullptr) {
                            queue.push(user->GetResult());
                        }
                    }
                }
            }
        }
        return false;
    };

    for (auto &&func : comp_unit.GetNormalFuncTable()) {
        func->SetSideEffect(AnalysisSideEffect(comp_unit, func));

        Variable::SetVarIdx(func->GetVarIdx());
        BasicBlock::SetBlkIdx(func->GetBlkIdx());

        Dominance::DominanceAnalysis(func);

        SSA::SSAConstruction(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        StructureAnalysis::LoopAnalysis(func);

        LoopInvariant::LoopInvariant(func);

        SCCP::SCCP(func);  // cause `final_performance/derich` WA, waiting fix

        DCE::DCE(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        InstComb::InstCombine(func);

        GVN::DVNT(func, comp_unit.getGlbTable());

        SSA::SSADestruction(func);

        DCE::DCE(func);

        func->SetVarIdx(Variable::GetVarIdx());
        func->SetBlkIdx(BasicBlock::GetBlkIdx());

        EDP::EliminateDeadParameter(func);
    }
}