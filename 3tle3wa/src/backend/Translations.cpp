#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/instruction/opCode.hh"

void InternalTranslation::li(VirtualRegister *dst, uint32_t imm) {
    if (ImmWithin(12, imm)) {
        auto uop_ori = new UopIBinImm;

        int32_t val12 = imm;
        SEXT32(val12, 12);

        uop_ori->SetImm(val12);
        uop_ori->SetLhs(nullptr);
        uop_ori->SetDst(dst);
        uop_ori->SetKind(IBIN_KIND::ADD);

        curstat_.cur_blk->Push(uop_ori);
        return;
    }
    uint32_t msk = 0xFFF;
    auto internal = curstat_.planner->NewVReg(VREG_TYPE::INT);

    int32_t val12 = imm & msk;
    int32_t upper = 0;
    SEXT32(val12, 12);

    if (val12 < 0) {
        uint32_t up20 = imm & (~msk);
        uint32_t down20 = up20 >> 12;
        upper = (down20 + 1) << 12;
    } else {
        upper = imm & (~msk);
    }

    auto uop_lui = new UopLui;
    uop_lui->SetImm(upper);
    uop_lui->SetDst(internal);

    auto uop_ori = new UopIBinImm;
    uop_ori->SetImm(val12);
    uop_ori->SetLhs(internal);
    uop_ori->SetDst(dst);
    if (val12 > 0) {
        uop_ori->SetKind(IBIN_KIND::OR);
    } else {
        uop_ori->SetKind(IBIN_KIND::ADD);
    }

    curstat_.cur_blk->Push(uop_lui);
    curstat_.cur_blk->Push(uop_ori);
}

