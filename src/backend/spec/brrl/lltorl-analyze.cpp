#include "lltorl.hh"

void rl_lltorl_instance::rmx() {
    // 删除所有只有一个跳转语句的基本块
    for (auto iter = rlp->bbs.begin(); iter != rlp->bbs.end(); ++iter) {
        auto cur = *iter;

        // 如果当前基本块只有一条指令
        if (cur->ops.size() == 1) {
            auto op = cur->ops.back();
            auto jop = std::dynamic_pointer_cast<uop_j>(op);

            // 判断该指令是否为跳转
            if (jop != nullptr) {
                // 获取跳转目的的信息
                auto &&target_record = rlp->lbmap.at(jop->lbid);

                // 修改当前块的被引用处，也即所有跳转到本块的跳转语句
                for (auto &&op : rlp->lbmap.at(cur->lbid).refs) {
                    if (auto &&jopx = dynamic_cast<uop_j *>(op); jopx != nullptr) {
                        // 将跳转目的地转换到新的目的地
                        jopx->lbid = jop->lbid;

                        // 将其加入新目的地的引用中
                        target_record.refs.push_back(jopx);
                    } else if (auto &&bopx = dynamic_cast<uop_b *>(op); bopx != nullptr) {
                        // 将跳转目的地转换到新的目的地
                        bopx->lbid = jop->lbid;

                        // 将其加入新目的地的引用中
                        target_record.refs.push_back(bopx);
                    } else {
                        panic("unexpected");
                    }
                }
                // 删除当前跳转在目的基本块内的引用
                rlp->lbmap.at(jop->lbid).refs.remove(jop.get());

                // 删除当前块的记录信息
                rlp->lbmap.erase(cur->lbid);

                // 删除当前块
                iter = rlp->bbs.erase(iter);
            }
        }
    }

    // 删除非必要跳转
    for (auto iter = rlp->bbs.begin(); iter != rlp->bbs.end();) {
        // 获得当前基本块和下一个基本块
        auto cur = iter++;
        if (iter == rlp->bbs.end()) break;

        auto &&bb = *cur;

        // 检查当前基本块的最后一条指令是否为跳转
        auto op = bb->ops.back();
        auto jop = std::dynamic_pointer_cast<uop_j>(op);

        if (jop != nullptr) {
            auto target_info = rlp->lbmap[jop->lbid];

            // 检查目的基本块的引用是否只有当前这一个
            // *如果要更加保险，请检查引用是否是该跳转*
            if (target_info.refs.size() > 1) {
                continue;
            }

            // 删除它所跳转的目的基本块中的引用
            rlp->lbmap[jop->lbid].refs.remove(jop.get());

            // 删除该指令
            // 接下来该指令只能被 jop 获取
            bb->ops.pop_back();

            // 将目的基本块内的所有指令都加到当前基本块中
            auto &&nxt = target_info.bbp;
            for (auto &&inst : nxt->ops) {
                bb->ops.push_back(inst);
            }

            // 删除目的基本块的记录信息
            rlp->lbmap.erase(nxt->lbid);

            // 删除目的基本块
            rlp->bbs.remove_if([nxt](std::shared_ptr<rl_basicblock> &sptr) -> bool { return sptr.get() == nxt; });

            // 回到自身，再检查一次
            iter = cur;
        }
    }
}

void rl_lltorl_instance::mvret() {
    // 将返回指令移动到基本块的最后
    // 在开启优化后该次历遍可以省略

    std::shared_ptr<rl_basicblock> retbb = nullptr;

    for (auto iter = rlp->bbs.begin(); iter != rlp->bbs.end(); ++iter) {
        auto &&bb = *iter;
        auto back = bb->ops.back();
        if (std::dynamic_pointer_cast<uop_ret>(back) != nullptr) {
            // 加入当前基本块包含返回指令
            // 保证不会有多个返回指令
            if (retbb != nullptr) {
                panic("multi ret");
            }

            // 复制对基本块的指针
            retbb = bb;

            // 删除基本块
            iter = rlp->bbs.erase(iter);
        }
    }
    // 检查是否找到
    Assert(retbb, "no ret inst");

    // 将包含返回值的基本块放到最后
    rlp->bbs.push_back(retbb);
}

