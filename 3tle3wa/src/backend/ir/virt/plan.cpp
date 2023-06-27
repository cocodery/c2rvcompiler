#include <algorithm>

#include "3tle3wa/backend/ir/bbtype.hh"
#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/ir/virt/register.hh"
#include "3tle3wa/backend/ir/virt/resource.hh"

class ralcor {
   public:
    size_t cur_rinfo[64] = {0};
    bool cur_inuse[64] = {0};

    void set(virt_reg *vr, rid_t rid) {
        cur_inuse[rid] = true;
        cur_rinfo[rid] = vr->value();
    }

    void rls(size_t vidx, size_t ridx) {
        Assert(cur_inuse[ridx] and cur_rinfo[ridx] == vidx, "fail");
        cur_inuse[ridx] = false;
        cur_rinfo[ridx] = 0;
    }
};

void vr_allocor::plan_reg(rl_progress &rlp) {
    // 初步计算 liveout
    for (auto &&bb : rlp.bbs_) {
        for (auto &&uop : bb->ops_) {
            uop->calcu_lvif(bb->dli);
        }
    }

    // 迭代计算 liveout
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto &&bb : rlp.bbs_) {
            auto &&old = bb->dli.live_out;
            std::unordered_set<size_t> nwset;
            for (auto &&m : bb->successer) {
                auto &&succ = rlp.lbmap_.at(m).bbp_;
                Assert(succ, "not generate well");

                auto &&cur = succ->dli;

                for (auto &&v : cur.live_out) {
                    if (cur.var_kill.find(v) == cur.var_kill.end()) {
                        nwset.insert(v);
                    }
                }

                nwset.insert(cur.ue_var.begin(), cur.ue_var.end());
            }
            auto osize = old.size();
            old.insert(nwset.begin(), nwset.end());
            if (osize != old.size()) {
                changed = true;
            }
        }
    }

    // 第一次分配虚拟寄存器
    virt_resource vrsc;
    for (auto &&bb : rlp.bbs_) {
        size_t uop_idx = 1;

        std::unordered_set<virt_reg *> scope_vrg;
        for (auto &&reg : bb->dli.live_out) {
            auto vr = get_reg(reg);
            scope_vrg.insert(vr);
        }

        for (auto &&uop : bb->ops_) {
            uop->set_uop_idx(uop_idx);
            uop->givr(scope_vrg);
            uop_idx += 1;
        }

        for (auto &&reg : scope_vrg) {
            if (reg->vconfirm() and not reg->confirm()) {
                vrsc.set(reg, reg->vregid());
            }
            reg->set_begin(0);
            reg->set_end(uop_idx);
        }

        for (auto &&uop : bb->ops_) {
            uop->live_info();
        }

        using pqelem_t = std::pair<size_t, virt_reg *>;

        std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_begin;
        std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_end;

        for (auto &&reg : scope_vrg) {
            if (not reg->vconfirm() and not reg->confirm()) {
                sort_begin.emplace(reg->begin(), reg);
                sort_end.emplace(reg->end(), reg);
            }
        }

        auto nxtit = bb->ops_.begin();

        // while (not sort_begin.empty() and sort_begin.top().first <= 0) {
        //     auto vr = sort_begin.top().second;
        //     vrsc.alc(vr);
        //     vrsc.access(vr, vr->refs().size());
        //     sort_begin.pop();
        // }

        while (nxtit != bb->ops_.end()) {
            auto curit = nxtit++;
            auto &&op = (*curit);

            auto curpos = op->get_uop_idx();

            while (not sort_end.empty() and sort_end.top().first <= curpos) {
                auto info = sort_end.top().second;
                if (bb->dli.live_out.find(info->value()) == bb->dli.live_out.end()) {
                    vrsc.rls(info);
                }
                sort_end.pop();
            }

            while (not sort_begin.empty() and sort_begin.top().first <= curpos) {
                auto vr = sort_begin.top().second;
                vrsc.alc(vr);
                vrsc.access(vr, vr->refs().size());
                sort_begin.pop();
            }
        }

        vrsc.rlsall();
    }

    const size_t mxparm = 8;

    // 开始分配
    vrsc.alcreal(rlp, std::min(rlp.puse_.i, mxparm), std::min(rlp.puse_.f, mxparm));

    // 然后分配真实寄存器
    for (auto &&reginfo : vr_map_) {
        auto &&reg = reginfo.second;
        if (reg->kind() == VREG_KIND::PRM) {
            continue;
        }

        auto vidx = reg->vregid();

        if (auto rrid = vrsc.rregs_.at(vidx); rrid != (size_t)-1) {
            reg->set_onstk(false);
            reg->set_confirm(true);
            reg->set_rregid(rrid);
        } else {
            auto stkinfo = vrsc.stks_.at(vidx);
            reg->set_onstk(true);
            reg->set_confirm(true);
            reg->set_sinfo(stkinfo);
            reg->set_rregid(riscv::zero);
            if (reg->type() == VREG_TYPE::PTR) {
                reg->stkpp_ = true;
            }
        }
    }

    // 在加载参数之前定一个锚点
    // 未来用于保存寄存器信息
    std::list<std::unique_ptr<uop_general>>::iterator anchor;
    bool set_anchor = false;

    // 复用保存栈
    std::unordered_map<size_t, virt_reg *> ctx;

    // 开始考虑状态
    for (auto &&bb : rlp.bbs_) {
        // 本地寄存器分配
        ralcor nst_alcr;
        std::unordered_set<size_t> inouts;

        size_t uop_idx = 1;
        std::unordered_set<virt_reg *> scope_vrg;
        for (auto &&out : bb->dli.live_out) {
            inouts.insert(out);
            auto vr = get_reg(out);
            scope_vrg.insert(vr);
        }

        for (auto &&brj : rlp.lbmap_.at(bb->get_lbid()).refs_) {
            auto fa = brj->get_fa_idx();
            for (auto &&in: rlp.lbmap_.at(fa).bbp_->dli.live_out) {
                inouts.insert(in);
                auto vr = get_reg(in);
                scope_vrg.insert(vr);
            }
        }

        for (auto &&uop : bb->ops_) {
            uop->set_uop_idx(uop_idx);
            uop->givr(scope_vrg);
            uop_idx += 1;
        }

        for (auto &&reg : scope_vrg) {
            if (reg->confirm() and not reg->onstk()) {
                nst_alcr.set(reg, reg->rregid());
            }

            reg->set_begin(0);
            reg->set_end(uop_idx);
        }

        for (auto &&reg : rlp.params_) {
            nst_alcr.set(reg, reg->rregid());
        }

        for (auto &&uop : bb->ops_) {
            uop->live_info();
        }

        using pqelem_t = std::pair<size_t, virt_reg *>;

        std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_begin;
        std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_end;

        for (auto &&reg : scope_vrg) {
            if (inouts.find(reg->value()) == inouts.end()) {
                nst_alcr.rls(reg->value(), reg->rregid());
                sort_begin.emplace(reg->begin(), reg);
                sort_end.emplace(reg->end(), reg);
            } else if (reg->begin() != 0) {
                nst_alcr.rls(reg->value(), reg->rregid());
                sort_begin.emplace(reg->begin(), reg);
                sort_end.emplace(reg->end(), reg);
            }
        }

        auto nxtit = bb->ops_.begin();
        bool r_edit[64] = {0};

        while (nxtit != bb->ops_.end()) {
            auto curit = nxtit++;
            auto &&op = (*curit);

            auto curpos = op->get_uop_idx();

            while (not sort_end.empty() and sort_end.top().first <= curpos) {
                auto info = sort_end.top().second;
                if (bb->dli.live_out.find(info->value()) == bb->dli.live_out.end() and not info->onstk()) {
                    nst_alcr.rls(info->value(), info->rregid());
                }
                sort_end.pop();
            }

            if (auto fprm = dynamic_cast<uop_set_fparam *>(op.get()); fprm != nullptr) {
                auto rs = fprm->get_rs();
                if (rs->confirm() and (rs->rregid() - riscv::fa0) < 8) {
                    if (true == r_edit[rs->rregid()]) {
                        size_t i = rs->rregid();

                        virt_reg *stk = nullptr;
                        if (auto fnd = ctx.find(i); fnd == ctx.end()) {
                            stk = alloc_stk(VREG_TYPE::PTR, 8);
                            ctx[i] = stk;
                        } else {
                            stk = fnd->second;
                        }

                        fprm->set_rs(stk);
                    }

                    r_edit[fprm->get_idx() + riscv::fa0] = true;
                }
                if (not set_anchor) anchor = curit;

                set_anchor = true;
            }

            if (auto iprm = dynamic_cast<uop_set_iparam *>(op.get()); iprm != nullptr) {
                auto rs = iprm->get_rs();
                if (rs->confirm() and (rs->rregid() - riscv::a0) < 8) {
                    if (true == r_edit[rs->rregid()]) {
                        size_t i = rs->rregid();

                        virt_reg *stk = nullptr;
                        if (auto fnd = ctx.find(i); fnd == ctx.end()) {
                            stk = alloc_stk(VREG_TYPE::PTR, 8);
                            ctx[i] = stk;
                        } else {
                            stk = fnd->second;
                        }

                        iprm->set_rs(stk);
                    }

                    r_edit[iprm->get_idx() + riscv::a0] = true;
                }
                if (not set_anchor) anchor = curit;

                set_anchor = true;
            }

            if (set_anchor and dynamic_cast<uop_ret *>(op.get())) {
                set_anchor = false;
            }

            const size_t len = 64;

            if (auto callinst = dynamic_cast<uop_call *>(op.get()); callinst != nullptr) {
                if (set_anchor) {
                    set_anchor = false;
                } else {
                    anchor = curit;
                }
                memset(r_edit, 0, 64 * sizeof(bool));

                for (size_t i = 1; i < len; ++i) {
                    if (nst_alcr.cur_inuse[i]) {
                        virt_reg *stk = nullptr;
                        if (auto fnd = ctx.find(i); fnd == ctx.end()) {
                            stk = alloc_stk(VREG_TYPE::PTR, 8);
                            ctx[i] = stk;
                        } else {
                            stk = fnd->second;
                        }

                        auto nwst = std::make_unique<uop_st_stk>();
                        nwst->set_rb(stk);
                        nwst->set_rd(i);

                        anchor = bb->ops_.insert(anchor, std::move(nwst));
                    }
                }

                for (size_t i = 1; i < len; ++i) {
                    if (nst_alcr.cur_inuse[i]) {
                        virt_reg *stk = nullptr;
                        if (auto fnd = ctx.find(i); fnd == ctx.end()) {
                            panic("unexpected");
                        } else {
                            stk = fnd->second;
                        }

                        auto nwld = std::make_unique<uop_ld_stk>();
                        nwld->set_rb(stk);
                        nwld->set_rd(i);

                        nxtit = bb->ops_.insert(nxtit, std::move(nwld));
                    }
                }
                if (auto retval = callinst->get_retval(); retval != nullptr) {
                    auto getres = std::make_unique<uop_mv>();
                    if (retval->type() == VREG_TYPE::FLT) {
                        getres->set_rs(rlp.valc_.alloc_spec(riscv::fa0));
                    } else {
                        getres->set_rs(rlp.valc_.alloc_spec(riscv::a0));
                    }
                    getres->set_rd(callinst->get_retval());
                    nxtit = bb->ops_.insert(nxtit, std::move(getres));
                }
            }

            while (not sort_begin.empty() and sort_begin.top().first <= curpos) {
                auto vr = sort_begin.top().second;
                if (not vr->onstk()) {
                    nst_alcr.set(vr, vr->rregid());
                }
                sort_begin.pop();
            }
        }
    }
}

void vr_allocor::plan_stack() {
    std::vector<stk_info *> sclrstk;
    size_t sclr_total = 0;

    std::vector<stk_info *> arrystk;
    size_t arry_total = 0;

    for (auto &&pair : stk_map_) {
        auto &&ptr = pair.second->sinfo();
        if (ptr == nullptr) {
            panic("unexpected");
        }
        if (pair.second->type() == VREG_TYPE::ARR) {
            arrystk.push_back(ptr.get());
            arry_total += ptr->slen() * 4;
        } else {
            sclrstk.push_back(ptr.get());
            sclr_total += ptr->slen();
        }
    }

    total_stk_len = round_up(16, sclr_total) + round_up(16, arry_total) + ex_argl * 8 + 16;

    int64_t off = 16;

    for (auto &&stkinfo : sclrstk) {
        auto &&len = stkinfo->slen();
        off += len;
        stkinfo->set_off(-off);
    }

    off = 16 + round_up(16, sclr_total);

    for (auto &&stkinfo : arrystk) {
        auto &&len = stkinfo->slen() * 4;
        off += len;
        stkinfo->set_off(-off);
    }
}
