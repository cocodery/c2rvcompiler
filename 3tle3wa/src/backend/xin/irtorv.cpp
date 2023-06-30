#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/xin/xin.hh"

static std::mutex lcmtx;

void cross_internal_manager::irtorv() {
    apg_ = std::make_unique<progress>(rl_pgrs_.label_.data(), rl_pgrs_.bbs_.size(), gname_map_);

    for (auto &&rlbb : rl_pgrs_.bbs_) {
        auto &&pb = std::make_unique<pblock>(rlbb->get_lbid(), rlbb->ops_.size(), apg_.get());
        for (auto &&uop : rlbb->ops_) {
            uop->toasm(pb.get());
        }
        apg_->push(pb);
    }

    auto &&front_ilst = apg_->front()->ilst();

    if (rl_pgrs_.valc_.total_stk_len != 0) {
        if (imm_within(12, rl_pgrs_.valc_.total_stk_len)) {
            auto rv3 = new rv_addi(riscv::sp, riscv::sp, -(i64)rl_pgrs_.valc_.total_stk_len);
            front_ilst.push_front(std::unique_ptr<asm_inst>(rv3));
        } else {
            auto rv3 = new rv_li(riscv::t0, rl_pgrs_.valc_.total_stk_len);
            auto rv4 = new rv_sub(riscv::sp, riscv::sp, riscv::t0);
            front_ilst.push_front(std::unique_ptr<asm_inst>(rv4));
            front_ilst.push_front(std::unique_ptr<asm_inst>(rv3));
        }

        if (rl_pgrs_.contain_funcall_) {
            if (rl_pgrs_.valc_.total_stk_len > 8) {
                auto rv2 = new rv_mv(riscv::fp, riscv::sp);
                front_ilst.push_front(std::unique_ptr<asm_inst>(rv2));

                auto rv1 = new rv_sd(riscv::fp, riscv::sp, -16);
                front_ilst.push_front(std::unique_ptr<asm_inst>(rv1));
            }

            auto rv0 = new rv_sd(riscv::ra, riscv::sp, -8);
            front_ilst.push_front(std::unique_ptr<asm_inst>(rv0));

        } else {
            auto rv2 = new rv_mv(riscv::fp, riscv::sp);
            front_ilst.push_front(std::unique_ptr<asm_inst>(rv2));

            auto rv0 = new rv_sd(riscv::fp, riscv::sp, -8);
            front_ilst.push_front(std::unique_ptr<asm_inst>(rv0));
        }
    }

    auto &&back_ilst = apg_->back()->ilst();
    auto last = std::move(back_ilst.back());
    auto retinst = dynamic_cast<rv_ret *>(last.get());
    Assert(retinst, "last inst not ret");
    (void)retinst;

    back_ilst.pop_back();

    if (rl_pgrs_.valc_.total_stk_len != 0) {
        if (imm_within(12, rl_pgrs_.valc_.total_stk_len)) {
            auto rv5 = new rv_addi(riscv::sp, riscv::sp, rl_pgrs_.valc_.total_stk_len);
            back_ilst.push_back(std::unique_ptr<asm_inst>(rv5));
        } else {
            auto rv5 = new rv_li(riscv::t0, rl_pgrs_.valc_.total_stk_len);
            back_ilst.push_back(std::unique_ptr<asm_inst>(rv5));

            auto rv6 = new rv_add(riscv::sp, riscv::sp, riscv::t0);
            back_ilst.push_back(std::unique_ptr<asm_inst>(rv6));
        }

        if (rl_pgrs_.contain_funcall_) {
            auto rv7 = new rv_ld(riscv::ra, riscv::sp, -8);
            back_ilst.push_back(std::unique_ptr<asm_inst>(rv7));

            if (rl_pgrs_.valc_.total_stk_len > 8) {
                auto rv8 = new rv_ld(riscv::fp, riscv::sp, -16);
                back_ilst.push_back(std::unique_ptr<asm_inst>(rv8));
            }
        } else {
            auto rv8 = new rv_ld(riscv::fp, riscv::sp, -8);
            back_ilst.push_back(std::unique_ptr<asm_inst>(rv8));
        }
    }

    back_ilst.push_back(std::move(last));

    auto tails = apg_->tails();
    for (auto &&tail : tails) {
        auto pb = tail.first->get();
        auto &&tail_ilst = pb->ilst();
        auto inst_it = tail.second;

        auto tailinst = dynamic_cast<rv_tail *>(inst_it->get());
        auto pstk = tailinst->imm_;
        for (size_t i = 0; i < (size_t)pstk; ++i) {
            auto rvx = new rv_ld(riscv::t0, riscv::sp, i * 8);
            tail_ilst.insert(inst_it, std::unique_ptr<asm_inst>(rvx));

            auto rvy = new rv_sd(riscv::t0, riscv::fp, i * 8);
            tail_ilst.insert(inst_it, std::unique_ptr<asm_inst>(rvy));
        }

        if (rl_pgrs_.valc_.total_stk_len != 0) {
            if (imm_within(12, rl_pgrs_.valc_.total_stk_len)) {
                auto rv5 = new rv_addi(riscv::sp, riscv::sp, rl_pgrs_.valc_.total_stk_len);
                tail_ilst.insert(inst_it, std::unique_ptr<asm_inst>(rv5));
            } else {
                auto rv5 = new rv_li(riscv::t0, rl_pgrs_.valc_.total_stk_len);
                tail_ilst.insert(inst_it, std::unique_ptr<asm_inst>(rv5));

                auto rv6 = new rv_add(riscv::sp, riscv::sp, riscv::t0);
                tail_ilst.insert(inst_it, std::unique_ptr<asm_inst>(rv6));
            }

            if (rl_pgrs_.contain_funcall_) {
                auto rv7 = new rv_ld(riscv::ra, riscv::sp, -8);
                tail_ilst.insert(inst_it, std::unique_ptr<asm_inst>(rv7));

                if (rl_pgrs_.valc_.total_stk_len > 8) {
                    auto rv8 = new rv_ld(riscv::fp, riscv::sp, -16);
                    tail_ilst.insert(inst_it, std::unique_ptr<asm_inst>(rv8));
                }
            } else {
                auto rv8 = new rv_ld(riscv::fp, riscv::sp, -8);
                tail_ilst.insert(inst_it, std::unique_ptr<asm_inst>(rv8));
            }
        }
    }

    std::scoped_lock<std::mutex> lck{lcmtx};
    rl_pgrs_.valc_.give_loc(lc_pool_);
}