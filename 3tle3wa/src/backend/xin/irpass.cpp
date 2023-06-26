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

    irpass_delete_single_jump();

    irpass_combine_fallthrough();

    // 因为做了块合并，所以需要重新计算 father 标签
    for (auto &&rlbb : rl_pgrs_.bbs_) {
        auto falb = rlbb->get_lbid();
        for (auto &&uop : rlbb->ops_) {
            uop->trace_inst(falb);
        }
    }

    irpass_gen_cmpb();

    // irpass_gen_fmas();
}

// 删除单跳转块
void cross_internal_manager::irpass_delete_single_jump() {
    auto bbit = rl_pgrs_.bbs_.begin();
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
}

// 合并块
void cross_internal_manager::irpass_combine_fallthrough() {
    auto bbit = rl_pgrs_.bbs_.begin();
    while (bbit != rl_pgrs_.bbs_.end()) {
        auto curbit = bbit++;
        auto nxtbit = bbit;

        auto &&curbb = *curbit;
        auto &&nxtbb = *nxtbit;

        if (nxtbit == rl_pgrs_.bbs_.end()) {
            break;
        }

        auto &&backinst = curbb->ops_.back();
        auto jmp = dynamic_cast<uop_j *>(backinst.get());

        if (jmp == nullptr) {
            // 结尾不是跳转，而是分支的话，不合并
            continue;
        }

        if (jmp->get_lbid() != nxtbb->get_lbid()) {
            // 不是 fall through 不合并
            continue;
        }

        auto &&nxtlbinfo = rl_pgrs_.lbmap_.at(nxtbb->get_lbid());
        if (nxtlbinfo.refs_.size() > 1) {
            // 下一个块如果可能从其他地方进入，也不合并
            continue;
        }

        // 接下来认为两个块可以合并，那么检查

        // 将当前块的 successer 清空并复制下一个块的 successer 信息
        // 删除下一个块的标签信息
        // 这两步由上方的判断做保障
        curbb->successer.clear();
        curbb->successer.insert(nxtbb->successer.begin(), nxtbb->successer.end());
        rl_pgrs_.lbmap_.erase(nxtbb->get_lbid());

        // 开始合并
        // 删除最后的跳转
        curbb->ops_.pop_back();

        auto opit = nxtbb->ops_.begin();

        while (opit != nxtbb->ops_.end()) {
            curbb->ops_.push_back(std::move(*opit));
            opit++;
        }

        // 删除下一个块
        rl_pgrs_.bbs_.erase(nxtbit);

        // 合并完成后，需要从当前重新开始检查
        // 可能还可以继续合并
        bbit = curbit;
    }
}

// 合并 icmp 和 b
void cross_internal_manager::irpass_gen_cmpb() {
    for (auto &&rlbb : rl_pgrs_.bbs_) {
        auto &&lastinst = rlbb->ops_.back();

        auto br = dynamic_cast<uop_b *>(lastinst.get());
        if (br == nullptr) {
            continue;
        }

        auto &&cond = br->get_cond();

        auto icmp = dynamic_cast<uop_icmp *>(cond->from());
        if (icmp == nullptr or cond->kind() != VREG_KIND::REG) {
            // 不是来自整数比较就放弃合并
            // 小小假设必须是 REG 类型，这样删除方便
            continue;
        }

        auto &&cond_refs = cond->refs();

        if (cond_refs.size() > 1) {
            // 多于一次使用就放弃合并
            continue;
        }

        auto &&lhs = icmp->get_lhs();
        auto &&lrefs = lhs->refs();
        lrefs.erase(icmp);

        auto &&rhs = icmp->get_rhs();
        auto &&rrefs = rhs->refs();
        rrefs.erase(icmp);

        auto op = std::make_unique<uop_icmp_b>();
        op->set_lhs(icmp->get_lhs());
        op->set_rhs(icmp->get_rhs());
        op->set_lbid(br->get_lbid());

        if (br->get_ontrue()) {
            op->set_kind(icmp->get_kind());
        } else {
            switch (icmp->get_kind()) {
                case COMP_KIND::EQU:
                    op->set_kind(COMP_KIND::NEQ);
                    break;
                case COMP_KIND::NEQ:
                    op->set_kind(COMP_KIND::EQU);
                    break;
                case COMP_KIND::LTH:
                    op->set_kind(COMP_KIND::GEQ);
                    break;
                case COMP_KIND::GTH:
                    op->set_kind(COMP_KIND::LEQ);
                    break;
                case COMP_KIND::LEQ:
                    op->set_kind(COMP_KIND::GTH);
                    break;
                case COMP_KIND::GEQ:
                    op->set_kind(COMP_KIND::LTH);
                    break;
            }
        }
                        
        op->trace_inst(rlbb->get_lbid());

        auto &&target = rl_pgrs_.lbmap_.at(br->get_lbid());
        target.refs_.remove(lastinst.get());
        target.refs_.emplace_back(op.get());

        rlbb->ops_.remove_if([icmp, br](const std::unique_ptr<uop_general> &elem) -> bool {
            return elem.get() == icmp or elem.get() == br;
        });

        rlbb->ops_.push_back(std::move(op));

        rl_pgrs_.valc_.rmreg(cond->value());
    }
}

