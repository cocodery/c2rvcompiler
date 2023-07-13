#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmLocalConstant.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/backend/rl/Enums.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/riscv/asm.hh"
#include "3tle3wa/riscv/spec.hh"
#include "3tle3wa/utils/logs.hh"

// t0, t1, ft0, ft1 are reserved for loading problems

extern std::unordered_set<size_t> caller_save;

extern std::unordered_set<size_t> callee_save;

void RLPlanner::Init(AsmBasicBlock *abb) {
    if (total_stack_size_ == 0) {
        return;
    }

    auto save_ra = new riscv::SD(riscv::ra, riscv::sp, -8);
    auto save_fp = new riscv::SD(riscv::fp, riscv::sp, -16);
    auto update_fp = new riscv::MV(riscv::fp, riscv::sp);

    abb->Push(save_ra);
    abb->Push(save_fp);
    abb->Push(update_fp);

    if (ImmWithin(12, -(int64_t)total_stack_size_)) {
        auto make_stack = new riscv::ADDI(riscv::sp, riscv::sp, -(int64_t)total_stack_size_);
        abb->Push(make_stack);
    } else {
        auto load_imm = new riscv::LI(riscv::t0, total_stack_size_);
        auto make_stack = new riscv::SUB(riscv::sp, riscv::sp, riscv::t0);
        abb->Push(load_imm);
        abb->Push(make_stack);
    }

    for (auto &&[rridx, off] : place_to_save_) {
        if (callee_save.find(rridx) != callee_save.end()) {
            if (rridx >= 32) {
                auto save_reg = new riscv::FSW(rridx, riscv::fp, off);
                abb->Push(save_reg);
            } else {
                auto save_reg = new riscv::SD(rridx, riscv::fp, off);
                abb->Push(save_reg);
            }
        }
    }
}

void RLPlanner::Recover(AsmBasicBlock *abb) {
    if (total_stack_size_ == 0) {
        return;
    }

    for (auto &&[rridx, off] : place_to_save_) {
        if (callee_save.find(rridx) != callee_save.end()) {
            if (rridx >= 32) {
                auto recover_reg = new riscv::FLW(rridx, riscv::fp, off);
                abb->Push(recover_reg);
            } else {
                auto recover_reg = new riscv::LD(rridx, riscv::fp, off);
                abb->Push(recover_reg);
            }
        }
    }

    if (ImmWithin(12, total_stack_size_)) {
        auto rm_stack = new riscv::ADDI(riscv::sp, riscv::sp, total_stack_size_);
        abb->Push(rm_stack);
    } else {
        auto load_imm = new riscv::LI(riscv::t0, total_stack_size_);
        auto rm_stack = new riscv::ADD(riscv::sp, riscv::sp, riscv::t0);
        abb->Push(load_imm);
        abb->Push(rm_stack);
    }

    auto recover_ra = new riscv::LD(riscv::ra, riscv::sp, -8);
    auto recover_fp = new riscv::LD(riscv::fp, riscv::sp, -16);

    abb->Push(recover_ra);
    abb->Push(recover_fp);
}

void RLPlanner::BeforeCall(AsmBasicBlock *abb, std::unordered_set<VirtualRegister *> &living_regs) {
    for (auto &&reg : living_regs) {
        if (not reg->OnStk() and not reg->IsSaving() and caller_save.find(reg->GetRRid()) != caller_save.end()) {
            auto rridx = reg->GetRRid();
            auto off = place_to_save_.at(rridx);
            if (reg->GetType() == VREG_TYPE::FLT) {
                auto store = new riscv::FSW(rridx, riscv::fp, off);
                abb->Push(store);
            } else {
                auto store = new riscv::SD(rridx, riscv::fp, off);
                abb->Push(store);
            }
            reg->SetSaving(off);
            savings_.push(reg);
        }
    }
}

void RLPlanner::RecoverCall(AsmBasicBlock *abb, const std::vector<VirtualRegister *> &after_this) {
    auto blk = belong_to_->FindBlkById(abb->Lbidx());
    while (not savings_.empty()) {
        auto reg = savings_.front();
        savings_.pop();
        if (not reg->OnStk()) {
            auto rrid = reg->GetRRid();

            if (reg->IsSaving() and caller_save.find(rrid) != caller_save.end()) {
                reg->GetRRidWithSaving(abb);
                auto cancelable = not blk->IsLiveOut(reg->GetVRIdx()) and not reg->IsThisRet();

                for (auto &&v : after_this) {
                    if (v == reg) {
                        cancelable = false;
                    }
                }

                if (cancelable) {
                    abb->Pop();
                }
            }
        }
    }
}

size_t VirtualRegister::GetRRidWithSaving(AsmBasicBlock *abb) {
    if (saving_) {
        if (type_ == VREG_TYPE::FLT) {
            auto load = new riscv::FLW(real_regidx_, riscv::fp, save_off_);
            abb->Push(load);
        } else {
            auto load = new riscv::LD(real_regidx_, riscv::fp, save_off_);
            abb->Push(load);
        }
        saving_ = false;
    }
    return real_regidx_;
}

