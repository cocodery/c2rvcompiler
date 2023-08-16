#include "3tle3wa/pass/analysis/sideeffect/sideeffect.hh"

void SideEffect::SideEffectAnalysis(CompilationUnit &comp_unit, NormalFuncPtr func) {
    bool side_effect = false;
    for (auto &&callee : func->GetCallWho()) {
        if (callee->GetSideEffect()) {
            side_effect = true;
            func->se_type.call_se_func = true;
        }
    }
    for (auto &&[_, value] : comp_unit.getGlbTable().GetNameValueMap()) {
        if (value->IsGlobalValue()) {
            auto &&glb_value = std::static_pointer_cast<GlobalValue>(value);
            auto &&definers = glb_value->GetDefineIn();
            if (definers.find(func.get()) != definers.end()) {
                side_effect = true;
                func->se_type.mod_glb_value = true;
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
                        func->se_type.mod_param_arr = true;
                    }
                    if (user->GetResult() != nullptr) {
                        queue.push(user->GetResult());
                    }
                }
            }
        }
    }

    func->SetSideEffect(side_effect);
    return;
};