// 合并 fmul 和 fadd fsub
// 保证 fmul 先执行
void cross_internal_manager::irpass_gen_fmas() {
    for (auto &&rlbb : rl_pgrs_.bbs_) {
        auto curit = rlbb->ops_.begin();
        while (curit != rlbb->ops_.end()) {
            auto &&cur = *curit;
            auto fbin_as = dynamic_cast<uop_fbin *>(cur.get());

            if (fbin_as == nullptr or
                (fbin_as->get_kind() != FBIN_KIND::ADD and fbin_as->get_kind() != FBIN_KIND::SUB)) {
                curit++;
                continue;
            }

            bool combine_success = false;

            do {
                auto lhs = fbin_as->get_lhs();
                auto lhs_refs = lhs->refs();

                if (lhs_refs.size() > 1) {
                    break;
                }

                auto lfrom = lhs->from();

                auto lfrom_fbin_mul = dynamic_cast<uop_fbin *>(lfrom);
                if (lfrom_fbin_mul != nullptr and lfrom_fbin_mul->get_kind() == FBIN_KIND::MUL) {
                    if (fbin_as->get_kind() == FBIN_KIND::ADD) {
                        auto op = std::make_unique<uop_ftri>();
                        op->set_lhs(lfrom_fbin_mul->get_lhs());
                        op->set_rhs(lfrom_fbin_mul->get_rhs());
                        op->set_ahs(fbin_as->get_rhs());
                        op->set_rd(fbin_as->get_rd());

                        op->trace_inst(rlbb->get_lbid());

                        op->set_kind(FTRI_KIND::MADD);

                        rlbb->ops_.remove_if([lfrom_fbin_mul](const std::unique_ptr<uop_general> &elem) -> bool {
                            return elem.get() == lfrom_fbin_mul;
                        });

                        rlbb->ops_.insert(curit, std::move(op));
                        curit = rlbb->ops_.erase(curit);

                        // 直接假设是寄存器类型
                        rl_pgrs_.valc_.rmreg(fbin_as->get_lhs()->value());
                        combine_success = true;
                        break;
                    }

                    if (fbin_as->get_kind() == FBIN_KIND::SUB) {
                        auto op = std::make_unique<uop_ftri>();
                        op->set_lhs(lfrom_fbin_mul->get_lhs());
                        op->set_rhs(lfrom_fbin_mul->get_rhs());
                        op->set_ahs(fbin_as->get_rhs());
                        op->set_rd(fbin_as->get_rd());
                        
                        op->trace_inst(rlbb->get_lbid());

                        op->set_kind(FTRI_KIND::MSUB);

                        rlbb->ops_.remove_if([lfrom_fbin_mul](const std::unique_ptr<uop_general> &elem) -> bool {
                            return elem.get() == lfrom_fbin_mul;
                        });

                        rlbb->ops_.insert(curit, std::move(op));
                        curit = rlbb->ops_.erase(curit);

                        // 直接假设是寄存器类型
                        rl_pgrs_.valc_.rmreg(fbin_as->get_lhs()->value());
                        combine_success = true;
                        break;
                    }
                }
            } while (0);

            if (combine_success) {
                continue;
            }

            do {
                auto rhs = fbin_as->get_rhs();
                auto rhs_refs = rhs->refs();

                if (rhs_refs.size() > 1) {
                    break;
                }

                auto rfrom = rhs->from();

                auto rfrom_fbin_mul = dynamic_cast<uop_fbin *>(rfrom);
                if (rfrom_fbin_mul != nullptr and rfrom_fbin_mul->get_kind() == FBIN_KIND::MUL) {
                    if (fbin_as->get_kind() == FBIN_KIND::ADD) {
                        auto op = std::make_unique<uop_ftri>();
                        op->set_lhs(rfrom_fbin_mul->get_lhs());
                        op->set_rhs(rfrom_fbin_mul->get_rhs());
                        op->set_ahs(fbin_as->get_lhs());
                        op->set_rd(fbin_as->get_rd());
                        
                        op->trace_inst(rlbb->get_lbid());

                        op->set_kind(FTRI_KIND::MADD);

                        rlbb->ops_.remove_if([rfrom_fbin_mul](const std::unique_ptr<uop_general> &elem) -> bool {
                            return elem.get() == rfrom_fbin_mul;
                        });

                        rlbb->ops_.insert(curit, std::move(op));
                        curit = rlbb->ops_.erase(curit);

                        // 直接假设是寄存器类型
                        rl_pgrs_.valc_.rmreg(fbin_as->get_rhs()->value());
                        combine_success = true;
                        break;
                    }

                    if (fbin_as->get_kind() == FBIN_KIND::SUB) {
                        auto op = std::make_unique<uop_ftri>();
                        op->set_lhs(rfrom_fbin_mul->get_lhs());
                        op->set_rhs(rfrom_fbin_mul->get_rhs());
                        op->set_ahs(fbin_as->get_lhs());
                        op->set_rd(fbin_as->get_rd());
                        
                        op->trace_inst(rlbb->get_lbid());

                        op->set_kind(FTRI_KIND::NMSUB);

                        rlbb->ops_.remove_if([rfrom_fbin_mul](const std::unique_ptr<uop_general> &elem) -> bool {
                            return elem.get() == rfrom_fbin_mul;
                        });

                        rlbb->ops_.insert(curit, std::move(op));
                        curit = rlbb->ops_.erase(curit);

                        // 直接假设是寄存器类型
                        rl_pgrs_.valc_.rmreg(fbin_as->get_rhs()->value());
                        combine_success = true;
                        break;
                    }
                }
            } while (0);

            if (combine_success) {
                continue;
            }

            curit++;
        }
    }
}
