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
                        // 将跳转目的地转换
                        jopx->lbid = jop->lbid;

                        // 将其加入新目的地的引用中
                        target_record.refs.push_back(jopx);
                    } else if (auto &&bopx = dynamic_cast<uop_b *>(op); bopx != nullptr) {
                        // 将跳转目的地转换
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

void rl_lltorl_instance::ireidx() {
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
    for (auto &&prm: rlp->params) {
        auto op = std::make_unique<uop_load>();
        auto fp = rlp->vreg_alloc.alloc(VREG_KIND::FP, 0, 0);
        Assert(fp->kind == VREG_KIND::FP, "fp mistake");
        op->dst = prm;
        op->base = fp;
        op->off = off;
        top->ops.push_front(std::move(op));
        off += prm->len;
    }
}