void VirtualRegister::LoadTo(size_t to, size_t to_tmp, AsmBasicBlock *abb, RLPlanner *plan) {
    if (not onstack_) {
        panic("not on stack");
    }

    Assert(sinfo_ != nullptr, "sinfo should not be nullptr");
    int64_t soff = sinfo_->GetOff();

    if (param_) {
        switch (type_) {
            case VREG_TYPE::PTR: {
                if (ImmWithin(12, soff)) {
                    auto load_to = new riscv::LD(to, riscv::fp, soff);
                    abb->Push(load_to);
                } else {
                    auto load_imm = new riscv::LI(to, soff);
                    auto gen_addr = new riscv::ADD(to, to, riscv::fp);
                    auto load_to = new riscv::LD(to, to, 0);
                    abb->Push(load_imm);
                    abb->Push(gen_addr);
                    abb->Push(load_to);
                }
            } break;

            case VREG_TYPE::FLT: {
                if (ImmWithin(12, soff)) {
                    auto load_to = new riscv::FLW(to, riscv::fp, soff);
                    abb->Push(load_to);
                } else {
                    Assert(to_tmp != riscv::zero, "set another tmp reg");
                    auto load_imm = new riscv::LI(to_tmp, soff);
                    auto gen_addr = new riscv::ADD(to_tmp, to_tmp, riscv::fp);
                    auto load_to = new riscv::FLW(to, to_tmp, 0);
                    abb->Push(load_imm);
                    abb->Push(gen_addr);
                    abb->Push(load_to);
                }
            } break;

            case VREG_TYPE::INT: {
                if (ImmWithin(12, soff)) {
                    auto load_to = new riscv::LW(to, riscv::fp, soff);
                    abb->Push(load_to);
                } else {
                    auto load_imm = new riscv::LI(to, soff);
                    auto gen_addr = new riscv::ADD(to, to, riscv::fp);
                    auto load_to = new riscv::LW(to, to, 0);
                    abb->Push(load_imm);
                    abb->Push(gen_addr);
                    abb->Push(load_to);
                }
            } break;

            default:
                panic("unexpected");
        }
        return;
    }

    int64_t off_sp = sinfo_->GetOff();
    int64_t off_fp = plan->TotalStackSize() - off_sp;

    switch (type_) {
        case VREG_TYPE::PTR: {
            if (ImmWithin(12, off_sp)) {
                auto load_to = new riscv::LD(to, riscv::sp, off_sp);
                abb->Push(load_to);
            } else if (ImmWithin(12, -off_fp)) {
                auto load_to = new riscv::LD(to, riscv::fp, -off_fp);
                abb->Push(load_to);
            } else {
                auto load_imm = new riscv::LI(to, sinfo_->GetOff());
                auto gen_addr = new riscv::ADD(to, to, riscv::sp);
                auto load_to = new riscv::LD(to, to, 0);
                abb->Push(load_imm);
                abb->Push(gen_addr);
                abb->Push(load_to);
            }
        } break;

        case VREG_TYPE::FLT: {
            if (ImmWithin(12, off_sp)) {
                auto load_to = new riscv::FLW(to, riscv::sp, off_sp);
                abb->Push(load_to);
            } else if (ImmWithin(12, -off_fp)) {
                auto load_to = new riscv::FLW(to, riscv::fp, -off_fp);
                abb->Push(load_to);
            } else {
                Assert(to_tmp != riscv::zero, "set another tmp reg");
                auto load_imm = new riscv::LI(to_tmp, sinfo_->GetOff());
                auto gen_addr = new riscv::ADD(to_tmp, to_tmp, riscv::sp);
                auto load_to = new riscv::FLW(to, to_tmp, 0);
                abb->Push(load_imm);
                abb->Push(gen_addr);
                abb->Push(load_to);
            }
        } break;

        case VREG_TYPE::INT: {
            if (ImmWithin(12, off_sp)) {
                auto load_to = new riscv::LW(to, riscv::sp, off_sp);
                abb->Push(load_to);
            } else if (ImmWithin(12, -off_fp)) {
                auto load_to = new riscv::LW(to, riscv::fp, -off_fp);
                abb->Push(load_to);
            } else {
                auto load_imm = new riscv::LI(to, sinfo_->GetOff());
                auto gen_addr = new riscv::ADD(to, to, riscv::sp);
                auto load_to = new riscv::LW(to, to, 0);
                abb->Push(load_imm);
                abb->Push(gen_addr);
                abb->Push(load_to);
            }
        } break;

        default:
            panic("unexpected");
    }
}

