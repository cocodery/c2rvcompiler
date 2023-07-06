#include "3tle3wa/backend/InternalTranslation.hh"

#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/utils/Logs.hh"
#include "3tle3wa/ir/IR.hh"

InternalTranslation::InternalTranslation(const NormalFuncPtr &fptr, const std::unordered_map<uint32_t, size_t> &lc_map,
                                         const std::unordered_map<size_t, AsmGlobalValue *> &gv_map)
    : fptr_(fptr), lc_map_(lc_map), gv_map_(gv_map) {
    auto &&nxt_var_idx = fptr_->GetVarIdx();
    auto &&func_name = fptr_->GetFuncName();

    auto planner = std::make_unique<RLPlanner>(nxt_var_idx);
    curstat_.planner = planner.get();

    rlps_ = std::make_unique<RLProgress>(func_name);
    rlps_->RegisterPlanner(planner);
}

void InternalTranslation::DoTranslation() {
    for (auto &&param : fptr_->GetParamList()) {
        auto var = dynamic_cast<Variable *>(param.get());
        Assert(var, "input param is not variable");

        auto &&var_type = var->GetBaseType();

        if (var_type->IsPointer()) {
            rlps_->SetParam(var, VREG_TYPE::PTR);
        } else if (var_type->IntType()) {
            rlps_->SetParam(var, VREG_TYPE::INT);
        } else if (var_type->FloatType()) {
            rlps_->SetParam(var, VREG_TYPE::FLT);
        } else {
            panic("unexpected");
        }
    }

    auto &&topo = fptr_->TopoSortFromEntry();
    auto &&nxt_cfgit = topo.begin();
    auto &&cur_cfgit = nxt_cfgit++;

    curstat_.meetcall = false;

    while (cur_cfgit != topo.end()) {
        auto rlbb = std::make_unique<RLBasicBlock>(*cur_cfgit);

        curstat_.cur_cfg = cur_cfgit->get();
        curstat_.nxt_cfg = nxt_cfgit->get();
        curstat_.cur_blk = rlbb.get();
        curstat_.meettail = false;

        for (auto &inst : (*cur_cfgit)->GetInstList()) {
            inst->TranslateTo(*this);
        }

        rlps_->Push(rlbb);

        cur_cfgit = nxt_cfgit++;
    }
}