void rl_lltorl_instance::ireidx() {
    // 0 用于表示进入基本块
    // 保留给形式参数类型的虚拟寄存器
    size_t reidx = 1;
    auto biter = rlp->bbs.begin();

    do {
        auto &&bb = *biter;

        auto iiter = bb->ops.begin();

        do {
            auto &&inst = *iiter;
            inst->uop_idx = reidx;

            // 分析生存区间，同时计算引用次数
            inst->set_interval(reidx);

            reidx += 1;
        } while (++iiter != bb->ops.end());

    } while (++biter != rlp->bbs.end());
}

void rl_lltorl_instance::loadparam() {
    off64_t off = 0;
    auto &&top = rlp->bbs.front();

    size_t idx = 0;
    size_t fidx = 0;

    for (auto &&prm : rlp->params) {
        if (prm->isflt) {
            if (fidx > 7) {
                auto op = std::make_unique<uop_load>();
                auto fp = rlp->vreg_alloc.alloc(VREG_KIND::FP, 0, 0);
                Assert(fp->kind == VREG_KIND::FP, "fp mistake");
                op->dst = prm;
                op->base = fp;
                op->off = off;
                op->onflt;
                top->ops.push_front(std::move(op));
                off += prm->len;
            }
            fidx += 1;
        } else {
            if (idx > 7) {
                auto op = std::make_unique<uop_load>();
                auto fp = rlp->vreg_alloc.alloc(VREG_KIND::FP, 0, 0);
                Assert(fp->kind == VREG_KIND::FP, "fp mistake");
                op->dst = prm;
                op->base = fp;
                op->off = off;
                top->ops.push_front(std::move(op));
                off += prm->len;
            }
            idx += 1;
        }
    }
}