void VirtualRegister::StoreFrom(size_t from, size_t to_tmp, AsmBasicBlock *abb, RLPlanner *plan) {
    if (not onstack_) {
        panic("not on stack");
    }

    Assert(sinfo_ != nullptr, "sinfo should not be nullptr");
    int64_t soff = sinfo_->GetOff();

    if (param_) {
        switch (type_) {
            case VREG_TYPE::PTR: {
                if (ImmWithin(12, soff)) {
                    auto load_to = new riscv::SD(from, riscv::fp, soff);
                    abb->Push(load_to);
                } else {
                    Assert(to_tmp != riscv::zero, "set another tmp reg");
                    auto load_imm = new riscv::LI(to_tmp, soff);
                    auto gen_addr = new riscv::ADD(to_tmp, to_tmp, riscv::fp);
                    auto load_to = new riscv::SD(from, to_tmp, 0);
                    abb->Push(load_imm);
                    abb->Push(gen_addr);
                    abb->Push(load_to);
                }
            } break;

            case VREG_TYPE::FLT: {
                if (ImmWithin(12, soff)) {
                    auto load_to = new riscv::FSW(from, riscv::fp, soff);
                    abb->Push(load_to);
                } else {
                    Assert(to_tmp != riscv::zero, "set another tmp reg");
                    auto load_imm = new riscv::LI(to_tmp, soff);
                    auto gen_addr = new riscv::ADD(to_tmp, to_tmp, riscv::fp);
                    auto load_to = new riscv::FSW(from, to_tmp, 0);
                    abb->Push(load_imm);
                    abb->Push(gen_addr);
                    abb->Push(load_to);
                }
            } break;

            case VREG_TYPE::INT: {
                if (ImmWithin(12, soff)) {
                    auto load_to = new riscv::SW(from, riscv::fp, soff);
                    abb->Push(load_to);
                } else {
                    Assert(to_tmp != riscv::zero, "set another tmp reg");
                    auto load_imm = new riscv::LI(to_tmp, soff);
                    auto gen_addr = new riscv::ADD(to_tmp, to_tmp, riscv::fp);
                    auto load_to = new riscv::SW(from, to_tmp, 0);
                    abb->Push(load_imm);
                    abb->Push(gen_addr);
                    abb->Push(load_to);
                }
            } break;

            default:
                panic("unexpected");
        }
    }

    int64_t off_sp = sinfo_->GetOff();
    int64_t off_fp = plan->TotalStackSize() - off_sp;

    switch (type_) {
        case VREG_TYPE::PTR: {
            if (ImmWithin(12, off_sp)) {
                auto store_from = new riscv::SD(from, riscv::sp, off_sp);
                abb->Push(store_from);
            } else if (ImmWithin(12, -off_fp)) {
                auto store_from = new riscv::SD(from, riscv::fp, -off_fp);
                abb->Push(store_from);
            } else {
                Assert(to_tmp != riscv::zero, "set another tmp reg");
                auto load_imm = new riscv::LI(to_tmp, off_sp);
                auto gen_addr = new riscv::ADD(to_tmp, to_tmp, riscv::sp);
                auto store_from = new riscv::SD(from, to_tmp, 0);
                abb->Push(load_imm);
                abb->Push(gen_addr);
                abb->Push(store_from);
            }
        } break;

        case VREG_TYPE::FLT: {
            if (ImmWithin(12, off_sp)) {
                auto store_from = new riscv::FSW(from, riscv::sp, off_sp);
                abb->Push(store_from);
            } else if (ImmWithin(12, -off_fp)) {
                auto store_from = new riscv::FSW(from, riscv::fp, -off_fp);
                abb->Push(store_from);
            } else {
                Assert(to_tmp != riscv::zero, "set another tmp reg");
                auto load_imm = new riscv::LI(to_tmp, off_sp);
                auto gen_addr = new riscv::ADD(to_tmp, to_tmp, riscv::sp);
                auto store_from = new riscv::FSW(from, to_tmp, 0);
                abb->Push(load_imm);
                abb->Push(gen_addr);
                abb->Push(store_from);
            }
        } break;

        case VREG_TYPE::INT: {
            if (ImmWithin(12, off_sp)) {
                auto store_from = new riscv::SW(from, riscv::sp, off_sp);
                abb->Push(store_from);
            } else if (ImmWithin(12, -off_fp)) {
                auto store_from = new riscv::SW(from, riscv::fp, -off_fp);
                abb->Push(store_from);
            } else {
                Assert(to_tmp != riscv::zero, "set another tmp reg");
                auto load_imm = new riscv::LI(to_tmp, off_sp);
                auto gen_addr = new riscv::ADD(to_tmp, to_tmp, riscv::sp);
                auto store_from = new riscv::SW(from, to_tmp, 0);
                abb->Push(load_imm);
                abb->Push(gen_addr);
                abb->Push(store_from);
            }
        } break;

        default:
            panic("unexpected");
    }
}

void UopRet::ToAsm(AsmBasicBlock *abb, RLPlanner *plan) {
    abb->SetIsRet(true);

    if (retval_ != nullptr) {
        if (retval_->OnStk()) {
            if (retval_->FGPR()) {
                retval_->LoadTo(riscv::fa0, riscv::a0, abb, plan);
            } else {
                retval_->LoadTo(riscv::a0, riscv::zero, abb, plan);
            }
        } else if (retval_->FGPR()) {
            if (retval_->GetRRidWithSaving(abb) != riscv::fa0) {
                auto set_retval = new riscv::FMV_S(riscv::fa0, retval_->GetRRid());
                abb->Push(set_retval);
            }
        } else if (retval_->GetRRidWithSaving(abb) != riscv::a0) {
            auto set_retval = new riscv::MV(riscv::a0, retval_->GetRRid());
            abb->Push(set_retval);
        }
    }

    plan->Recover(abb);

    auto ret = new riscv::RET();
    abb->Push(ret);
}

