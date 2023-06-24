#include "xin.hh"

void cross_internal_manager::irpass() {
    // size_t uop_idx = 1;
    // std::unique_ptr<rl_basicblock> retblk;
    // auto it = rl_pgrs_.bbs_.begin();
    // while (true) {
    //     if (++it == rl_pgrs_.bbs_.end()) {
    //         break;
    //     }
    //     auto &&back = (*it)->ops_.back();
    //     if (auto retinst = dynamic_cast<uop_ret *>(back.get()); retinst != nullptr) {
    //         retblk = std::move(*it);
    //         rl_pgrs_.bbs_.erase(it);
    //         break;
    //     }
    // }
    // rl_pgrs_.bbs_.push_back(std::move(retblk));

    for (auto &&lrbb : rl_pgrs_.bbs_) {
        for (auto &&uop : lrbb->ops_) {
            // uop->set_uop_idx(uop_idx);
            uop->trace_call();
            uop->trace_inst();
            // uop->live_info();

            // uop_idx += 1;
        }
    }
}