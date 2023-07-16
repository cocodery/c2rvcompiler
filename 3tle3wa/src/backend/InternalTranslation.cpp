#include "3tle3wa/backend/InternalTranslation.hh"

#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmLocalConstant.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/utils/logs.hh"

InternalTranslation::InternalTranslation(const NormalFuncPtr &fptr, const std::unordered_map<uint32_t, size_t> &lc_map,
                                         const std::unordered_map<size_t, AsmGlobalValue *> &gv_map)
    : fptr_(fptr), lc_map_(lc_map), gv_map_(gv_map) {
    auto &&nxt_var_idx = fptr_->GetVarIdx();
    auto &&func_name = fptr_->GetFuncName();

    auto planner = std::make_unique<RLPlanner>(nxt_var_idx);
    curstat_.planner = planner.get();

    rlps_ = std::make_unique<RLProgress>(func_name);
    rlps_->RegisterPlanner(planner);

    apg_ = std::make_unique<AsmProgress>(func_name);
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
        auto rlbb = std::make_unique<RLBasicBlock>(*cur_cfgit, curstat_.planner);

        curstat_.cur_cfg = cur_cfgit->get();
        curstat_.nxt_cfg = nxt_cfgit->get();
        curstat_.cur_blk = rlbb.get();
        curstat_.meettail = false;

        if (nxt_cfgit == topo.end()) {
            curstat_.nxt_cfg = nullptr;
        }

        for (auto &inst : (*cur_cfgit)->GetInstList()) {
            inst->TranslateTo(*this);
        }

        rlps_->Push(rlbb);

        cur_cfgit = nxt_cfgit++;
    }

    if (curstat_.meetcall) {
        rlps_->MeetCall();
    }

    // Assert(icmp_map.empty(), "ill formed");
}

std::unique_ptr<AsmProgress> &InternalTranslation::ExportAPG() { return apg_; }
