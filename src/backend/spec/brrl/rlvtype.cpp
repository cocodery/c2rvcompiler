#include "rlvtype.hh"

#include "../../../debug/Logs.hh"

std::string virt_reg::to_string() {
    std::stringstream ss;
    if (kind == VREG_KIND::LOC) {
        ss << "f:" << value;
    } else if (kind == VREG_KIND::IMM) {
        ss << value;
    } else if (kind == VREG_KIND::REG || kind == VREG_KIND::PRM) {
        ss << "r" << value;
    } else if (kind == VREG_KIND::PRM) {
        ss << "p" << value;
    } else if (kind == VREG_KIND::STK) {
        ss << "sp:" << std::hex << value;
    } else if (kind == VREG_KIND::ZERO) {
        ss << "zero";
    } else if (kind == VREG_KIND::FP) {
        ss << "fp";
    } else if (kind == VREG_KIND::SP) {
        ss << "sp";
    } else if (kind == VREG_KIND::RA) {
        ss << "ra";
    }
    return ss.str();
}

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
        static std::unique_ptr<virt_reg> fp(new virt_reg{VREG_KIND::FP});
        return fp.get();
    } else if (kind == VREG_KIND::SP) {
        static std::unique_ptr<virt_reg> sp(new virt_reg{VREG_KIND::SP});
        return sp.get();
    } else if (kind == VREG_KIND::RA) {
        static std::unique_ptr<virt_reg> ra(new virt_reg{VREG_KIND::RA});
        return ra.get();
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