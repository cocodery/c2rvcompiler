#include "3tle3wa/backend/ir/virt/resource.hh"

#include "3tle3wa/backend/ir/bbtype.hh"

void virt_resource::set(virt_reg *vr, rid_t vrid) {
    if (vr->type() == VREG_TYPE::FLT) {
        fvrsc_map_[vrid] = vr->value();
        ffree_.erase(vrid);
    } else {
        vrsc_map_[vrid] = vr->value();
        ifree_.erase(vrid);
    }
}

void virt_resource::alc(virt_reg *vr) {
    if (vr->type() == VREG_TYPE::FLT) {
        size_t alc_idx = 0;
        if (ffree_.empty()) {
            idx_ += 1;
            falloced_.insert(idx_);
            alc_idx = idx_;
            hit_map_[idx_] = 1;
        } else {
            alc_idx = *ffree_.begin();
            ffree_.erase(ffree_.begin());
            hit_map_[alc_idx] += 1;
        }

        fvrsc_map_[alc_idx] = vr->value();

        vr->set_vregid(alc_idx);
    } else {
        size_t alc_idx = 0;
        if (ifree_.empty()) {
            idx_ += 1;
            ialloced_.insert(idx_);
            alc_idx = idx_;
            hit_map_[idx_] = 1;
        } else {
            alc_idx = *ifree_.begin();
            ifree_.erase(ifree_.begin());
            hit_map_[alc_idx] += 1;
        }

        vrsc_map_[alc_idx] = vr->value();

        vr->set_vregid(alc_idx);
    }

    vr->set_vconfirm(true);
    idx_map_[vr->value()] = vr->vregid();
}

void virt_resource::rls(virt_reg *vr) {
    auto idx = idx_map_.at(vr->value());
    if (vr->type() == VREG_TYPE::FLT) {
        ffree_.insert(idx);
    } else {
        ifree_.insert(idx);
    }
}

void virt_resource::access(virt_reg *vr, size_t n) {
    Assert(vr->vconfirm(), "only on vreg");
    hit_map_[vr->vregid()] += n;
}

void virt_resource::rlsall() {
    for (auto &&pi : vrsc_map_) {
        ifree_.insert(pi.first);
    }
    vrsc_map_.clear();
    for (auto &&pi : fvrsc_map_) {
        ffree_.insert(pi.first);
    }
    fvrsc_map_.clear();
}

void virt_resource::alcreal(rl_progress &rlp, size_t iuse, size_t fuse) {
    static constexpr size_t i_gpr[] = {
        riscv::a0, riscv::a1,  riscv::a2,  riscv::a3, riscv::a4, riscv::a5, riscv::a6, riscv::a7,
        riscv::s1, riscv::s2,  riscv::s3,  riscv::s4, riscv::s5, riscv::s6, riscv::s7, riscv::s8,
        riscv::s9, riscv::s10, riscv::s11, riscv::t3, riscv::t4, riscv::t5, riscv::t6,
    };

    constexpr size_t i_gpr_len = sizeof(i_gpr) / sizeof(*i_gpr);

    static constexpr size_t f_gpr[] = {
        riscv::fa0,  riscv::fa1, riscv::fa2, riscv::fa3,  riscv::fa4,  riscv::fa5, riscv::fa6, riscv::fa7,
        riscv::ft0,  riscv::ft1, riscv::ft2, riscv::ft3,  riscv::ft4,  riscv::ft5, riscv::ft6, riscv::ft7,
        riscv::fs3,  riscv::fs4, riscv::fs5, riscv::fs6,  riscv::fs7,  riscv::fs8, riscv::fs9, riscv::fs10,
        riscv::fs11, riscv::ft8, riscv::ft9, riscv::ft10, riscv::ft11,
    };

    constexpr size_t f_gpr_len = sizeof(f_gpr) / sizeof(*f_gpr);

    // 给所有的参数寄存器指定位置

    for (auto &&reg : rlp.params_) {
        if (reg->onstk()) {
            continue;
        }
        auto vidx = reg->vregid();
        if (reg->type() == VREG_TYPE::FLT) {
            falloced_.erase(vidx);
        } else {
            ialloced_.erase(vidx);
        }
        rregs_[vidx] = reg->rregid();
    }

    struct {
        size_t i;
        size_t f;
    } pa = {iuse, fuse};

    using pr = std::pair<size_t, size_t>;
    std::priority_queue<pr> pq;

    for (auto &&vregidx : ialloced_) {
        pq.push(pr{hit_map_[vregidx], vregidx});
    }

    size_t rridx = pa.i;
    while (not pq.empty()) {
        auto &&elem = pq.top();
        if (rridx < i_gpr_len) {
            rregs_[elem.second] = i_gpr[rridx];
            rridx += 1;
            pq.pop();
            continue;
        }
        rregs_[elem.second] = (size_t)-1;

        auto sptr = rlp.valc_.alloc_stk(VREG_TYPE::INT, 8);
        stks_[elem.second] = sptr->sinfo();
        pq.pop();
    }

    for (auto &&vregidx : falloced_) {
        pq.push(pr{hit_map_[vregidx], vregidx});
    }

    rridx = pa.f;
    while (not pq.empty()) {
        auto &&elem = pq.top();
        if (rridx < f_gpr_len) {
            rregs_[elem.second] = f_gpr[rridx];
            rridx += 1;
            pq.pop();
            continue;
        }
        rregs_[elem.second] = (size_t)-1;

        auto sptr = rlp.valc_.alloc_stk(VREG_TYPE::FLT, 8);
        stks_[elem.second] = sptr->sinfo();
        pq.pop();
    }
}