#include <algorithm>

#include "3tle3wa/backend/ir/bbtype.hh"
#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/ir/virt/register.hh"

struct rpack {
    size_t *cur_rinfo;
    bool *cur_inuse;
};

class ralcor {
    size_t cur_rinfo[64] = {0};
    bool cur_inuse[64] = {0};

   public:
    rpack curinfo() { return {cur_rinfo, cur_inuse}; }

    void set(virt_reg *vr, rid_t rid) {
        cur_inuse[rid] = true;
        cur_rinfo[rid] = vr->value();
    }

    bool try_alc(virt_reg *vr) {
        static constexpr size_t i_gpr[] = {
            riscv::s1, riscv::s2,  riscv::s3,  riscv::s4, riscv::s5, riscv::s6, riscv::s7, riscv::s8,
            riscv::s9, riscv::s10, riscv::s11, riscv::t3, riscv::t4, riscv::t5, riscv::t6,
        };

        constexpr size_t i_gpr_len = sizeof(i_gpr) / sizeof(*i_gpr);

        static constexpr size_t f_gpr[] = {
            riscv::ft0, riscv::ft1,  riscv::ft2,  riscv::ft3, riscv::ft4, riscv::ft5,  riscv::ft6,
            riscv::ft7, riscv::fs3,  riscv::fs4,  riscv::fs5, riscv::fs6, riscv::fs7,  riscv::fs8,
            riscv::fs9, riscv::fs10, riscv::fs11, riscv::ft8, riscv::ft9, riscv::ft10, riscv::ft11,

            // riscv::fs2,
        };

        constexpr size_t f_gpr_len = sizeof(f_gpr) / sizeof(*f_gpr);

        if (vr->type() == VREG_TYPE::FLT) {
            for (size_t f = 0; f < f_gpr_len; ++f) {
                if (not cur_inuse[f_gpr[f]]) {
                    cur_inuse[f_gpr[f]] = true;
                    cur_rinfo[f_gpr[f]] = vr->value();
                    vr->set_onstk(false);

                    vr->set_confirm(true);
                    vr->set_rregid(f_gpr[f]);
                    return true;
                }
            }
        } else {
            for (size_t i = 0; i < i_gpr_len; ++i) {
                if (not cur_inuse[i_gpr[i]]) {
                    cur_inuse[i_gpr[i]] = true;
                    cur_rinfo[i_gpr[i]] = vr->value();
                    vr->set_onstk(false);

                    vr->set_confirm(true);
                    vr->set_rregid(i_gpr[i]);
                    return true;
                }
            }
        }

        return false;
    }

    void rls(size_t vidx) {
        for (size_t i = 0; i < 64; ++i) {
            if (cur_rinfo[i] == vidx) {
                cur_inuse[i] = false;
            }
        }
    }
};

void vr_allocor::plan_reg(rl_progress &rlp) {
    std::unordered_map<size_t, virt_reg *> ctx;

    for (auto &&bb : rlp.bbs_) {
        for (auto &&uop : bb->ops_) {
            uop->calcu_lvif(bb->dli);
        }
    }

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

            for (auto &&v : nwset) {
                if (old.find(v) == old.end()) {
                    changed = true;
                    old.insert(v);
                }
            }
        }
    }

    std::unordered_map<size_t, size_t> occupy_map;
    std::list<std::unique_ptr<uop_general>>::iterator anchor;
    bool set_anchor = false;

    for (auto &&bb : rlp.bbs_) {
        ralcor nst_alcr;

        size_t uop_idx = 1;
        std::unordered_set<virt_reg *> scope_vrg;
        for (auto &&reg : bb->dli.live_out) {
            auto vr = get_reg(reg);
            scope_vrg.insert(vr);
            if (vr->confirm()) nst_alcr.set(vr, vr->rregid());
        }

        for (auto &&uop : bb->ops_) {
            uop->set_uop_idx(uop_idx);
            uop->givr(scope_vrg);
            uop_idx += 1;
        }

        for (auto &&reg : scope_vrg) {
            if (reg->confirm()) nst_alcr.set(reg, reg->rregid());
            reg->set_begin(1);
        }

        for (auto &&reg : vr_map_) {
            if (reg.second->kind() == VREG_KIND::PRM) {
                if (reg.second->value() < 8) {
                    if (reg.second->type() == VREG_TYPE::FLT) {
                        nst_alcr.set(reg.second, reg.second->rregid());
                    } else if (reg.second->type() == VREG_TYPE::INT or reg.second->type() == VREG_TYPE::PTR) {
                        nst_alcr.set(reg.second, reg.second->rregid());
                    } else {
                        panic("unexpected");
                    }
                }
            }
        }

        for (auto &&uop : bb->ops_) {
            uop->live_info();
        }

        using pqelem_t = std::pair<size_t, virt_reg *>;

        std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_begin;
        std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_end;

        for (auto &&reg : scope_vrg) {
            if (not reg->confirm()) {
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
                if (bb->dli.live_out.find(info->value()) == bb->dli.live_out.end()) nst_alcr.rls(info->value());
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

            if (set_anchor && dynamic_cast<uop_ret *>(op.get())) {
                set_anchor = false;
            }

            auto &&cipk = nst_alcr.curinfo();
            const size_t len = 64;

            if (dynamic_cast<uop_call *>(op.get())) {
                if (set_anchor) {
                    set_anchor = false;
                } else {
                    anchor = curit;
                }
                memset(r_edit, 0, 64 * sizeof(bool));

                for (size_t i = 0; i < len; ++i) {
                    if (cipk.cur_inuse[i]) {
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

                        bb->ops_.insert(anchor, std::move(nwst));
                    }
                }
                for (size_t i = 0; i < len; ++i) {
                    if (cipk.cur_inuse[i]) {
                        virt_reg *stk = nullptr;
                        if (auto fnd = ctx.find(i); fnd == ctx.end()) {
                            panic("unexpected");
                        } else {
                            stk = fnd->second;
                        }

                        auto nwld = std::make_unique<uop_ld_stk>();
                        nwld->set_rb(stk);
                        nwld->set_rd(i);

                        bb->ops_.insert(nxtit, std::move(nwld));
                    }
                }
            }

            while (not sort_begin.empty() and sort_begin.top().first <= curpos) {
                auto vr = sort_begin.top().second;
                auto res = nst_alcr.try_alc(vr);

                if (not res) {
                    vr->set_onstk(true);

                    auto sptr = std::make_unique<stk_info>();
                    auto sraw = sptr.get();

                    switch (vr->type()) {
                        case VREG_TYPE::FLT:
                        case VREG_TYPE::INT:
                            sptr->set_slen(8);
                            break;
                        case VREG_TYPE::PTR:
                            vr->stkpp_ = true;
                            sptr->set_slen(8);
                            break;
                        default:
                            panic("unexpected");
                    }

                    vr->set_sinfo(sptr);
                    vr->set_confirm(true);
                    vr->set_rregid(riscv::zero);

                    stk_map_[(uxlen_t)sraw] = vr;
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