void rl_lltorl_instance::combine() {
    std::unordered_map<uop_fbin *, std::list<std::shared_ptr<uop_general>>::iterator> colle;

    //
    //      rx = ry * rz
    //      ^
    // rp = rx +/- rq
    //
    // can convert to
    // rp = ry * rz +/- rq
    //
    // madd / msub
    //
    //
    //             rx = ry * rz
    //             ^
    // rp = rq +/- rx
    //
    // can convert to
    // rp = ry * rz + rq
    // rp = - (ry * rz - rq)
    //
    // madd / nmsub
    //
    //       rq = 0 - rr   rx = ry * rz
    //       ^             ^
    // rp =  rq      -     rx
    //
    // can convert to
    // rp = - ry * rz - rq
    //
    // nmadd
    //
    // too many T_T
    // just greedy !
    //
    auto biter = rlp->bbs.begin();

    do {
        auto &&bb = *biter;

        auto iiter = bb->ops.begin();

        do {
            auto &&inst = *iiter;

            auto fop = dynamic_cast<uop_fbin *>(inst.get());
            if (fop != nullptr) {
                colle[fop] = iiter;
            } else {
                continue;
            }
            if (fop->kind == FBIN_KIND::ADD) {
                if (auto lfexp = dynamic_cast<uop_fbin *>(fop->lhs->from);
                    lfexp != nullptr && fop->lhs->end == fop->uop_idx) {
                    if (lfexp->kind == FBIN_KIND::MUL && lfexp->lhs->kind == VREG_KIND::REG &&
                        lfexp->rhs->kind == VREG_KIND::REG && fop->rhs->kind == VREG_KIND::REG) {
                        auto op = std::make_unique<uop_ftri>();
                        op->kind = FTRI_KIND::MADD;
                        op->op1 = lfexp->lhs;
                        op->op2 = lfexp->rhs;
                        op->op3 = fop->rhs;
                        op->dst = fop->dst;
                        op->uop_idx = fop->uop_idx;
                        inst = std::move(op);

                        rlp->vreg_alloc.storage.remove_if([fop](virt_reg_ptr &ptr) -> bool { return ptr.get() == fop->lhs; });
                        auto former = colle[lfexp];
                        bb->ops.erase(former);
                    }
                } else if (auto rfexp = dynamic_cast<uop_fbin *>(fop->rhs->from);
                           rfexp != nullptr && fop->rhs->end == fop->uop_idx) {
                    if (rfexp->kind == FBIN_KIND::MUL && rfexp->lhs->kind == VREG_KIND::REG &&
                        rfexp->rhs->kind == VREG_KIND::REG && fop->lhs->kind == VREG_KIND::REG) {
                        auto op = std::make_unique<uop_ftri>();
                        op->kind = FTRI_KIND::MADD;
                        op->op1 = rfexp->lhs;
                        op->op2 = rfexp->rhs;
                        op->op3 = fop->lhs;
                        op->dst = fop->dst;
                        op->uop_idx = fop->uop_idx;
                        inst = std::move(op);

                        rlp->vreg_alloc.storage.remove_if([fop](virt_reg_ptr &ptr) -> bool { return ptr.get() == fop->rhs; });
                        auto former = colle[rfexp];
                        bb->ops.erase(former);
                    }
                }
            } else if (fop->kind == FBIN_KIND::SUB) {
                if (auto lfexp = dynamic_cast<uop_fbin *>(fop->lhs->from);
                    lfexp != nullptr && fop->lhs->end == fop->uop_idx) {
                    if (lfexp->kind == FBIN_KIND::MUL && lfexp->lhs->kind == VREG_KIND::REG &&
                        lfexp->rhs->kind == VREG_KIND::REG && fop->rhs->kind == VREG_KIND::REG) {
                        auto op = std::make_unique<uop_ftri>();
                        op->kind = FTRI_KIND::MSUB;
                        op->op1 = lfexp->lhs;
                        op->op2 = lfexp->rhs;
                        op->op3 = fop->rhs;
                        op->dst = fop->dst;
                        op->uop_idx = fop->uop_idx;
                        inst = std::move(op);

                        rlp->vreg_alloc.storage.remove_if([fop](virt_reg_ptr &ptr) -> bool { return ptr.get() == fop->lhs; });
                        auto former = colle[lfexp];
                        bb->ops.erase(former);
                    }
                } else if (auto rfexp = dynamic_cast<uop_fbin *>(fop->rhs->from);
                           rfexp != nullptr && fop->rhs->end == fop->uop_idx) {
                    if (rfexp->kind == FBIN_KIND::MUL && rfexp->lhs->kind == VREG_KIND::REG &&
                        rfexp->rhs->kind == VREG_KIND::REG && fop->lhs->kind == VREG_KIND::REG) {
                        auto op = std::make_unique<uop_ftri>();
                        op->kind = FTRI_KIND::NMSUB;
                        op->op1 = rfexp->lhs;
                        op->op2 = rfexp->rhs;
                        op->op3 = fop->lhs;
                        op->dst = fop->dst;
                        op->uop_idx = fop->uop_idx;
                        inst = std::move(op);

                        rlp->vreg_alloc.storage.remove_if([fop](virt_reg_ptr &ptr) -> bool { return ptr.get() == fop->rhs; });
                        auto former = colle[rfexp];
                        bb->ops.erase(former);
                    }
                }
            }

        } while (++iiter != bb->ops.end());

    } while (++biter != rlp->bbs.end());

    biter = rlp->bbs.begin();

    std::unordered_map<uop_cmp *, std::list<std::shared_ptr<uop_general>>::iterator> cmp_colle;

    do {
        auto &&bb = *biter;

        auto iiter = bb->ops.begin();

        do {
            auto &&inst = *iiter;

            if (auto cmpop = dynamic_cast<uop_cmp *>(inst.get()); cmpop != nullptr) {
                cmp_colle[cmpop] = iiter;
            } else if (auto bop = dynamic_cast<uop_b *>(inst.get()); bop != nullptr) {
                if (bop->cond->end != bop->uop_idx && bop->cond->ref <= 1) {
                    continue;
                }

                auto from = dynamic_cast<uop_cmp *>(bop->cond->from);
                if (from == nullptr || from->onflt) {
                    continue;
                }

                auto op = std::make_unique<uop_cmp_b>();
                op->pfx = bop->pfx;
                op->lbid = bop->lbid;
                switch (from->kind) {
                    case COMP_KIND::EQU:
                        op->lhs = from->lhs;
                        op->rhs = from->rhs;
                        op->kind = COMP_KIND::NEQ;
                        break;
                    case COMP_KIND::NEQ:
                        op->lhs = from->lhs;
                        op->rhs = from->rhs;
                        op->kind = COMP_KIND::EQU;
                        break;
                    case COMP_KIND::LTH:
                        op->lhs = from->lhs;
                        op->rhs = from->rhs;
                        op->kind = COMP_KIND::GEQ;
                        break;
                    case COMP_KIND::GTH:
                        op->lhs = from->rhs;
                        op->rhs = from->lhs;
                        op->kind = COMP_KIND::GEQ;
                        break;
                    case COMP_KIND::LEQ:
                        op->lhs = from->rhs;
                        op->rhs = from->lhs;
                        op->kind = COMP_KIND::LTH;
                        break;
                    case COMP_KIND::GEQ:
                        op->lhs = from->lhs;
                        op->rhs = from->rhs;
                        op->kind = COMP_KIND::LTH;
                        break;
                    default:
                        panic("unexpected");
                }

                inst = std::move(op);
                rlp->vreg_alloc.storage.remove_if([bop](virt_reg_ptr &ptr) -> bool { return ptr.get() == bop->cond; });
                auto former = cmp_colle[from];
                bb->ops.erase(former);
            }

        } while (++iiter != bb->ops.end());

    } while (++biter != rlp->bbs.end());
}