void UopCall::ToAsm(CRVC_UNUSE AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    for (auto &&param : params_) {
        if (not param->OnStk()) {
            if (riscv::a0 <= param->GetRRid() and riscv::a7 >= param->GetRRid()) {
                living_regs_.insert(param);
                continue;
            }
            if (riscv::fa0 <= param->GetRRid() and riscv::fa7 >= param->GetRRid()) {
                living_regs_.insert(param);
                continue;
            }
        }
    }
    plan->BeforeCall(abb, living_regs_);

    std::unordered_set<size_t> editted;

    extern size_t abi_arg_reg;

    size_t ips = 0;
    size_t fps = 0;
    size_t sps = 0;

    for (auto &&param : params_) {
        auto &&ptype = param->GetType();

        bool edit = true;

        if (ptype == VREG_TYPE::FLT) {
            if (fps < abi_arg_reg) {
                if (param->OnStk()) {
                    param->LoadTo(riscv::fa0 + fps, riscv::t0, abb, plan);
                } else {
                    auto rrid = param->GetRRid();
                    if (param->IsSaving() or editted.find(rrid) != editted.end()) {
                        Assert(param->IsSaving() or (*param->Imgr().AskInterval(abb->GetBlockIdx()))[uop_idx_] == false,
                               "not save before call");
                        auto set_param = new riscv::FLW(riscv::fa0 + fps, riscv::fp, param->SavingInfo());
                        abb->Push(set_param);
                    } else if (rrid == riscv::fa0 + fps) {
                        edit = false;
                    } else {
                        auto set_param = new riscv::FMV_S(riscv::fa0 + fps, rrid);
                        abb->Push(set_param);
                    }
                }

                if (edit) {
                    editted.insert(riscv::fa0 + fps);
                }

                fps += 1;
            } else {
                if (param->OnStk()) {
                    param->LoadTo(riscv::ft0, riscv::zero, abb, plan);
                    auto set_param = new riscv::FSW(riscv::ft0, riscv::sp, sps * 8);
                    abb->Push(set_param);
                } else {
                    auto rrid = param->GetRRid();
                    if (param->IsSaving() or editted.find(rrid) != editted.end()) {
                        Assert(param->IsSaving() or (*param->Imgr().AskInterval(abb->GetBlockIdx()))[uop_idx_] == false,
                               "not save before call");
                        auto load = new riscv::LW(riscv::t0, riscv::fp, param->SavingInfo());
                        auto set_param = new riscv::SW(riscv::t0, riscv::sp, sps * 8);
                        abb->Push(load);
                        abb->Push(set_param);
                    } else {
                        auto set_param = new riscv::FSW(rrid, riscv::sp, sps * 8);
                        abb->Push(set_param);
                    }
                }

                sps += 1;
            }
        } else {
            if (ips < abi_arg_reg) {
                if (param->OnStk()) {
                    param->LoadTo(riscv::a0 + ips, riscv::zero, abb, plan);
                } else {
                    auto rrid = param->GetRRid();
                    if (param->IsSaving() or editted.find(rrid) != editted.end()) {
                        Assert(param->IsSaving() or (*param->Imgr().AskInterval(abb->GetBlockIdx()))[uop_idx_] == false,
                               "not save before call");
                        if (param->GetType() == VREG_TYPE::PTR) {
                            auto set_param = new riscv::LD(riscv::a0 + ips, riscv::fp, param->SavingInfo());
                            abb->Push(set_param);
                        } else if (param->GetType() == VREG_TYPE::INT) {
                            auto set_param = new riscv::LW(riscv::a0 + ips, riscv::fp, param->SavingInfo());
                            abb->Push(set_param);
                        } else {
                            panic("unexpected");
                        }
                    } else if (rrid == riscv::a0 + ips) {
                        edit = false;
                    } else {
                        auto set_param = new riscv::MV(riscv::a0 + ips, rrid);
                        abb->Push(set_param);
                    }
                }

                if (edit) {
                    editted.insert(riscv::a0 + ips);
                }

                ips += 1;
            } else {
                if (param->OnStk()) {
                    param->LoadTo(riscv::t0, riscv::zero, abb, plan);
                    auto set_param = new riscv::SD(riscv::t0, riscv::sp, sps * 8);
                    abb->Push(set_param);
                } else {
                    auto rrid = param->GetRRid();
                    if (param->IsSaving() or editted.find(rrid) != editted.end()) {
                        Assert(param->IsSaving() or (*param->Imgr().AskInterval(abb->GetBlockIdx()))[uop_idx_] == false,
                               "not save before call");
                        if (param->GetType() == VREG_TYPE::PTR) {
                            auto load = new riscv::LD(riscv::t0, riscv::fp, param->SavingInfo());
                            abb->Push(load);
                        } else if (param->GetType() == VREG_TYPE::INT) {
                            auto load = new riscv::LW(riscv::t0, riscv::fp, param->SavingInfo());
                            abb->Push(load);
                        } else {
                            panic("unexpected");
                        }

                        auto set_param = new riscv::SD(riscv::t0, riscv::sp, sps * 8);
                        abb->Push(set_param);
                    } else {
                        auto set_param = new riscv::SD(rrid, riscv::sp, sps * 8);
                        abb->Push(set_param);
                    }
                }

                sps += 1;
            }
        }
    }

    if (libcall_) {
        auto callfunc = new riscv::CALL(callee_.c_str());
        abb->Push(callfunc);
    } else if (callself_ and tailcall_ and sps == 0) {
        auto lbidx = abb->SearchFirst();
        auto label = ".L." + std::string(abb->FatherLabel()) + ".b" + std::to_string(lbidx);
        auto callfunc = new riscv::J(label.c_str());
        abb->Push(callfunc);
        return;
    } else {
        auto callfunc = new riscv::JAL(callee_.c_str());
        abb->Push(callfunc);
    }

    if (retval_ != nullptr) {
        if (retval_->OnStk()) {
            if (retval_->GetType() == VREG_TYPE::FLT) {
                retval_->StoreFrom(riscv::fa0, riscv::t0, abb, plan);
            } else {
                retval_->StoreFrom(riscv::a0, riscv::t0, abb, plan);
            }
        } else {
            if (retval_->GetType() == VREG_TYPE::FLT) {
                if (retval_->IsSaving()) {
                    retval_->GetRRidWithSaving(abb);
                    abb->Pop();
                }
                auto fmv = new riscv::FMV_S(retval_->GetRRid(), riscv::fa0);
                abb->Push(fmv);
            } else {
                if (retval_->IsSaving()) {
                    retval_->GetRRidWithSaving(abb);
                    abb->Pop();
                }
                auto mv = new riscv::MV(retval_->GetRRid(), riscv::a0);
                abb->Push(mv);
            }
        }
    }
}

void UopLui::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    if (dst_->OnStk()) {
        auto lui = new riscv::LUI(riscv::t0, imm_up20_);
        abb->Push(lui);

        dst_->StoreFrom(riscv::t0, riscv::t1, abb, plan);
    } else {
        auto lui = new riscv::LUI(dst_->GetRRidWithSaving(abb), imm_up20_);
        abb->Push(lui);
    }
}