void InternalTranslation::Translate(ReturnInst *ll) {
    auto uop = new UopRet;

    if (auto &&retval = ll->GetRetValue(); retval != nullptr) {
        if (retval->IsVariable()) {
            auto var = dynamic_cast<Variable *>(retval.get());
            Assert(var, "bad dynamic cast");

            auto vr_retval = curstat_.planner->GetVReg(var->GetVariableIdx());

            uop->SetRetVal(vr_retval);
        } else if (retval->IsConstant()) {
            auto cst = dynamic_cast<Constant *>(retval.get());
            Assert(cst, "bad dynamic cast");

            auto &&cinfo = XConstValue(cst->GetValue());

            if (cinfo.isflt_) {
                auto lc_idx = lc_map_.at(cinfo.v32_.u32_);
                auto lbname = std::string(".LC") + std::to_string(lc_idx);

                auto lc_addr = curstat_.planner->NewVReg(VREG_TYPE::PTR);
                auto flt_val = curstat_.planner->NewVReg(VREG_TYPE::FLT);

                auto uop_lla = new UopLla;
                uop_lla->SetSrc(lbname);
                uop_lla->SetDst(lc_addr);

                auto uop_fload = new UopFLoad;
                uop_fload->SetOff(0);
                uop_fload->SetBase(lc_addr);
                uop_fload->SetDst(flt_val);

                curstat_.cur_blk->Push(uop_lla);
                curstat_.cur_blk->Push(uop_fload);

                uop->SetRetVal(flt_val);
            } else {
                auto vr_retval = curstat_.planner->NewVReg(VREG_TYPE::INT);

                li(vr_retval, cinfo.v32_.u32_);

                uop->SetRetVal(vr_retval);
            }
        } else {
            panic("unexpected");
        }
    }

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(JumpInst *ll) {
    auto uop = new UopJump;

    auto tgid = ll->GetTarget()->GetBlockIdx();

    uop->SetDstIdx(tgid);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(BranchInst *ll) {
    auto nxtblkidx = curstat_.nxt_cfg->GetBlockIdx();

    auto uop = new UopBranch;

    auto tridx = ll->GetTrueTarget()->GetBlockIdx();
    auto faidx = ll->GetFalseTarget()->GetBlockIdx();

    auto &&cond = ll->GetCondition();

    if (cond->IsConstant()) {
        panic("unexpected");
    }

    auto var_cond = dynamic_cast<Variable *>(cond.get());
    auto vr_cond = curstat_.planner->GetVReg(var_cond->GetVariableIdx());
    uop->SetCond(vr_cond);

    if (nxtblkidx == tridx) {
        uop->SetOnTrue(false);
        uop->SetDstIdx(faidx);
    } else if (nxtblkidx == faidx) {
        uop->SetOnTrue(true);
        uop->SetDstIdx(tridx);
    } else {
        panic("unexpected");
    }

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(ICmpInst *ll) {
    auto &&res = ll->GetResult();
    auto lhs = ll->GetLHS();
    auto rhs = ll->GetRHS();
    auto opcode = ll->GetOpCode();

    if (lhs->IsConstant() and rhs->IsConstant()) {
        panic("unexpected");
    }

    VirtualRegister *vrlhs;
    VirtualRegister *vrrhs;

    if (lhs->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(lhs.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());
        Assert(not cinfo.isflt_, "unexpected");

        vrlhs = curstat_.planner->NewVReg(VREG_TYPE::INT);

        li(vrlhs, cinfo.v32_.u32_);
    } else if (lhs->IsVariable()) {
        auto var = dynamic_cast<Variable *>(lhs.get());
        Assert(var, "bad dynamic cast");

        vrlhs = curstat_.planner->GetVReg(var->GetVariableIdx());
    } else {
        panic("unexpected");
    }

    if (rhs->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(rhs.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());
        Assert(not cinfo.isflt_, "unexpected");

        vrrhs = curstat_.planner->NewVReg(VREG_TYPE::INT);

        li(vrrhs, cinfo.v32_.u32_);
    } else if (rhs->IsVariable()) {
        auto var = dynamic_cast<Variable *>(rhs.get());
        Assert(var, "bad dynamic cast");

        vrrhs = curstat_.planner->GetVReg(var->GetVariableIdx());
    } else {
        panic("unexpected");
    }

    if (vrlhs == nullptr or vrrhs == nullptr) {
        panic("unexpected");
    }

    auto dst = curstat_.planner->AllocVReg(VREG_TYPE::INT, res->GetVariableIdx());

    auto uop = new UopICmp;
    uop->SetLhs(vrlhs);
    uop->SetRhs(vrrhs);
    uop->SetDst(dst);
    uop->SetKind((COMP_KIND)opcode);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(FCmpInst *ll) {
    auto &&res = ll->GetResult();
    auto lhs = ll->GetLHS();
    auto rhs = ll->GetRHS();
    auto opcode = ll->GetOpCode();

    if (lhs->IsConstant() and rhs->IsConstant()) {
        panic("unexpected");
    }

    VirtualRegister *vrlhs;
    VirtualRegister *vrrhs;

    if (lhs->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(lhs.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());
        Assert(cinfo.isflt_, "unexpected");

        vrlhs = curstat_.planner->NewVReg(VREG_TYPE::FLT);

        auto lc_idx = lc_map_.at(cinfo.v32_.u32_);
        auto lbname = std::string(".LC") + std::to_string(lc_idx);

        auto lc_addr = curstat_.planner->NewVReg(VREG_TYPE::PTR);

        auto uop_lla = new UopLla;
        uop_lla->SetSrc(lbname);
        uop_lla->SetDst(lc_addr);

        auto uop_fload = new UopFLoad;
        uop_fload->SetOff(0);
        uop_fload->SetBase(lc_addr);
        uop_fload->SetDst(vrlhs);

        curstat_.cur_blk->Push(uop_lla);
        curstat_.cur_blk->Push(uop_fload);
    } else {
        auto var = dynamic_cast<Variable *>(lhs.get());
        Assert(var, "bad dynamic cast");

        vrlhs = curstat_.planner->GetVReg(var->GetVariableIdx());
    }

    if (rhs->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(rhs.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());
        Assert(cinfo.isflt_, "unexpected");

        vrrhs = curstat_.planner->NewVReg(VREG_TYPE::FLT);

        auto lc_idx = lc_map_.at(cinfo.v32_.u32_);
        auto lbname = std::string(".LC") + std::to_string(lc_idx);

        auto lc_addr = curstat_.planner->NewVReg(VREG_TYPE::PTR);

        auto uop_lla = new UopLla;
        uop_lla->SetSrc(lbname);
        uop_lla->SetDst(lc_addr);

        auto uop_fload = new UopFLoad;
        uop_fload->SetOff(0);
        uop_fload->SetBase(lc_addr);
        uop_fload->SetDst(vrrhs);

        curstat_.cur_blk->Push(uop_lla);
        curstat_.cur_blk->Push(uop_fload);
    } else {
        auto var = dynamic_cast<Variable *>(rhs.get());
        Assert(var, "bad dynamic cast");

        vrrhs = curstat_.planner->GetVReg(var->GetVariableIdx());
    }

    if (vrlhs == nullptr or vrrhs == nullptr) {
        panic("unexpected");
    }

    auto dst = curstat_.planner->AllocVReg(VREG_TYPE::INT, res->GetVariableIdx());

    auto uop = new UopFCmp;
    uop->SetLhs(vrlhs);
    uop->SetRhs(vrrhs);
    uop->SetDst(dst);
    uop->SetKind((COMP_KIND)opcode);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(IBinaryInst *ll) {
    auto &&res = ll->GetResult();
    auto lhs = ll->GetLHS();
    auto rhs = ll->GetRHS();
    auto opcode = ll->GetOpCode();

    if (lhs->IsConstant() and rhs->IsConstant()) {
        panic("unexpected");
    }

    bool swappable = false;
    switch (opcode) {
        case OP_MUL:
        case OP_ADD:
            swappable = true;
            break;
        default:
            break;
    }

    VirtualRegister *vrlhs;
    VirtualRegister *vrrhs;

    if (lhs->IsConstant() and swappable) {
        std::swap(lhs, rhs);
    }

    if (lhs->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(lhs.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());
        Assert(not cinfo.isflt_, "unexpected");

        vrlhs = curstat_.planner->NewVReg(VREG_TYPE::INT);

        li(vrlhs, cinfo.v32_.u32_);
    } else if (lhs->IsVariable()) {
        auto var = dynamic_cast<Variable *>(lhs.get());
        Assert(var, "bad dynamic cast");

        vrlhs = curstat_.planner->GetVReg(var->GetVariableIdx());
    } else {
        panic("unexpected");
    }

    if (rhs->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(rhs.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());
        Assert(not cinfo.isflt_, "unexpected");

        if (ImmWithin(12, cinfo.v32_.u32_)) {
            bool immable = false;
            int32_t imm = 0;
            switch (opcode) {
                case OP_ADD:
                    immable = true;
                    imm = cinfo.v32_.i32_;
                    break;
                case OP_SUB:
                    if (ImmWithin(12, -cinfo.v32_.i32_)) {
                        immable = true;
                        imm = -cinfo.v32_.i32_;
                    }
                    break;
                default:
                    break;
            };
            if (immable) {
                auto dst = curstat_.planner->AllocVReg(VREG_TYPE::INT, res->GetVariableIdx());
                auto uop_imm = new UopIBinImm;
                uop_imm->SetImm(imm);
                uop_imm->SetLhs(vrlhs);
                uop_imm->SetDst(dst);
                uop_imm->SetKind(IBIN_KIND::ADD);

                curstat_.cur_blk->Push(uop_imm);

                return;
            }
        }

        vrrhs = curstat_.planner->NewVReg(VREG_TYPE::INT);

        li(vrrhs, cinfo.v32_.u32_);
    } else if (rhs->IsVariable()) {
        auto var = dynamic_cast<Variable *>(rhs.get());
        Assert(var, "bad dynamic cast");

        vrrhs = curstat_.planner->GetVReg(var->GetVariableIdx());
    } else {
        panic("unexpected");
    }

    if (vrlhs == nullptr or vrrhs == nullptr) {
        panic("unexpected");
    }

    auto dst = curstat_.planner->AllocVReg(VREG_TYPE::INT, res->GetVariableIdx());

    auto uop = new UopIBin;
    uop->SetLhs(vrlhs);
    uop->SetRhs(vrrhs);
    uop->SetDst(dst);
    uop->SetKind((IBIN_KIND)opcode);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(FBinaryInst *ll) {
    auto &&res = ll->GetResult();
    auto lhs = ll->GetLHS();
    auto rhs = ll->GetRHS();
    auto opcode = ll->GetOpCode();

    if (lhs->IsConstant() and rhs->IsConstant()) {
        panic("unexpected");
    }

    VirtualRegister *vrlhs;
    VirtualRegister *vrrhs;

    if (lhs->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(lhs.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());
        Assert(cinfo.isflt_, "unexpected");

        vrlhs = curstat_.planner->NewVReg(VREG_TYPE::FLT);

        auto lc_idx = lc_map_.at(cinfo.v32_.u32_);
        auto lbname = std::string(".LC") + std::to_string(lc_idx);

        auto lc_addr = curstat_.planner->NewVReg(VREG_TYPE::PTR);

        auto uop_lla = new UopLla;
        uop_lla->SetSrc(lbname);
        uop_lla->SetDst(lc_addr);

        auto uop_fload = new UopFLoad;
        uop_fload->SetOff(0);
        uop_fload->SetBase(lc_addr);
        uop_fload->SetDst(vrlhs);

        curstat_.cur_blk->Push(uop_lla);
        curstat_.cur_blk->Push(uop_fload);
    } else {
        auto var = dynamic_cast<Variable *>(lhs.get());
        Assert(var, "bad dynamic cast");

        vrlhs = curstat_.planner->GetVReg(var->GetVariableIdx());
    }

    if (rhs->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(rhs.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());
        Assert(cinfo.isflt_, "unexpected");

        vrrhs = curstat_.planner->NewVReg(VREG_TYPE::FLT);

        auto lc_idx = lc_map_.at(cinfo.v32_.u32_);
        auto lbname = std::string(".LC") + std::to_string(lc_idx);

        auto lc_addr = curstat_.planner->NewVReg(VREG_TYPE::PTR);

        auto uop_lla = new UopLla;
        uop_lla->SetSrc(lbname);
        uop_lla->SetDst(lc_addr);

        auto uop_fload = new UopFLoad;
        uop_fload->SetOff(0);
        uop_fload->SetBase(lc_addr);
        uop_fload->SetDst(vrrhs);

        curstat_.cur_blk->Push(uop_lla);
        curstat_.cur_blk->Push(uop_fload);
    } else {
        auto var = dynamic_cast<Variable *>(rhs.get());
        Assert(var, "bad dynamic cast");

        vrrhs = curstat_.planner->GetVReg(var->GetVariableIdx());
    }

    if (vrlhs == nullptr or vrrhs == nullptr) {
        panic("unexpected");
    }

    auto dst = curstat_.planner->AllocVReg(VREG_TYPE::FLT, res->GetVariableIdx());

    auto uop = new UopFBin;
    uop->SetLhs(vrlhs);
    uop->SetRhs(vrrhs);
    uop->SetDst(dst);
    uop->SetKind((FBIN_KIND)opcode);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(SitoFpInst *ll) {
    auto oprand = ll->GetOprand();
    auto var = std::dynamic_pointer_cast<Variable>(oprand);
    Assert(var, "bitcast should op on var");

    auto &&res = ll->GetResult();

    auto rsvr = curstat_.planner->GetVReg(var->GetVariableIdx());
    auto nwvr = curstat_.planner->AllocVReg(VREG_TYPE::FLT, res->GetVariableIdx());

    auto uop = new UopCvtW2S;
    uop->SetSrc(rsvr);
    uop->SetDst(nwvr);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(FptoSiInst *ll) {
    auto oprand = ll->GetOprand();
    auto var = std::dynamic_pointer_cast<Variable>(oprand);
    Assert(var, "bitcast should op on var");

    auto &&res = ll->GetResult();

    auto rsvr = curstat_.planner->GetVReg(var->GetVariableIdx());
    auto nwvr = curstat_.planner->AllocVReg(VREG_TYPE::INT, res->GetVariableIdx());

    auto uop = new UopCvtS2W;
    uop->SetSrc(rsvr);
    uop->SetDst(nwvr);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(ZextInst *ll) {
    auto oprand = ll->GetOprand();
    auto var = std::dynamic_pointer_cast<Variable>(oprand);
    Assert(var, "bitcast should op on var");

    auto &&res = ll->GetResult();

    auto rsvr = curstat_.planner->GetVReg(var->GetVariableIdx());
    auto nwvr = curstat_.planner->AllocVReg(VREG_TYPE::INT, res->GetVariableIdx());

    auto uop = new UopMv;
    uop->SetSrc(rsvr);
    uop->SetDst(nwvr);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(BitCastInst *ll) {
    auto oprand = ll->GetOprand();
    auto var = std::dynamic_pointer_cast<Variable>(oprand);
    Assert(var, "bitcast should op on var");

    auto &&res = ll->GetResult();

    auto rsvr = curstat_.planner->GetVReg(var->GetVariableIdx());
    auto nwvr = curstat_.planner->AllocVReg(VREG_TYPE::INT, res->GetVariableIdx());

    auto uop = new UopMv;
    uop->SetSrc(rsvr);
    uop->SetDst(nwvr);

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(AllocaInst *ll) {
    if (auto &&type = ll->GetAllocaType(); type->IsScalar()) {
        size_t slen = 0;
        if (type->IsPointer()) {
            slen = 8;
        } else if (type->IntType() or type->FloatType()) {
            slen = 4;
        } else {
            panic("unexpected");
        }

        auto stkvr = curstat_.planner->Alloca(ll->GetResult()->GetVariableIdx(), slen);
        auto uop = new UopLla;
        uop->SetDst(stkvr);

        curstat_.cur_blk->Push(uop);
    } else {
        auto lstty = dynamic_cast<ListType *>(type.get());
        Assert(lstty, "bad dynamic cast");

        if (type->IsPointer()) {
            panic("unexpected");
        }

        auto stkvr = curstat_.planner->Alloca(ll->GetResult()->GetVariableIdx(), lstty->GetCapacity() * 4);
        auto uop = new UopLla;
        uop->SetDst(stkvr);

        curstat_.cur_blk->Push(uop);
    }
}

void InternalTranslation::Translate(StoreInst *ll) {
    auto &&s_val = ll->GetStoreValue();
    auto &&s_addr = ll->GetStoreAddr();

    VirtualRegister *srcvr = nullptr;
    bool onflt = false;

    if (s_val->IsVariable()) {
        auto var = dynamic_cast<Variable *>(s_val.get());
        Assert(var, "bad dynamic cast");

        onflt = s_val->GetBaseType()->FloatType();
        srcvr = curstat_.planner->GetVReg(var->GetVariableIdx());
    } else if (s_val->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(s_val.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());

        if (cinfo.v32_.u32_ == 0) {
            srcvr = nullptr;
        } else {
            srcvr = curstat_.planner->NewVReg(VREG_TYPE::INT);

            li(srcvr, cinfo.v32_.u32_);
        }
    }

    if (s_addr->IsGlobalValue()) {
        panic("try another way please");
    } else {
        auto addr = dynamic_cast<Variable *>(s_addr.get());
        Assert(addr, "store addr should be var");

        auto base = curstat_.planner->GetVReg(addr->GetVariableIdx());

        if (onflt) {
            auto uop = new UopFStore;
            uop->SetBase(base);
            uop->SetSrc(srcvr);
            uop->SetOff(0);

            curstat_.cur_blk->Push(uop);
        } else {
            auto uop = new UopStore;
            uop->SetBase(base);
            uop->SetSrc(srcvr);
            uop->SetOff(0);

            curstat_.cur_blk->Push(uop);
        }
    }
}

void InternalTranslation::Translate(LoadInst *ll) {
    auto &&l_dst = ll->GetResult();
    auto &&l_addr = ll->GetOprand();

    VirtualRegister *dstvr = nullptr;
    VREG_TYPE vtype;

    bool onflt = false;
    if (auto type = l_dst->GetBaseType(); type->FloatType()) {
        onflt = true;
        vtype = VREG_TYPE::FLT;
    } else {
        vtype = VREG_TYPE::INT;
    }
    dstvr = curstat_.planner->AllocVReg(vtype, l_dst->GetVariableIdx());

    if (l_addr->IsGlobalValue()) {
        panic("try another way please");
    } else {
        auto addr = dynamic_cast<Variable *>(l_addr.get());
        Assert(addr, "load addr should be var");

        auto base = curstat_.planner->GetVReg(addr->GetVariableIdx());

        if (onflt) {
            auto uop = new UopFLoad;
            uop->SetBase(base);
            uop->SetDst(dstvr);
            uop->SetOff(0);

            curstat_.cur_blk->Push(uop);
        } else {
            auto uop = new UopLoad;
            uop->SetBase(base);
            uop->SetDst(dstvr);
            uop->SetOff(0);

            curstat_.cur_blk->Push(uop);
        }
    }
}

void InternalTranslation::Translate(GetElementPtrInst *ll) {
    auto &&res = ll->GetResult();
    auto resvr = curstat_.planner->AllocVReg(VREG_TYPE::PTR, res->GetVariableIdx());

    VirtualRegister *off = nullptr;
    auto &&offset = ll->GetOffList().back();
    if (offset->IsVariable()) {
        auto var = std::dynamic_pointer_cast<Variable>(offset);
        Assert(var, "bad dynamic cast");

        auto offnum = curstat_.planner->GetVReg(var->GetVariableIdx());
        off = curstat_.planner->NewVReg(VREG_TYPE::PTR);

        auto uop_slli = new UopIBinImm;
        uop_slli->SetLhs(offnum);
        uop_slli->SetImm(2);
        uop_slli->SetDst(off);
        uop_slli->SetKind(IBIN_KIND::SLL);

        curstat_.cur_blk->Push(uop_slli);
    } else if (offset->IsConstant()) {
        auto cst = dynamic_cast<Constant *>(offset.get());
        Assert(cst, "bad dynamic cast");

        auto &&cinfo = XConstValue(cst->GetValue());

        off = curstat_.planner->NewVReg(VREG_TYPE::PTR);

        li(off, cinfo.v32_.u32_ << 2);
    } else {
        panic("unexpected");
    }

    auto &&base = ll->GetBaseAddr();
    if (base->IsGlobalValue()) {
        auto gv = dynamic_cast<GlobalValue *>(base.get());
        Assert(gv, "bad dynamic cast");

        auto gvidx = gv->GetGlobalValueIdx();

        auto glb_addr = curstat_.planner->NewVReg(VREG_TYPE::PTR);
        auto glb_name = std::string(gv_map_.at(gvidx)->Label());
        auto uop_lla = new UopLla;
        uop_lla->SetSrc(glb_name);
        uop_lla->SetDst(glb_addr);

        auto uop_add = new UopIBin;
        uop_add->SetLhs(glb_addr);
        uop_add->SetRhs(off);
        uop_add->SetDst(resvr);
        uop_add->SetKind(IBIN_KIND::ADD);

        curstat_.cur_blk->Push(uop_lla);
        curstat_.cur_blk->Push(uop_add);

    } else if (base->IsVariable()) {
        auto addr = dynamic_cast<Variable *>(base.get());
        Assert(addr, "bad dynamic cast");

        auto vr_addr = curstat_.planner->GetVReg(addr->GetVariableIdx());

        auto uop_add = new UopIBin;
        uop_add->SetLhs(vr_addr);
        uop_add->SetRhs(off);
        uop_add->SetDst(resvr);
        uop_add->SetKind(IBIN_KIND::ADD);

        curstat_.cur_blk->Push(uop_add);

    } else {
        panic("unexpected");
    }
}

void InternalTranslation::Translate(CallInst *ll) {
    curstat_.meetcall = true;

    // decide if is tail call

    auto &&params = ll->GetParamList();
    auto &&callee = ll->GetCalleeFunc();

    auto uop = new UopCall;

    if (callee->GetFuncName() != fptr_->GetFuncName()) {
        rlps_->MeetCallOther();
        uop->SetCallSelf(false);
    } else {
        uop->SetCallSelf(true);
    }

    auto make_param = [&params, &uop, this](size_t num) {
        size_t pcnt = 0;

        size_t ips = 0;
        size_t fps = 0;
        size_t sps = 0;

        extern size_t abi_arg_reg;

        for (auto &&param : params) {
            auto &&ptype = param->GetBaseType();

            if (ptype->FloatType()) {
                if (fps < abi_arg_reg) {
                    fps += 1;
                }
                sps += 1;
            } else {
                if (ips < abi_arg_reg) {
                    ips += 1;
                }
                sps += 1;
            }

            if (param->IsVariable()) {
                auto var = dynamic_cast<Variable *>(param.get());
                Assert(var, "bad dynamic cast");

                auto vr_param = curstat_.planner->GetVReg(var->GetVariableIdx());
                uop->PushParam(vr_param);
            } else if (param->IsConstant()) {
                auto cst = dynamic_cast<Constant *>(param.get());
                Assert(cst, "bad dynamic cast");

                auto &&cinfo = XConstValue(cst->GetValue());

                if (cinfo.isflt_) {
                    auto lc_idx = lc_map_.at(cinfo.v32_.u32_);
                    auto lbname = std::string(".LC") + std::to_string(lc_idx);

                    auto lc_addr = curstat_.planner->NewVReg(VREG_TYPE::PTR);
                    auto vr_param = curstat_.planner->NewVReg(VREG_TYPE::FLT);

                    auto uop_lla = new UopLla;
                    uop_lla->SetSrc(lbname);
                    uop_lla->SetDst(lc_addr);

                    auto uop_fload = new UopFLoad;
                    uop_fload->SetOff(0);
                    uop_fload->SetBase(lc_addr);
                    uop_fload->SetDst(vr_param);

                    curstat_.cur_blk->Push(uop_lla);
                    curstat_.cur_blk->Push(uop_fload);

                    uop->PushParam(vr_param);
                } else {
                    auto vr_param = curstat_.planner->NewVReg(VREG_TYPE::INT);

                    li(vr_param, cinfo.v32_.u32_);

                    uop->PushParam(vr_param);
                }
            } else {
                panic("unexpected");
            }

            pcnt += 1;
            if (pcnt >= num) {
                break;
            }
        }

        curstat_.planner->SetPstkSiz(sps);
    };

    if (callee->IsLibFunction()) {
        bool meet = false;

        uop->SetLibCall(true);

        rlps_->MeetLibCall();

        do /* sylib */ {
            auto sylib_func = dynamic_cast<SYSYLibFunction *>(callee.get());
            if (sylib_func == nullptr) {
                break;
            }
            uop->SetCallee(sylib_func->GetFuncName());

            make_param(-1);

            meet = true;
        } while (0);

        do /* sylib */ {
            auto llvm_func = dynamic_cast<LLVMLibFunction *>(callee.get());
            if (llvm_func == nullptr) {
                break;
            }

            if (llvm_func->GetFuncName() == "llvm.memset.p0i8.i64") {
                auto fnn = std::string("memset");
                uop->SetCallee(fnn);

                make_param(3);

                meet = true;
            }
        } while (0);

        if (not meet) {
            panic("unrecognizable");
        }
    }

    if (not callee->IsLibFunction() and not params.empty()) {
        uop->SetLibCall(false);
        uop->SetCallee(callee->GetFuncName());

        make_param(-1);
    }

    if (auto &&res = ll->GetResult(); res != nullptr) {
        VirtualRegister *retvr;
        VREG_TYPE vtype;
        if (auto btype = res->GetBaseType(); btype->FloatType()) {
            vtype = VREG_TYPE::FLT;
        } else if (btype->IntType()) {
            vtype = VREG_TYPE::INT;
        } else {
            panic("unexpected");
        }
        retvr = curstat_.planner->AllocVReg(vtype, res->GetVariableIdx());
        uop->SetRetVal(retvr);
    }

    curstat_.cur_blk->Push(uop);
}

void InternalTranslation::Translate(CRVC_UNUSE PhiInst *ll) {
    panic("not implemented well");

    // auto &&datalst = ll->GetDataList();
    // for (auto &&[value, cfg] : datalst) {
    //     PhiOperand popd;

    //     if (value->IsVariable()) {
    //         popd.kind = PHI_KIND::REG;

    //         auto var = dynamic_cast<Variable *>(value.get());
    //         Assert(var, "bad dynamic cast");

    //         popd.data = var->GetVariableIdx();
    //         popd.lbidx = cfg->GetBlockIdx();
    //     } else if (value->IsConstant()) {
    //         uint32_t imm = 0;
    //         if (value->GetBaseType()->FloatType()) {
    //             panic("impossible");
    //         } else {
    //             popd.kind = PHI_KIND::IMM;
    //         }

    //         popd.data = imm;
    //         popd.lbidx = cfg->GetBlockIdx();
    //     } else {
    //         panic("unexpected");
    //     }
    // }
}