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

    opm_reorder();
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
        } else if (auto mv_inst = dynamic_cast<rv_mv *>(bear_ptr); mv_inst != nullptr) {
            if (mv_inst->rs_ == riscv::t0 and online) {
                auto rv = new rv_li(mv_inst->rd_, curint);
                insts_.insert(instit, std::unique_ptr<rv_inst>(rv));
                instit = insts_.erase(instit);
            }
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
        auto rvinst = dynamic_cast<rv_inst *>(bear_ptr);
        Assert(rvinst, "not rv inst");

        do /* check */ {
            lsinfo status;

            if (dynamic_cast<rv_lw *>(bear_ptr) != nullptr or dynamic_cast<rv_ld *>(bear_ptr) != nullptr or
                dynamic_cast<rv_flw *>(bear_ptr) != nullptr) {
                if (rvinst->rs_ != riscv::fp) {
                    // 不在这里解决
                    break;
                }

                meet = true;
                status.ls_ = LS_LOAD;

                // 目前先不考虑
                status.width_ = 8;

                status.it_ = instit;
                status.off_ = rvinst->off_;
                status.rd_ = rvinst->rd_;

                if (auto fnd = lsmap.find(status.rd_); fnd != lsmap.end()) {
                    auto &&info = fnd->second;
                    if (info.ls_ == LS_LOAD and (info.rd_ < riscv::a0 or info.rd_ > riscv::a7)) {
                        // insts_.erase(info.it_);
                    }
                    lsmap.erase(fnd);
                }

                lsmap[status.rd_] = std::move(status);

                break;
            }

            if (dynamic_cast<rv_sw *>(bear_ptr) != nullptr or dynamic_cast<rv_sd *>(bear_ptr) != nullptr or
                dynamic_cast<rv_fsw *>(bear_ptr) != nullptr) {
                if (rvinst->rs_ != riscv::fp) {
                    // 不在这里解决
                    break;
                }

                meet = true;
                status.ls_ = LS_STORE;

                // 目前先不考虑
                status.width_ = 8;

                status.it_ = instit;
                status.off_ = rvinst->off_;
                status.rd_ = rvinst->rs_;

                if (auto fnd = lsmap.find(status.rd_); fnd != lsmap.end()) {
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
                if (info.ls_ == LS_LOAD and (info.rd_ < riscv::a0 or info.rd_ > riscv::a7) and
                    (info.rd_ < riscv::fa0 or info.rd_ > riscv::fa7)) {
                    // insts_.erase(info.it_);
                }
                lsmap.erase(fnd);
            }
        }

        instit++;
    }
}