void UopMv::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t src = 0;

    if (src_ == nullptr) {
        if (dst_->GetType() == VREG_TYPE::FLT) {
            auto fmvwx = new riscv::FMV_W_X(dst_->GetRRidWithSaving(abb), riscv::zero);
            abb->Push(fmvwx);
        } else {
            auto mv = new riscv::MV(dst_->GetRRidWithSaving(abb), riscv::zero);
            abb->Push(mv);
        }
    } else if (src_->OnStk()) {
        src_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        src = riscv::t0;
    } else {
        src = src_->GetRRidWithSaving(abb);
    }

    if (dst_->OnStk()) {
        dst_->StoreFrom(src, riscv::t1, abb, plan);
    } else if (src_ != nullptr) {
        auto mv = new riscv::MV(dst_->GetRRidWithSaving(abb), src);
        abb->Push(mv);
    }
}

void UopCvtS2W::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t src = 0;

    if (src_->OnStk()) {
        src_->LoadTo(riscv::ft0, riscv::t0, abb, plan);
        src = riscv::ft0;
    } else {
        src = src_->GetRRidWithSaving(abb);
    }

    if (dst_->OnStk()) {
        auto cvt = new riscv::FCVT_W_S(riscv::t0, src);
        abb->Push(cvt);
        dst_->StoreFrom(riscv::t0, riscv::t1, abb, plan);
    } else {
        auto cvt = new riscv::FCVT_W_S(dst_->GetRRidWithSaving(abb), src);
        abb->Push(cvt);
    }
}

void UopCvtW2S::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t src = 0;

    if (src_->OnStk()) {
        src_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        src = riscv::t0;
    } else {
        src = src_->GetRRidWithSaving(abb);
    }

    if (dst_->OnStk()) {
        auto cvt = new riscv::FCVT_S_W(riscv::ft0, src);
        abb->Push(cvt);
        dst_->StoreFrom(riscv::ft0, riscv::t1, abb, plan);
    } else {
        auto cvt = new riscv::FCVT_S_W(dst_->GetRRidWithSaving(abb), src);
        abb->Push(cvt);
    }
}

void UopBranch::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    auto label = ".L." + std::string(abb->FatherLabel()) + ".b" + std::to_string(dst_idx_);

    size_t cond = 0;

    if (cond_->OnStk()) {
        cond_->LoadTo(riscv::t0, riscv::zero, abb, plan);

        cond = riscv::t0;
    } else {
        cond = cond_->GetRRidWithSaving(abb);
    }

    if (on_true_) {
        auto bnez = new riscv::BNEZ(cond, label.c_str());
        abb->Push(bnez);
    } else {
        auto beqz = new riscv::BEQZ(cond, label.c_str());
        abb->Push(beqz);
    }
}

void UopJump::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    auto label = ".L." + std::string(abb->FatherLabel()) + ".b" + std::to_string(dst_idx_);
    auto jmp = new riscv::J(label.c_str());
    abb->Push(jmp);
}

void UopLla::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    if (src_.empty()) {
        auto stkinfo = dst_->GetAllocaInfo();
        int64_t off = stkinfo->GetOff();
        int64_t off_fp = plan->TotalStackSize() - off;

        if (dst_->OnStk()) {
            if (ImmWithin(12, off)) {
                auto stkp_gen = new riscv::ADDI(riscv::t0, riscv::sp, off);
                abb->Push(stkp_gen);

                dst_->StoreFrom(riscv::t0, riscv::t1, abb, plan);
            } else if (ImmWithin(12, -off_fp)) {
                auto stkp_gen = new riscv::ADDI(riscv::t0, riscv::fp, -off_fp);
                abb->Push(stkp_gen);

                dst_->StoreFrom(riscv::t0, riscv::t1, abb, plan);
            } else {
                auto load_imm = new riscv::LI(riscv::t0, off);
                auto stkp_gen = new riscv::ADD(riscv::t0, riscv::sp, riscv::t0);

                abb->Push(load_imm);
                abb->Push(stkp_gen);

                dst_->StoreFrom(riscv::t0, riscv::t1, abb, plan);
            }
        } else {
            if (ImmWithin(12, off)) {
                auto stkp_gen = new riscv::ADDI(dst_->GetRRidWithSaving(abb), riscv::sp, off);
                abb->Push(stkp_gen);
            } else if (ImmWithin(12, -off_fp)) {
                auto stkp_gen = new riscv::ADDI(dst_->GetRRidWithSaving(abb), riscv::fp, -off_fp);
                abb->Push(stkp_gen);
            } else {
                auto load_imm = new riscv::LI(riscv::t0, off);
                auto stkp_gen = new riscv::ADD(dst_->GetRRidWithSaving(abb), riscv::sp, riscv::t0);

                abb->Push(load_imm);
                abb->Push(stkp_gen);
            }
        }
        return;
    }

    if (dst_->OnStk()) {
        auto stkp_gen = new riscv::LLA_LB(riscv::t0, src_.c_str());
        abb->Push(stkp_gen);

        dst_->StoreFrom(riscv::t0, riscv::t1, abb, plan);
    } else {
        auto stkp_gen = new riscv::LLA_LB(dst_->GetRRidWithSaving(abb), src_.c_str());
        abb->Push(stkp_gen);
    }
}

void UopLoad::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t base = 0;

    if (base_->PtrOnStk()) {
        base_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        base = riscv::t0;
    } else {
        base = base_->GetRRidWithSaving(abb);
    }

    if (dst_->OnStk()) {
        auto lw = new riscv::LW(riscv::t0, base, off_lo12_);
        abb->Push(lw);

        dst_->StoreFrom(riscv::t0, riscv::t1, abb, plan);
    } else {
        auto lw = new riscv::LW(dst_->GetRRidWithSaving(abb), base, off_lo12_);
        abb->Push(lw);
    }
}

