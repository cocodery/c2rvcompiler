#include <queue>
#include <vector>

#include "../../asm/riscv/def.hh"
#include "../../utils.hh"
#include "Logs.hh"
#include "register.hh"

class ralcor {
    size_t cur_rinfo[64] = {0};
    bool cur_inuse[64] = {0};

   public:
    bool try_alc(virt_reg *vr) {
        static constexpr size_t i_gpr[] = {
            riscv::s1, riscv::s2, riscv::s3,  riscv::s4,  riscv::s5, riscv::s6, riscv::s7, riscv::s8,
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

void vr_allocor::plan_reg() {
    ralcor alcer;

    size_t curpos = 0;

    using pqelem_t = std::pair<size_t, virt_reg *>;

    std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_begin;
    std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_end;

    for (auto &&pair : vr_map_) {
        auto &&reg = pair.second;
        if (not reg->confirm()) {
            sort_begin.emplace(reg->begin(), reg);
            sort_end.emplace(reg->end(), reg);
        }
    }

    while (not sort_begin.empty() or not sort_end.empty()) {
        while (not sort_end.empty() and sort_end.top().first <= curpos) {
            auto info = sort_end.top().second;
            alcer.rls(info->value());
            sort_end.pop();
        }

        while (not sort_begin.empty() and sort_begin.top().first <= curpos) {
            auto vr = sort_begin.top().second;
            auto res = alcer.try_alc(vr);

            if (not res) {
                vr->set_onstk(true);

                auto sptr = std::make_unique<stk_info>();
                auto sraw = sptr.get();

                switch (vr->type()) {
                    case VREG_TYPE::FLT:
                    case VREG_TYPE::INT:
                        sptr->set_slen(4);
                        break;
                    case VREG_TYPE::PTR:
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

        curpos += 1;
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

    off64_t off = 16;

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
