#include "3tle3wa/pass/analysis/sideeffect/sideeffect.hh"

void SideEffect::SideEffectAnalysis(CompilationUnit &comp_unit, NormalFuncPtr func) {
    bool side_effect = false;
    for (auto &&callee : func->GetCallWho()) {
        if (callee->GetSideEffect()) {
            side_effect = true;
        }
    }
    for (auto &&[_, value] : comp_unit.getGlbTable().GetNameValueMap()) {
        if (value->IsGlobalValue()) {
            auto &&glb_value = std::static_pointer_cast<GlobalValue>(value);
            auto &&definers = glb_value->GetDefineIn();
            if (definers.find(func.get()) != definers.end()) {
                side_effect = true;
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
                        side_effect = true;
                    }
                    if (user->GetResult() != nullptr) {
                        queue.push(user->GetResult());
                    }
                }
            }
        }
    }
    side_effect = false;

    func->SetSideEffect(side_effect);
    return;
};