void UopStore::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t base = 0;
    size_t src = 0;

    if (base_->PtrOnStk()) {
        base_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        base = riscv::t0;
    } else {
        base = base_->GetRRidWithSaving(abb);
    }

    if (src_ == nullptr) {
        src = riscv::zero;
    } else if (src_->OnStk()) {
        src_->LoadTo(riscv::t1, riscv::zero, abb, plan);
        src = riscv::t1;
    } else {
        src = src_->GetRRidWithSaving(abb);
    }

    auto sw = new riscv::SW(src, base, off_lo12_);
    abb->Push(sw);
}

void UopFLoad::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t base = 0;

    if (base_->PtrOnStk()) {
        base_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        base = riscv::t0;
    } else {
        base = base_->GetRRidWithSaving(abb);
    }

    if (dst_->OnStk()) {
        auto lw = new riscv::FLW(riscv::ft0, base, off_lo12_);
        abb->Push(lw);

        dst_->StoreFrom(riscv::ft0, riscv::t1, abb, plan);
    } else {
        auto lw = new riscv::FLW(dst_->GetRRidWithSaving(abb), base, off_lo12_);
        abb->Push(lw);
    }
}

void UopFStore::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t base = 0;
    size_t src = 0;

    if (base_->PtrOnStk()) {
        base_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        base = riscv::t0;
    } else {
        base = base_->GetRRidWithSaving(abb);
    }

    if (src_->OnStk()) {
        src_->LoadTo(riscv::ft0, riscv::t1, abb, plan);
        src = riscv::ft0;
    } else {
        src = src_->GetRRidWithSaving(abb);
    }

    auto sw = new riscv::FSW(src, base, off_lo12_);
    abb->Push(sw);
}

void UopICmp::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t lhs = 0;
    size_t rhs = 0;

    if (lhs_ == nullptr) {
        lhs = riscv::zero;
    } else if (lhs_->OnStk()) {
        lhs_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        lhs = riscv::t0;
    } else {
        lhs = lhs_->GetRRidWithSaving(abb);
    }

    if (rhs_ == nullptr) {
        rhs = riscv::zero;
    } else if (rhs_->OnStk()) {
        rhs_->LoadTo(riscv::t1, riscv::zero, abb, plan);
        rhs = riscv::t1;
    } else {
        rhs = rhs_->GetRRidWithSaving(abb);
    }

    size_t dst = 0;

    if (dst_->OnStk()) {
        dst = riscv::t0;
    } else {
        dst = dst_->GetRRidWithSaving(abb);
    }

    switch (kind_) {
        case COMP_KIND::EQU: {
            auto cmp = new riscv::XOR(dst, lhs, rhs);
            auto res = new riscv::SEQZ(dst, dst);

            abb->Push(cmp);
            abb->Push(res);
        } break;
        case COMP_KIND::NEQ: {
            auto cmp = new riscv::XOR(dst, lhs, rhs);
            auto res = new riscv::SNEZ(dst, dst);

            abb->Push(cmp);
            abb->Push(res);
        } break;
        case COMP_KIND::LTH: {
            auto cmp = new riscv::SLT(dst, lhs, rhs);

            abb->Push(cmp);
        } break;
        case COMP_KIND::GTH: {
            auto cmp = new riscv::SGT(dst, lhs, rhs);

            abb->Push(cmp);
        } break;
        case COMP_KIND::LEQ: {
            auto cmp = new riscv::SGT(dst, lhs, rhs);
            auto res = new riscv::SEQZ(dst, dst);

            abb->Push(cmp);
            abb->Push(res);
        } break;
        case COMP_KIND::GEQ: {
            auto cmp = new riscv::SLT(dst, lhs, rhs);
            auto res = new riscv::SEQZ(dst, dst);

            abb->Push(cmp);
            abb->Push(res);
        } break;
    }

    if (dst_->OnStk()) {
        dst_->StoreFrom(dst, riscv::t1, abb, plan);
    }
}

void UopFCmp::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t lhs = 0;
    size_t rhs = 0;

    if (lhs_ == nullptr) {
        lhs = riscv::zero;
    }
    if (lhs_->OnStk()) {
        lhs_->LoadTo(riscv::ft0, riscv::t0, abb, plan);
        lhs = riscv::ft0;
    } else {
        lhs = lhs_->GetRRidWithSaving(abb);
    }

    if (rhs_ == nullptr) {
        rhs = riscv::zero;
    }
    if (rhs_->OnStk()) {
        rhs_->LoadTo(riscv::ft1, riscv::t1, abb, plan);
        rhs = riscv::ft1;
    } else {
        rhs = rhs_->GetRRidWithSaving(abb);
    }

    size_t dst = 0;

    if (dst_->OnStk()) {
        dst = riscv::t0;
    } else {
        dst = dst_->GetRRidWithSaving(abb);
    }

    switch (kind_) {
        case COMP_KIND::EQU: {
            auto cmp = new riscv::FEQ(dst, lhs, rhs);

            abb->Push(cmp);
        } break;
        case COMP_KIND::NEQ: {
            auto cmp = new riscv::FEQ(dst, lhs, rhs);
            auto res = new riscv::SEQZ(dst, dst);

            abb->Push(cmp);
            abb->Push(res);
        } break;
        case COMP_KIND::LTH: {
            auto cmp = new riscv::FLT(dst, lhs, rhs);

            abb->Push(cmp);
        } break;
        case COMP_KIND::GTH: {
            auto cmp = new riscv::FLT(dst, rhs, lhs);

            abb->Push(cmp);
        } break;
        case COMP_KIND::LEQ: {
            auto cmp = new riscv::FLE(dst, lhs, rhs);

            abb->Push(cmp);
        } break;
        case COMP_KIND::GEQ: {
            auto cmp = new riscv::FLE(dst, rhs, lhs);

            abb->Push(cmp);
        } break;
    }

    if (dst_->OnStk()) {
        dst_->StoreFrom(dst, riscv::t1, abb, plan);
    }
}