void pblock::opm_reorder() {
    return;

    aov_node *reguser[64] = {nullptr};
    uint64_t busy = 0;

    std::unordered_set<aov_node *> free_to_issue;

    for (auto &&inst : insts_) {
        if (dynamic_cast<rv_call *>(inst.get()) != nullptr) {
            return;
        }
        auto bear_ptr = inst.get();
        auto rvinst = dynamic_cast<rv_inst *>(bear_ptr);

        auto aovp = std::make_unique<aov_node>();
        aovp->cur_ = rvinst;

        if (rvinst->rd_ != riscv::zero) {
            aovp->write_reg_ |= 1l << rvinst->rd_;
        }
        if (rvinst->rs_ != riscv::zero) {
            aovp->issue_need_ |= 1l << rvinst->rs_;
        }
        if (rvinst->rt_ != riscv::zero) {
            aovp->issue_need_ |= 1l << rvinst->rt_;
        }
        if (rvinst->ra_ != riscv::zero) {
            aovp->issue_need_ |= 1l << rvinst->ra_;
        }

        auto need_wait = busy & aovp->issue_need_;
        uint64_t msk = 0x1;
        for (size_t i = 1; i < 64; ++i) {
            if (need_wait & (msk << i)) {
                if (reguser[i] != nullptr) {
                    auto prev = reguser[i];
                    aovp->prevs_.insert(prev);
                    prev->posts_.insert(aovp.get());
                }
            }
        }

        if (aovp->write_reg_ != 0) {
            auto use = __builtin_ctzll(aovp->write_reg_);
            busy |= 1l << use;
            reguser[use] = aovp.get();
        }

        if (aovp->prevs_.size() == 0) {
            free_to_issue.insert(aovp.get());
        }

        aov_source.push_back(std::move(aovp));
    }

    while (not free_to_issue.empty()) {
        size_t num_take = 0;
        aov_node *first = nullptr;
        aov_node *second = nullptr;
        busy = 0;

        // 选择一个内存存取操作
        for (auto &&node : free_to_issue) {
            if (node->cur_->type_ == ACCMEM) {
                bool taken = false;
                if (num_take == 0) {
                    auto rd = node->cur_->rd_;
                    if (rd != riscv::zero) {
                        busy |= 0x1 << rd;
                    }
                    first = node;
                    taken = true;
                    num_take += 1;
                } else if (num_take == 1) {
                    uint64_t curbusy = 0;
                    auto rd = node->cur_->rd_;
                    if (rd != riscv::zero) {
                        curbusy |= 0x1 << rd;
                    }
                    if ((curbusy & busy) != 0) {
                        continue;
                    }
                    second = node;
                    taken = true;
                    num_take += 1;
                } else {
                    break;
                }
                if (taken) {
                    for (auto &&post : node->posts_) {
                        post->prevs_.erase(node);
                        if (post->prevs_.empty()) {
                            free_to_issue.insert(post);
                        }
                    }
                    break;
                }
            }
        }

        // 选择浮点操作
        for (auto &&node : free_to_issue) {
            if (node->cur_->type_ == FLTOP) {
                bool taken = false;
                if (num_take == 0) {
                    auto rd = node->cur_->rd_;
                    if (rd != riscv::zero) {
                        busy |= 0x1 << rd;
                    }
                    first = node;
                    taken = true;
                    num_take += 1;
                } else if (num_take == 1) {
                    uint64_t curbusy = 0;
                    auto rd = node->cur_->rd_;
                    if (rd != riscv::zero) {
                        curbusy |= 0x1 << rd;
                    }
                    if ((curbusy & busy) != 0) {
                        continue;
                    }
                    second = node;
                    taken = true;
                    num_take += 1;
                } else {
                    break;
                }
                if (taken) {
                    for (auto &&post : node->posts_) {
                        post->prevs_.erase(node);
                        if (post->prevs_.empty()) {
                            free_to_issue.insert(post);
                        }
                    }
                    break;
                }
            }
        }

        // 选择乘除操作
        for (auto &&node : free_to_issue) {
            if (node->cur_->type_ == INTMDR) {
                bool taken = false;
                if (num_take == 0) {
                    auto rd = node->cur_->rd_;
                    if (rd != riscv::zero) {
                        busy |= 0x1 << rd;
                    }
                    first = node;
                    taken = true;
                    num_take += 1;
                } else if (num_take == 1) {
                    uint64_t curbusy = 0;
                    auto rd = node->cur_->rd_;
                    if (rd != riscv::zero) {
                        curbusy |= 0x1 << rd;
                    }
                    if ((curbusy & busy) != 0) {
                        continue;
                    }
                    second = node;
                    taken = true;
                    num_take += 1;
                } else {
                    break;
                }
                if (taken) {
                    for (auto &&post : node->posts_) {
                        post->prevs_.erase(node);
                        if (post->prevs_.empty()) {
                            free_to_issue.insert(post);
                        }
                    }
                    break;
                }
            }
        }

        // 选择一般操作
        for (auto &&node : free_to_issue) {
            if (node->cur_->type_ == NORM) {
                bool taken = false;
                if (num_take == 0) {
                    auto rd = node->cur_->rd_;
                    if (rd != riscv::zero) {
                        busy |= 0x1 << rd;
                    }
                    first = node;
                    taken = true;
                    num_take += 1;
                } else if (num_take == 1) {
                    uint64_t curbusy = 0;
                    auto rd = node->cur_->rd_;
                    if (rd != riscv::zero) {
                        curbusy |= 0x1 << rd;
                    }
                    if ((curbusy & busy) != 0) {
                        continue;
                    }
                    second = node;
                    taken = true;
                    num_take += 1;
                } else {
                    break;
                }
                if (taken) {
                    for (auto &&post : node->posts_) {
                        post->prevs_.erase(node);
                        if (post->prevs_.empty()) {
                            free_to_issue.insert(post);
                        }
                    }
                    break;
                }
            }
        }

        if (first != nullptr) {
            inst_view_.push_back(first->cur_);
            free_to_issue.erase(first);
        }

        if (second != nullptr) {
            inst_view_.push_back(second->cur_);
            free_to_issue.erase(second);
        }

        // 只剩下跳转则直接包含
        for (auto &&node : free_to_issue) {
            if (node->cur_->type_ == BRJMP) {
                inst_view_.push_back(node->cur_);
                free_to_issue.clear();
            }
        }

        for (auto &&node : free_to_issue) {
            if (node->cur_->type_ == UNDEFINED) {
                panic("unexpected");
            }
        }
    }

    Assert(inst_view_.size() == insts_.size(), "not all inst issued");
}