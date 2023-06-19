#include "rlvtype.hh"

#include <vector>

#include "../../../debug/Logs.hh"
#include "../riscv/riscv-cst.hh"
#include "../../utils.hh"

virt_reg *virt_reg_allocor::alloc(VREG_KIND kind, width_t len, uxlen_t value) {
    if (kind == VREG_KIND::LOC) {
        auto fnd = fcstmap.find(value);
        if (fnd == fcstmap.end()) {
            auto ptr = std::make_unique<virt_reg>();
            ptr->kind = kind;
            ptr->len = len;
            ptr->value = value;
            storage.push_back(std::move(ptr));

            auto bptr = storage.back().get();
            fcstmap[value] = bptr;

            return bptr;
        }
        return fnd->second;
    } else if (kind == VREG_KIND::REG || kind == VREG_KIND::PRM) {
        auto fnd = map.find(value);
        if (fnd == map.end()) {
            if (nwidx_alloc < value) {
                nwidx_alloc = value + 1;
            }
            auto ptr = std::make_unique<virt_reg>();
            ptr->kind = kind;
            ptr->len = len;
            ptr->value = value;
            storage.push_back(std::move(ptr));

            auto bptr = storage.back().get();
            map[value] = bptr;

            return bptr;
        }
        return fnd->second;
    } else if (kind == VREG_KIND::FP) {
        static std::unique_ptr<virt_reg> fp(new virt_reg{VREG_KIND::FP, .inrreg = riscv::fp});
        return fp.get();
    } else if (kind == VREG_KIND::SP) {
        static std::unique_ptr<virt_reg> sp(new virt_reg{VREG_KIND::SP, .inrreg = riscv::sp});
        return sp.get();
    } else if (kind == VREG_KIND::RA) {
        static std::unique_ptr<virt_reg> ra(new virt_reg{VREG_KIND::RA, .inrreg = riscv::ra});
        return ra.get();
    } else if (kind == VREG_KIND::IMM) {
        static std::unique_ptr<virt_reg> zero(new virt_reg{VREG_KIND::ZERO, .inrreg = riscv::zero});
        if (value == 0) {
            return zero.get();
        } else {
            auto ptr = std::make_unique<virt_reg>();
            ptr->kind = kind;
            ptr->len = len;
            ptr->value = value;
            storage.push_back(std::move(ptr));
            return storage.back().get();
        }
    } else {
        auto ptr = std::make_unique<virt_reg>();
        ptr->kind = kind;
        ptr->len = len;
        ptr->value = value;
        storage.push_back(std::move(ptr));
        return storage.back().get();
    }

    panic("unreachable");
}

virt_reg *virt_reg_allocor::getREG(uuid_t uid) {
    auto fnd = map.find(uid);
    if (fnd == map.end()) {
        panic("miss");
    }
    return fnd->second;
}

virt_reg *virt_reg_allocor::allocREG(width_t len) {
    auto nwidx = nwidx_alloc++;
    return alloc(VREG_KIND::REG, len, nwidx);
}

void virt_reg_allocor::link(uuid_t lhs, uuid_t rhs) { map[lhs] = map.at(rhs); }

uuid_t virt_stkinf_allocor::alloc(VSTK_KIND kind, width_t len) {
    auto stkinfo = std::make_unique<virt_stkinf>();
    stkinfo->kind = kind;
    stkinfo->len = len;
    storage.push_back(std::move(stkinfo));
    auto bptr = storage.back().get();
    map[(intptr_t)bptr] = bptr;
    return (intptr_t)bptr;
}

virt_stkinf *virt_stkinf_allocor::getSTK(uuid_t uid) {
    auto fnd = map.find(uid);
    if (fnd == map.end()) {
        panic("miss");
    }
    return fnd->second;
}

void virt_stkinf_allocor::plan(size_t ex_argl) {
    std::vector<virt_stkinf *> sclrstk;
    size_t sclr_total = 0;
    std::vector<virt_stkinf *> arrystk;
    size_t arry_total = 0;

    for (auto &&ptr : storage) {
        if (ptr->kind == VSTK_KIND::ARR) {
            arrystk.push_back(ptr.get());
            arry_total += ptr->len;
        } else {
            sclrstk.push_back(ptr.get());
            sclr_total += ptr->len;
        }
    }

    total_stk_len = ROUNDUP(16, sclr_total) + ROUNDUP(16, arry_total) + ex_argl + 16;

    //
    // hi                                                                                 lo
    // +----+----+----+----+---------------------------+-------------------------+----+----+
    // | ax | a8 | ra | fp |  for scalar aligned by 16 | for array aligned by 16 | ax | a8 |
    // +----+----+----+----+---------------------------+-------------------------+----+----+
    //           ^ fp                                                                      ^ sp
    //
    // stack plan like this
    // assuming scalar var use offset index more
    // and array use reg index more
    //
    off64_t off = 16;

    for (auto &&stkinfo : sclrstk) {
        auto &&len = stkinfo->len;
        off += len;
        stkinfo->off = -off;
    }

    off = 16 + ROUNDUP(16, sclr_total);

    for (auto &&stkinfo : arrystk) {
        auto &&len = stkinfo->len;
        off += len;
        stkinfo->off = -off;
    }
}