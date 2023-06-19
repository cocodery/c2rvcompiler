#include <queue>

#include "../riscv/riscv-asm.hh"
#include "../riscv/riscv-cst.hh"
#include "rlvtype.hh"

struct rreg_info {
    bool inuse = false;
};

struct vreg_info {
    bool instk = false;
    union {
        uuid_t stkid;
        rid_t rid;
    };
};

/**
 * for integer registers
 *
 * ra, sp, gp, tp, fp for spec purpose
 *
 * t0 for li
 * t1 for la
 * t2 for sw/sd/flw/fsw with tag
 * a0 for retval
 * a0 - a7 for arg
 * s0, s1 for tmp l/s
 *
 * other 14 reg for common use
 *
 * for float registers
 *
 * fa0 for retval
 * fa0 - fa7 for arg
 * fs0, fs1 for tmp l/s
 * other 22 reg for common use
 *
 * for now we just use static way to assign regs
 * for better assignment, some regs that won't be use or free in some interval
 * could be assign to some virtual regs
 */
void virt_reg_allocor::regassign(virt_stkinf_allocor &vsaor, bool nm) {
    size_t curpos = 0;

    static constexpr size_t i_gpr[] = {
        riscv::s2, riscv::s3,  riscv::s4,  riscv::s5, riscv::s6, riscv::s7, riscv::s8,
        riscv::s9, riscv::s10, riscv::s11, riscv::t3, riscv::t4, riscv::t5, riscv::t6,
    };

    constexpr size_t i_gpr_len = sizeof(i_gpr) / sizeof(*i_gpr);

    static constexpr size_t f_gpr[] = {
        riscv::ft0,  riscv::ft1,  riscv::ft2, riscv::ft3, riscv::ft4,  riscv::ft5,  riscv::ft6, riscv::ft7,
        riscv::fs2,  riscv::fs3,  riscv::fs4, riscv::fs5, riscv::fs6,  riscv::fs7,  riscv::fs8, riscv::fs9,
        riscv::fs10, riscv::fs11, riscv::ft8, riscv::ft9, riscv::ft10, riscv::ft11,
    };

    constexpr size_t f_gpr_len = sizeof(f_gpr) / sizeof(*f_gpr);

    rreg_info cur_rreg_info[64] = {0};

    using pqelem_t = std::pair<size_t, virt_reg *>;

    std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_begin;
    std::priority_queue<pqelem_t, std::vector<pqelem_t>, std::greater<>> sort_end;

    std::unordered_map<size_t, vreg_info> plan;

    for (auto &&ptr : storage) {
        if (ptr->inrreg == 0 && ptr->kind == VREG_KIND::REG) {
            sort_begin.push(pqelem_t(ptr->begin, ptr.get()));
            sort_end.push(pqelem_t(ptr->end, ptr.get()));
        }
    }

    while (!sort_begin.empty() || !sort_end.empty()) {
        while (!sort_end.empty() && sort_end.top().first <= curpos) {
            auto info = sort_end.top().second;
            auto &&curinfo = plan.at(info->value);
            if (curinfo.instk == false) {
                cur_rreg_info[curinfo.rid].inuse = false;
            }
            sort_end.pop();
        }
        while (!sort_begin.empty() && sort_begin.top().first <= curpos) {
            auto info = sort_begin.top().second;
            if (info->isflt) {
                size_t goth;
                for (goth = 0; goth < f_gpr_len; ++goth) {
                    auto &&currid = f_gpr[goth];
                    if (cur_rreg_info[currid].inuse == false) {
                        cur_rreg_info[currid].inuse = true;
                        if (nm) info->inrreg = currid;
                        plan.emplace(info->value, vreg_info{.instk = false, .rid = currid});
                        break;
                    }
                }
                if (goth == f_gpr_len) {
                    auto uuid = vsaor.alloc(VSTK_KIND::VAL, info->len);
                    info->kind = VREG_KIND::STK;
                    info->value = uuid;
                    plan.emplace(info->value, vreg_info{.instk = true, .rid = uuid});
                }
            } else {
                size_t goth;
                for (goth = 0; goth < i_gpr_len; ++goth) {
                    auto &&currid = i_gpr[goth];
                    if (cur_rreg_info[currid].inuse == false) {
                        cur_rreg_info[currid].inuse = true;
                        if (nm) info->inrreg = currid;
                        plan.emplace(info->value, vreg_info{.instk = false, .rid = currid});
                        break;
                    }
                }
                if (goth == i_gpr_len) {
                    uuid_t uuid;
                    if (info->len == 4 || info->len == 0) {
                        uuid = vsaor.alloc(VSTK_KIND::VAL, info->len);
                    } else {
                        uuid = vsaor.alloc(VSTK_KIND::PTR, info->len);
                    }
                    info->kind = VREG_KIND::STK;
                    info->value = uuid;
                    plan.emplace(info->value, vreg_info{.instk = true, .rid = uuid});
                }
            }
            sort_begin.pop();
        }

        curpos += 1;
    }
}