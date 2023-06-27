#include "3tle3wa/backend/asm/env.hh"
#include "3tle3wa/backend/asm/progress.hh"
#include "3tle3wa/backend/asm/riscv/asm.hh"

void asm_env::do_optimize() {
    for (auto &&pg : pgrs_) {
        pg->do_optimize();
    }
}

void progress::do_optimize() {
    for (auto &&pb : pblks_) {
        pb->do_optimize();
    }
}

void pblock::do_optimize() {
    opm_rm_needless_ls();
    opm_rm_needless_li();
}

void pblock::opm_rm_needless_li() {
    auto instit = insts_.begin();

    bool online = false;
    i64 curint = 0;

    while (instit != insts_.end()) {
        auto &&inst = *instit;
        auto &&bear_ptr = inst.get();

        if (auto li_inst = dynamic_cast<rv_li *>(bear_ptr); li_inst != nullptr and li_inst->rd_ == riscv::t0) {
            if (online and curint == li_inst->imm_) {
                instit = insts_.erase(instit);
                continue;
            }
            online = true;
            curint = li_inst->imm_;
        } else if (auto call_inst = dynamic_cast<rv_call *>(bear_ptr); call_inst != nullptr) {
            online = false;
        } else if (auto rvinst = dynamic_cast<rv_inst *>(bear_ptr); rvinst != nullptr and rvinst->rd_ == riscv::t0) {
            online = false;
        }

        instit++;
    }
}

