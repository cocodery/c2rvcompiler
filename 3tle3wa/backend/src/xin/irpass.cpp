#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/xin/xin.hh"

void cross_internal_manager::irpass() {
    for (auto &&rlbb : rl_pgrs_.bbs_) {
        auto falb = rlbb->get_lbid();
        for (auto &&uop : rlbb->ops_) {
            uop->trace_call();
            uop->trace_inst(falb);
        }
    }

    // return;

    std::list<std::unique_ptr<rl_basicblock>>::iterator bbit;

    // 删除单跳转块
    bbit = rl_pgrs_.bbs_.begin();
    while (bbit != rl_pgrs_.bbs_.end()) {
        auto &&rlbb = *bbit;
        auto &&curlb = rlbb->get_lbid();

        if (rlbb->ops_.size() > 1) {
            // 超过一句话的标签不做删除
            ++bbit;
            continue;
        }

        auto &&op = rlbb->ops_.back();
        auto jmp = dynamic_cast<uop_j *>(op.get());
        if (jmp == nullptr) {
            ++bbit;
            continue;
        }

        auto &&lbinfo = rl_pgrs_.lbmap_.at(curlb);

        auto target = jmp->get_lbid();
        auto &&target_lbinfo = rl_pgrs_.lbmap_.at(target);

        bool removable = true;
        for (auto &&ref : lbinfo.refs_) {
            if (auto ref_b = dynamic_cast<uop_b *>(ref); ref_b != nullptr) {
                if (ref_b->get_false_lbid() == curlb) {
                    removable = false;
                }
            }
        }

        if (not removable) {
            // 有 fall through 问题的一概不变
            ++bbit;
            continue;
        }

        // 对每个引用进行修改
        for (auto &&ref : lbinfo.refs_) {
            if (auto ref_j = dynamic_cast<uop_j *>(ref); ref_j != nullptr) {
                ref_j->set_lbid(target);
            } else if (auto ref_b = dynamic_cast<uop_b *>(ref); ref_b != nullptr) {
                if (ref_b->get_lbid() == curlb) {
                    ref_b->set_lbid(target);
                }
            } else {
                panic("unexpected");
            }

            // 删除 succ 记录，domi 记录目前不考虑
            auto &&fabb = rl_pgrs_.lbmap_.at(ref->get_fa_idx());
            fabb.bbp_->successer.erase(curlb);
            fabb.bbp_->successer.insert(target);
            target_lbinfo.refs_.push_back(ref);
        }

        target_lbinfo.refs_.remove(jmp);

        // 删除与该块相关的记录
        rl_pgrs_.lbmap_.erase(curlb);

        bbit = rl_pgrs_.bbs_.erase(bbit);
    }

    // 合并块

}