void UopIBin::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t lhs = 0;
    size_t rhs = 0;

    if (lhs_ == nullptr) {
        lhs = riscv::zero;
    } else if (lhs_->OnStk()) {
        lhs_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        lhs = riscv::t0;
    } else {
        lhs = lhs_->GetRRidWithSaving(abb);
    }

    if (rhs_ == nullptr) {
        rhs = riscv::zero;
    } else if (rhs_->OnStk()) {
        rhs_->LoadTo(riscv::t1, riscv::zero, abb, plan);
        rhs = riscv::t1;
    } else {
        rhs = rhs_->GetRRidWithSaving(abb);
    }

    size_t dst = 0;

    if (dst_->OnStk()) {
        dst = riscv::t0;
    } else {
        dst = dst_->GetRRidWithSaving(abb);
    }

    if (dst_->GetSize() == sizeof(intptr_t)) {
        switch (kind_) {
            case IBIN_KIND::ADD: {
                auto ibin = new riscv::ADD(dst, lhs, rhs);
                abb->Push(ibin);
            } break;
            case IBIN_KIND::SUB: {
                auto ibin = new riscv::SUB(dst, lhs, rhs);
                abb->Push(ibin);
            } break;
            case IBIN_KIND::MUL: {
                auto ibin = new riscv::MUL(dst, lhs, rhs);
                abb->Push(ibin);
            } break;
            case IBIN_KIND::SLL: {
                auto ibin = new riscv::SLL(dst, lhs, rhs);
                abb->Push(ibin);
            } break;
            case IBIN_KIND::SRA: {
                auto ibin = new riscv::SRA(dst, lhs, rhs);
                abb->Push(ibin);
            } break;
            case IBIN_KIND::SRL: {
                auto ibin = new riscv::SRL(dst, lhs, rhs);
                abb->Push(ibin);
            } break;
            default:
                panic("ptr op not support this");
        }

        if (dst_->OnStk()) {
            dst_->StoreFrom(dst, riscv::t1, abb, plan);
        }

        return;
    }

    switch (kind_) {
        case IBIN_KIND::ADD: {
            auto ibin = new riscv::ADDW(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::SUB: {
            auto ibin = new riscv::SUBW(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::MUL: {
            auto ibin = new riscv::MULW(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::DIV: {
            auto ibin = new riscv::DIVW(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::REM: {
            auto ibin = new riscv::REMW(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::OR: {
            auto ibin = new riscv::OR(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::XOR: {
            auto ibin = new riscv::XOR(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::AND: {
            auto ibin = new riscv::AND(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::SLL: {
            auto ibin = new riscv::SLLW(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::SRA: {
            auto ibin = new riscv::SRAW(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::SRL: {
            auto ibin = new riscv::SRLW(dst, lhs, rhs);
            abb->Push(ibin);
        } break;
        case IBIN_KIND::MULHS: {
            auto lhs_sext = new riscv::SEXT_W(lhs, lhs);
            auto rhs_sext = new riscv::SEXT_W(rhs, rhs);
            auto mul = new riscv::MUL(lhs, lhs, rhs);
            auto sra = new riscv::SRAI(dst, lhs, 32);

            abb->Push(lhs_sext);
            abb->Push(rhs_sext);
            abb->Push(mul);
            abb->Push(sra);
        } break;
    }

    if (dst_->OnStk()) {
        dst_->StoreFrom(dst, riscv::t1, abb, plan);
    }
}

void UopIBinImm::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t lhs = 0;

    if (lhs_ == nullptr) {
        lhs = riscv::zero;
    } else if (lhs_->OnStk()) {
        lhs_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        lhs = riscv::t0;
    } else {
        lhs = lhs_->GetRRidWithSaving(abb);
    }

    size_t dst = 0;

    if (dst_->OnStk()) {
        dst = riscv::t0;
    } else {
        dst = dst_->GetRRidWithSaving(abb);
    }

    if (dst_->GetSize() == sizeof(intptr_t)) {
        switch (kind_) {
            case IBIN_KIND::ADD: {
                auto ibini = new riscv::ADDI(dst, lhs, imm_lo12_);
                abb->Push(ibini);
            } break;
            case IBIN_KIND::SLL: {
                auto ibini = new riscv::SLLI(dst, lhs, imm_lo12_);
                abb->Push(ibini);
            } break;
            case IBIN_KIND::SRA: {
                auto ibini = new riscv::SRAI(dst, lhs, imm_lo12_);
                abb->Push(ibini);
            } break;
            case IBIN_KIND::SRL: {
                auto ibini = new riscv::SRLI(dst, lhs, imm_lo12_);
                abb->Push(ibini);
            } break;
            case IBIN_KIND::OR: {
                auto ibini = new riscv::ORI(dst, lhs, imm_lo12_);
                abb->Push(ibini);
            } break;
            case IBIN_KIND::XOR: {
                auto ibini = new riscv::XORI(dst, lhs, imm_lo12_);
                abb->Push(ibini);
            } break;
            case IBIN_KIND::AND: {
                auto ibini = new riscv::ANDI(dst, lhs, imm_lo12_);
                abb->Push(ibini);
            } break;
            default:
                panic("imm op not support this");
        }

        if (dst_->OnStk()) {
            dst_->StoreFrom(dst, riscv::t1, abb, plan);
        }

        return;
    }

    switch (kind_) {
        case IBIN_KIND::ADD: {
            auto ibini = new riscv::ADDIW(dst, lhs, imm_lo12_);
            abb->Push(ibini);
        } break;
        case IBIN_KIND::OR: {
            auto ibini = new riscv::ORI(dst, lhs, imm_lo12_);
            abb->Push(ibini);
        } break;
        case IBIN_KIND::XOR: {
            auto ibini = new riscv::XORI(dst, lhs, imm_lo12_);
            abb->Push(ibini);
        } break;
        case IBIN_KIND::AND: {
            auto ibini = new riscv::ANDI(dst, lhs, imm_lo12_);
            abb->Push(ibini);
        } break;
        case IBIN_KIND::SLL: {
            auto ibini = new riscv::SLLIW(dst, lhs, imm_lo12_);
            abb->Push(ibini);
        } break;
        case IBIN_KIND::SRA: {
            auto ibini = new riscv::SRAIW(dst, lhs, imm_lo12_);
            abb->Push(ibini);
        } break;
        case IBIN_KIND::SRL: {
            auto ibini = new riscv::SRLIW(dst, lhs, imm_lo12_);
            abb->Push(ibini);
        } break;
        default:
            panic("imm op not support this");
    }

    if (dst_->OnStk()) {
        dst_->StoreFrom(dst, riscv::t1, abb, plan);
    }
}

void UopFBin::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    size_t lhs = 0;
    size_t rhs = 0;

    if (lhs_->OnStk()) {
        lhs_->LoadTo(riscv::ft0, riscv::t0, abb, plan);
        lhs = riscv::ft0;
    } else {
        lhs = lhs_->GetRRidWithSaving(abb);
    }

    if (rhs_ == nullptr) {
        rhs = riscv::zero;
    } else if (rhs_->OnStk()) {
        rhs_->LoadTo(riscv::ft1, riscv::t1, abb, plan);
        rhs = riscv::ft1;
    } else {
        rhs = rhs_->GetRRidWithSaving(abb);
    }

    size_t dst = 0;

    if (dst_->OnStk()) {
        dst = riscv::ft0;
    } else {
        dst = dst_->GetRRidWithSaving(abb);
    }

    switch (kind_) {
        case FBIN_KIND::ADD: {
            auto fbin = new riscv::FADD_S(dst, lhs, rhs);

            abb->Push(fbin);
        } break;
        case FBIN_KIND::SUB: {
            auto fbin = new riscv::FSUB_S(dst, lhs, rhs);

            abb->Push(fbin);
        } break;
        case FBIN_KIND::MUL: {
            auto fbin = new riscv::FMUL_S(dst, lhs, rhs);

            abb->Push(fbin);
        } break;
        case FBIN_KIND::DIV: {
            auto fbin = new riscv::FDIV_S(dst, lhs, rhs);

            abb->Push(fbin);
        } break;
        case FBIN_KIND::MAX: {
            auto fbin = new riscv::FMAX_S(dst, lhs, rhs);

            abb->Push(fbin);
        } break;
        case FBIN_KIND::MIN: {
            auto fbin = new riscv::FMIN_S(dst, lhs, rhs);

            abb->Push(fbin);
        } break;
        case FBIN_KIND::ABS: {
            auto fbin = new riscv::FABS_S(dst, lhs);

            abb->Push(fbin);
        } break;
        case FBIN_KIND::NEG: {
            auto fbin = new riscv::FNEG_S(dst, lhs);

            abb->Push(fbin);
        } break;
    }

    if (dst_->OnStk()) {
        dst_->StoreFrom(dst, riscv::t1, abb, plan);
    }
}

void UopICmpBranch::ToAsm(AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {
    auto label = ".L." + std::string(abb->FatherLabel()) + ".b" + std::to_string(dst_idx_);

    size_t lhs = 0;
    size_t rhs = 0;

    if (lhs_ == nullptr) {
        lhs = riscv::zero;
    } else if (lhs_->OnStk()) {
        lhs_->LoadTo(riscv::t0, riscv::zero, abb, plan);
        lhs = riscv::t0;
    } else {
        lhs = lhs_->GetRRidWithSaving(abb);
    }

    if (rhs_ == nullptr) {
        rhs = riscv::zero;
    } else if (rhs_->OnStk()) {
        rhs_->LoadTo(riscv::t1, riscv::zero, abb, plan);
        rhs = riscv::t1;
    } else {
        rhs = rhs_->GetRRidWithSaving(abb);
    }

    switch (kind_) {
        case COMP_KIND::EQU: {
            auto bcmp = new riscv::BEQ(lhs, rhs, label.c_str());

            abb->Push(bcmp);
        } break;
        case COMP_KIND::NEQ: {
            auto bcmp = new riscv::BNE(lhs, rhs, label.c_str());

            abb->Push(bcmp);
        } break;
        case COMP_KIND::LTH: {
            auto bcmp = new riscv::BLT(lhs, rhs, label.c_str());

            abb->Push(bcmp);
        } break;
        case COMP_KIND::GTH: {
            auto bcmp = new riscv::BGT(lhs, rhs, label.c_str());

            abb->Push(bcmp);
        } break;
        case COMP_KIND::LEQ: {
            auto bcmp = new riscv::BLE(lhs, rhs, label.c_str());

            abb->Push(bcmp);
        } break;
        case COMP_KIND::GEQ: {
            auto bcmp = new riscv::BGE(lhs, rhs, label.c_str());

            abb->Push(bcmp);
        } break;
    }
}

// void UopPhi::ToAsm(CRVC_UNUSE AsmBasicBlock *abb, CRVC_UNUSE RLPlanner *plan) {}