void pblock::opm_rm_needless_ls() {
    constexpr bool LS_LOAD = true;
    constexpr bool LS_STORE = false;

    struct lsinfo {
        rid_t rd_;
        i64 off_;
        u64 width_;

        // true for load
        // false for store
        bool ls_;
        std::list<std::unique_ptr<asm_inst>>::iterator it_;
    };

    std::unordered_map<size_t, lsinfo> lsmap;

    auto instit = insts_.begin();

    while (instit != insts_.end()) {
        auto &&inst = *instit;
        auto &&bear_ptr = inst.get();
        bool meet = false;
        bool reduce = false;

        do /* check */ {
            lsinfo status;

            if (auto lw_inst = dynamic_cast<rv_lw *>(bear_ptr); lw_inst != nullptr) {
                if (lw_inst->rs_ != riscv::fp) {
                    // 不在这里解决
                    break;
                }

                meet = true;
                status.ls_ = LS_LOAD;
                status.width_ = 4;
                status.it_ = instit;
                status.off_ = lw_inst->off_;
                status.rd_ = lw_inst->rd_;
            }

            if (meet) {
                if (auto fnd = lsmap.find(status.rd_); fnd != nullptr) {
                    auto &&info = fnd->second;
                    if (info.ls_ == LS_LOAD and (info.rd_ < riscv::a0 or info.rd_ > riscv::a7)) {
                        // insts_.erase(info.it_);
                    }
                    lsmap.erase(fnd);
                }

                lsmap[status.rd_] = std::move(status);

                break;
            }

            if (auto ld_inst = dynamic_cast<rv_ld *>(bear_ptr); ld_inst != nullptr) {
                if (ld_inst->rs_ != riscv::fp) {
                    // 不在这里解决
                    break;
                }

                meet = true;
                status.ls_ = LS_LOAD;
                status.width_ = 8;
                status.it_ = instit;
                status.off_ = ld_inst->off_;
                status.rd_ = ld_inst->rd_;
            }

            if (meet) {
                if (auto fnd = lsmap.find(status.rd_); fnd != nullptr) {
                    auto &&info = fnd->second;
                    if (info.ls_ == LS_LOAD and (info.rd_ < riscv::a0 or info.rd_ > riscv::a7)) {
                        // insts_.erase(info.it_);
                    }
                    lsmap.erase(fnd);
                }

                lsmap[status.rd_] = std::move(status);

                break;
            }

            if (auto sd_inst = dynamic_cast<rv_flw *>(bear_ptr); sd_inst != nullptr) {
                if (sd_inst->rs_ != riscv::fp) {
                    // 不在这里解决
                    break;
                }

                meet = true;
                status.ls_ = LS_STORE;
                status.width_ = 8;
                status.it_ = instit;
                status.off_ = sd_inst->off_;
                status.rd_ = sd_inst->rs_;
            }

            if (meet) {
                if (auto fnd = lsmap.find(status.rd_); fnd != nullptr) {
                    auto &&info = fnd->second;
                    if (info.ls_ == LS_LOAD and (info.rd_ < riscv::fa0 or info.rd_ > riscv::fa7)) {
                        // insts_.erase(info.it_);
                    }
                    lsmap.erase(fnd);
                }

                lsmap[status.rd_] = std::move(status);

                break;
            }

            if (auto sw_inst = dynamic_cast<rv_sw *>(bear_ptr); sw_inst != nullptr) {
                if (sw_inst->rs_ != riscv::fp) {
                    // 不在这里解决
                    break;
                }

                meet = true;
                status.ls_ = LS_STORE;
                status.width_ = 4;
                status.it_ = instit;
                status.off_ = sw_inst->off_;
                status.rd_ = sw_inst->rs_;
            }

            if (meet) {
                if (auto fnd = lsmap.find(status.rd_); fnd != nullptr) {
                    auto &&info = fnd->second;
                    if (info.ls_ == LS_LOAD and info.off_ == status.off_) {
                        instit = insts_.erase(instit);
                        reduce = true;
                    }
                }

                break;
            }

            if (auto sd_inst = dynamic_cast<rv_sd *>(bear_ptr); sd_inst != nullptr) {
                if (sd_inst->rs_ != riscv::fp) {
                    // 不在这里解决
                    break;
                }

                meet = true;
                status.ls_ = LS_STORE;
                status.width_ = 8;
                status.it_ = instit;
                status.off_ = sd_inst->off_;
                status.rd_ = sd_inst->rs_;
            }

            if (meet) {
                if (auto fnd = lsmap.find(status.rd_); fnd != nullptr) {
                    auto &&info = fnd->second;
                    if (info.ls_ == LS_LOAD and info.off_ == status.off_) {
                        instit = insts_.erase(instit);
                        reduce = true;
                    }
                }

                break;
            }

            if (auto sd_inst = dynamic_cast<rv_fsw *>(bear_ptr); sd_inst != nullptr) {
                if (sd_inst->rs_ != riscv::fp) {
                    // 不在这里解决
                    break;
                }

                meet = true;
                status.ls_ = LS_STORE;
                status.width_ = 8;
                status.it_ = instit;
                status.off_ = sd_inst->off_;
                status.rd_ = sd_inst->rs_;
            }

            if (meet) {
                if (auto fnd = lsmap.find(status.rd_); fnd != nullptr) {
                    auto &&info = fnd->second;
                    if (info.ls_ == LS_LOAD and info.off_ == status.off_) {
                        instit = insts_.erase(instit);
                        reduce = true;
                    }
                }

                break;
            }
        } while (0);

        if (reduce) {
            continue;
        }

        if (not meet) {
            auto rvinst = dynamic_cast<rv_inst *>(bear_ptr);
            Assert(rvinst, "not rv inst");

            if (auto fnd = lsmap.find(rvinst->rs_); fnd != lsmap.end()) {
                lsmap.erase(fnd);
            }
            if (auto fnd = lsmap.find(rvinst->rt_); fnd != lsmap.end()) {
                lsmap.erase(fnd);
            }
            if (auto fnd = lsmap.find(rvinst->ra_); fnd != lsmap.end()) {
                lsmap.erase(fnd);
            }

            if (auto fnd = lsmap.find(rvinst->rd_); fnd != lsmap.end()) {
                auto &&info = fnd->second;
                if (info.ls_ == LS_LOAD and (info.rd_ < riscv::a0 or info.rd_ > riscv::a7) and (info.rd_ < riscv::fa0 or info.rd_ > riscv::fa7)) {
                    // insts_.erase(info.it_);
                }
                lsmap.erase(fnd);
            }
        }

        instit++;
    }
}