void rl_lltorl_instance::genstk() {
    rlp->vski_alloc.plan(mxpsiz);

    auto op0 = std::make_unique<uop_store>();
    op0->base = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);
    op0->off = -8;
    op0->onflt = false;
    op0->src = rlp->vreg_alloc.alloc(VREG_KIND::RA, 0, 0);

    auto op1 = std::make_unique<uop_store>();
    op1->base = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);
    op1->off = -16;
    op1->onflt = false;
    op1->src = rlp->vreg_alloc.alloc(VREG_KIND::FP, 0, 0);

    auto op2 = std::make_unique<uop_mv>();
    op2->src = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);
    op2->dst = rlp->vreg_alloc.alloc(VREG_KIND::FP, 0, 0);

    auto op3 = std::make_unique<uop_bin>();
    op3->lhs = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);
    op3->rhs = rlp->vreg_alloc.alloc(VREG_KIND::IMM, 4, rlp->vski_alloc.total_stk_len);
    op3->kind = IBIN_KIND::SUB;
    op3->dst = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);

    auto &&front_ilst = rlp->bbs.front()->ops;
    front_ilst.push_front(std::move(op3));
    front_ilst.push_front(std::move(op2));
    front_ilst.push_front(std::move(op1));
    front_ilst.push_front(std::move(op0));

    auto &&back_ilst = rlp->bbs.back()->ops;
    auto last = back_ilst.back();
    auto retinst = std::dynamic_pointer_cast<uop_ret>(last);
    Assert(retinst, "last inst not ret");

    back_ilst.pop_back();

    auto op4 = std::make_unique<uop_bin>();
    op4->lhs = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);
    op4->rhs = rlp->vreg_alloc.alloc(VREG_KIND::IMM, 4, rlp->vski_alloc.total_stk_len);
    op4->kind = IBIN_KIND::ADD;
    op4->dst = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);

    auto op5 = std::make_unique<uop_load>();
    op5->base = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);
    op5->off = -8;
    op5->onflt = false;
    op5->dst = rlp->vreg_alloc.alloc(VREG_KIND::RA, 0, 0);

    auto op6 = std::make_unique<uop_load>();
    op6->base = rlp->vreg_alloc.alloc(VREG_KIND::SP, 0, 0);
    op6->off = -16;
    op6->onflt = false;
    op6->dst = rlp->vreg_alloc.alloc(VREG_KIND::FP, 0, 0);

    back_ilst.push_back(std::move(op4));
    back_ilst.push_back(std::move(op5));
    back_ilst.push_back(std::move(op6));
    back_ilst.push_back(std::move(retinst));
}

void rl_lltorl_instance::loadimm() {
    auto biter = rlp->bbs.begin();

    do {
        auto &&bb = *biter;

        auto iiter = bb->ops.begin();

        do {
            auto &&inst = *iiter;

            auto res = inst->load_immediate(rlp->vreg_alloc);
            if (res != nullptr) {
                bb->ops.insert(iiter, std::move(res));
            }

        } while (++iiter != bb->ops.end());

    } while (++biter != rlp->bbs.end());
}

bool rl_lltorl_instance::assign() {
    return false;
}
