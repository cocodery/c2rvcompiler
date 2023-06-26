#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/xin/xin.hh"

static std::mutex lcmtx;

void cross_internal_manager::irtorv() {
    apg_ = std::make_unique<progress>(rl_pgrs_.label_.data(), rl_pgrs_.bbs_.size());
    for (auto &&rlbb : rl_pgrs_.bbs_) {
        auto &&pb = std::make_unique<pblock>(rlbb->get_lbid(), rlbb->ops_.size());
        for (auto &&uop : rlbb->ops_) {
            uop->toasm(pb.get());
        }
        apg_->push(pb);
    }

    auto rv0 = new rv_sd(riscv::ra, riscv::sp, -8);
    auto rv1 = new rv_sd(riscv::fp, riscv::sp, -16);
    auto rv2 = new rv_mv(riscv::fp, riscv::sp);

    auto &&front_ilst = apg_->front()->ilst();

    if (imm_within(12, rl_pgrs_.valc_.total_stk_len)) {
        auto rv3 = new rv_addi(riscv::sp, riscv::sp, -(i64)rl_pgrs_.valc_.total_stk_len);
        front_ilst.push_front(std::unique_ptr<asm_inst>(rv3));
    } else {
        auto rv3 = new rv_li(riscv::t0, rl_pgrs_.valc_.total_stk_len);
        auto rv4 = new rv_sub(riscv::sp, riscv::sp, riscv::t0);
        front_ilst.push_front(std::unique_ptr<asm_inst>(rv4));
        front_ilst.push_front(std::unique_ptr<asm_inst>(rv3));
    }

    front_ilst.push_front(std::unique_ptr<asm_inst>(rv2));
    front_ilst.push_front(std::unique_ptr<asm_inst>(rv1));
    front_ilst.push_front(std::unique_ptr<asm_inst>(rv0));

    auto &&back_ilst = apg_->back()->ilst();
    auto last = std::move(back_ilst.back());
    auto retinst = dynamic_cast<rv_ret *>(last.get());
    Assert(retinst, "last inst not ret");
    (void)retinst;

    back_ilst.pop_back();

    if (imm_within(12, rl_pgrs_.valc_.total_stk_len)) {
        auto rv5 = new rv_addi(riscv::sp, riscv::sp, rl_pgrs_.valc_.total_stk_len);
        back_ilst.push_back(std::unique_ptr<asm_inst>(rv5));
    } else {
        auto rv5 = new rv_li(riscv::t0, rl_pgrs_.valc_.total_stk_len);
        back_ilst.push_back(std::unique_ptr<asm_inst>(rv5));

        auto rv6 = new rv_add(riscv::sp, riscv::sp, riscv::t0);
        back_ilst.push_back(std::unique_ptr<asm_inst>(rv6));
    }

    auto rv7 = new rv_ld(riscv::ra, riscv::sp, -8);
    auto rv8 = new rv_ld(riscv::fp, riscv::sp, -16);

    back_ilst.push_back(std::unique_ptr<asm_inst>(rv7));
    back_ilst.push_back(std::unique_ptr<asm_inst>(rv8));

    back_ilst.push_back(std::move(last));

    std::scoped_lock<std::mutex> lck{lcmtx};
    rl_pgrs_.valc_.give_loc(lc_pool_);
}