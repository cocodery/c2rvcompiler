#include "3tle3wa/backend/ir/bbtype.hh"
#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/xin/xin.hh"

static size_t file_scope_label_alloc() {
    static std::mutex fslamtx;
    static size_t lbidx_ = 1;
    std::scoped_lock<std::mutex> lck{fslamtx};
    auto ret = lbidx_;
    lbidx_ += 1;
    return ret;
}

void cross_internal_manager::nftoir() {
    struct {
        size_t i{0};
        size_t f{0};
        size_t pstk{0};
    } pa;

    using glb_la_info = std::pair<size_t, size_t>;

    // 有脏东西 /(ㄒoㄒ)/~~
    auto alloc_reg = fptr_->GetVarIdx() + 0xdeadbeef;

    for (auto &&param : fptr_->GetParamList()) {
        auto var = std::dynamic_pointer_cast<Variable>(param);
        Assert(var, "input param is not variable");

        auto &&vartype = var->GetBaseType();
        if (vartype->IsPointer()) {
            auto nwvr = rl_pgrs_.valc_.alloc_prm(VREG_TYPE::PTR, var->GetVariableIdx(), pa.i);
            if (pa.i < 8) {
                nwvr->set_param_pos(pa.i);
                nwvr->set_rregid(pa.i + riscv::a0);
            } else {
                nwvr->set_rregid(riscv::zero);
                nwvr->set_param_pos(pa.i);
                nwvr->set_onstk(true);
                nwvr->set_pstk(pa.pstk);
                pa.pstk += 1;
            }

            nwvr->set_confirm(true);
            rl_pgrs_.params_.push_back(nwvr);
            pa.i += 1;
        } else if (vartype->IntType()) {
            auto nwvr = rl_pgrs_.valc_.alloc_prm(VREG_TYPE::INT, var->GetVariableIdx(), pa.i);
            if (pa.i < 8) {
                nwvr->set_param_pos(pa.i);
                nwvr->set_rregid(pa.i + riscv::a0);
            } else {
                nwvr->set_rregid(riscv::zero);
                nwvr->set_param_pos(pa.i);
                nwvr->set_onstk(true);
                nwvr->set_pstk(pa.pstk);
                pa.pstk += 1;
            }

            nwvr->set_confirm(true);
            rl_pgrs_.params_.push_back(nwvr);
            pa.i += 1;
        } else if (vartype->FloatType()) {
            auto nwvr = rl_pgrs_.valc_.alloc_prm(VREG_TYPE::FLT, var->GetVariableIdx(), pa.f);
            if (pa.f < 8) {
                nwvr->set_param_pos(pa.f);
                nwvr->set_rregid(pa.f + riscv::fa0);
            } else {
                nwvr->set_rregid(riscv::zero);
                nwvr->set_param_pos(pa.f);
                nwvr->set_onstk(true);
                nwvr->set_pstk(pa.pstk);
                pa.pstk += 1;
            }

            nwvr->set_confirm(true);
            rl_pgrs_.params_.push_back(nwvr);
            pa.f += 1;
        } else {
            panic("Unexpected parameter type");
        }
    }
    auto &&pu = rl_pgrs_.puse_;

    pu.i = std::max(pu.i, pa.i);
    pu.f = std::max(pu.f, pa.f);

    std::unordered_map<size_t, size_t> reidx_lbmap;

    for (auto &&bb : fptr_->TopoSortFromEntry()) {
        auto curbb_lbid = bb->GetBlockIdx();
        reidx_lbmap[curbb_lbid] = file_scope_label_alloc();
    }
    auto &&topo = fptr_->TopoSortFromEntry();
    auto bbit = topo.begin();

    while (bbit != topo.end()) {
        std::map<glb_la_info, size_t> glb_la_history;

        auto &&bb = (*bbit);
        ++bbit;
        size_t nxtlbid;
        if (bbit == topo.end()) {
            nxtlbid = reidx_lbmap[bb->GetBlockIdx()];
        } else {
            nxtlbid = reidx_lbmap[(*bbit)->GetBlockIdx()];
        }

        auto &&rlbb = std::make_unique<rl_basicblock>();
        auto &&lst = rlbb->ops_;

        auto curbb_lbid = bb->GetBlockIdx();

        if (auto fnd = reidx_lbmap.find(curbb_lbid); fnd != reidx_lbmap.end()) {
            rlbb->set_lbid(fnd->second);
        } else {
            panic("unexpected");
        }
        rl_pgrs_.lbmap_[rlbb->get_lbid()].bbp_ = rlbb.get();

        for (auto &&succ : bb->GetSuccessors()) {
            auto idx = succ->GetBlockIdx();
            rlbb->successer.insert(reidx_lbmap[idx]);
        }

        for (auto &&inst : bb->GetInstList()) {
            const auto opcode = inst->GetOpCode();
            switch (opcode) {
                case None:
                    break;

                case Ret: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<ReturnInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    if (auto &&retval = llinst->GetRetValue(); retval != nullptr) {
                        // 如果有返回值
                        // 构造指令
                        if (retval->GetBaseType()->FloatType()) {
                            // 如果是浮点数返回值
                            auto op0 = std::make_unique<uop_set_fparam>();
                            op0->set_idx(0);

                            if (retval->IsVariable()) {
                                // 如果返回值是 变量

                                // 转换，并按照该变量信息构造一个虚拟寄存器
                                // 该虚拟寄存器应该已经存在
                                auto var = std::dynamic_pointer_cast<Variable>(retval);
                                Assert(var, "bad dynamic cast");

                                // 保证该变量是存在的
                                auto evr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());

                                // 注册到返回值
                                op0->set_rs(evr);
                            } else if (retval->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(retval);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());

                                auto nwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);

                                // 注册到返回值
                                op0->set_rs(nwvr);
                            } else {
                                panic("unexpected");
                            }

                            lst.push_back(std::move(op0));
                        } else if (retval->GetBaseType()->IntType()) {
                            // 如果是整数返回值
                            auto op0 = std::make_unique<uop_set_iparam>();
                            op0->set_idx(0);

                            if (retval->IsVariable()) {
                                // 如果返回值是 变量

                                // 转换，并按照该变量信息构造一个虚拟寄存器
                                // 该虚拟寄存器应该已经存在
                                auto var = std::dynamic_pointer_cast<Variable>(retval);
                                Assert(var, "bad dynamic cast");

                                // 保证该变量是存在的
                                auto evr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());

                                // 注册到返回值
                                op0->set_rs(evr);
                            } else if (retval->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(retval);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());

                                auto nwvr = rl_pgrs_.valc_.alloc_imm(pk.v32);

                                // 注册到返回值
                                op0->set_rs(nwvr);
                            } else {
                                panic("unexpected");
                            }

                            lst.push_back(std::move(op0));
                        } else {
                            panic("unexpected");
                        }
                    }

                    // 必定有的 ret
                    auto op1 = std::make_unique<uop_ret>();
                    lst.push_back(std::move(op1));
                } break;

                case Jump: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<JumpInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    // 构造指令
                    auto op = std::make_unique<uop_j>();

                    auto curtgt_lbid = llinst->GetTarget()->GetBlockIdx();

                    if (auto fnd = reidx_lbmap.find(curtgt_lbid); fnd != reidx_lbmap.end()) {
                        op->set_lbid(fnd->second);
                    } else {
                        panic("unexpected");
                    }

                    // 记录一次对标签的引用
                    rl_pgrs_.lbmap_[op->get_lbid()].refs_.push_back(op.get());

                    // 将操作置入当前块中
                    lst.push_back(std::move(op));
                } break;

                case Branch: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<BranchInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    bool ontrue = false;

                    // 获取条件，构造标签
                    auto &&cond = llinst->GetCondition();
                    size_t tgid = 0;
                    size_t otherid = 0;
                    auto ftgid = llinst->GetFalseTarget()->GetBlockIdx();
                    auto ttgid = llinst->GetTrueTarget()->GetBlockIdx();

                    if (nxtlbid == reidx_lbmap[ftgid]) {
                        ontrue = true;
                        tgid = ttgid;
                        otherid = ftgid;
                    } else if (nxtlbid == reidx_lbmap[ttgid]) {
                        ontrue = false;
                        tgid = ftgid;
                        otherid = ttgid;
                    }

                    if (cond->IsVariable()) {
                        // 如果条件是一个变量
                        // 转换类型，检查转换结果
                        auto var = std::dynamic_pointer_cast<Variable>(cond);
                        Assert(var, "bad dynamic cast");

                        // 构造指令
                        auto op = std::make_unique<uop_b>();
                        op->set_ontrue(ontrue);

                        // 条件应当已经存在
                        auto evr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                        op->set_cond(evr);

                        op->set_lbid(reidx_lbmap.at(tgid));

                        // 记录一次对标签的引用
                        rl_pgrs_.lbmap_[op->get_lbid()].refs_.push_back(op.get());
                        rl_pgrs_.lbmap_[reidx_lbmap.at(otherid)].refs_.push_back(op.get());

                        op->set_false_lbid(reidx_lbmap.at(otherid));

                        // 将操作置入当前块中
                        lst.push_back(std::move(op));
                    } else if (cond->IsConstant()) {
                        auto cst = std::dynamic_pointer_cast<Constant>(cond);
                        Assert(cst, "bad dynamic cast");

                        auto &&pk = xcval(cst->GetValue());

                        if (pk.v32 != 0) {
                            // 构造 J 指令
                            auto op = std::make_unique<uop_j>();

                            // 设置目的地标签
                            op->set_lbid(reidx_lbmap.at(ttgid));

                            // 记录一次对标签的引用
                            rl_pgrs_.lbmap_[op->get_lbid()].refs_.push_back(op.get());

                            rlbb->successer.erase(reidx_lbmap.at(ftgid));

                            // 将操作置入当前块中
                            lst.push_back(std::move(op));
                        } else {
                            // 构造 J 指令
                            auto op = std::make_unique<uop_j>();

                            // 设置目的地标签
                            op->set_lbid(reidx_lbmap.at(ftgid));

                            // 记录一次对标签的引用
                            rl_pgrs_.lbmap_[op->get_lbid()].refs_.push_back(op.get());

                            rlbb->successer.erase(reidx_lbmap.at(ttgid));

                            // 将操作置入当前块中
                            lst.push_back(std::move(op));
                        }
                    } else {
                        panic("unexpected");
                    }
                } break;

                case Alloca: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<AllocaInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    // 该指令主要用于在栈上开辟一段内存使用
                    // 为了压缩栈的大小，应该对类型做严格的检查
                    // 其中，指针类型占 8 个字节，其余类型占 4 个字节，这需要注意

                    virt_reg *nwvr = nullptr;

                    if (auto &&type = llinst->GetAllocaType(); type->IsScalar()) {
                        if (type->IntType()) {
                            nwvr = rl_pgrs_.valc_.alloc_stk(VREG_TYPE::INT, 8);
                        } else if (type->FloatType()) {
                            nwvr = rl_pgrs_.valc_.alloc_stk(VREG_TYPE::FLT, 8);
                        } else {
                            panic("unexpected");
                        }
                    } else if (type->IsArray()) {
                        auto lstty = std::dynamic_pointer_cast<ListType>(type);
                        Assert(lstty, "bad dynamic cast");

                        nwvr = rl_pgrs_.valc_.alloc_stk(VREG_TYPE::ARR, lstty->GetCapacity());
                    } else {
                        panic("unexpected");
                    }

                    // 该 mv 操作应该翻译成一个 add 或者 addi 操作
                    // 注意，是 64 位操作
                    auto recv = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::PTR, llinst->GetResult()->GetVariableIdx());
                    auto op = std::make_unique<uop_mv>();
                    op->set_rd(recv);
                    op->set_rs(nwvr);

                    lst.push_back(std::move(op));
                } break;

                case Store: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<StoreInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    // 先分析出需要保存的数值
                    virt_reg *nwvr = nullptr;
                    auto &&sval = llinst->GetStoreValue();
                    bool onflt = false;

                    if (sval->IsVariable()) {
                        auto var = std::dynamic_pointer_cast<Variable>(sval);
                        Assert(var, "bad dynamic cast");

                        if (auto stype = var->GetBaseType(); stype->FloatType()) {
                            onflt = true;
                            nwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                        } else if (stype->IntType()) {
                            nwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                        } else {
                            panic("unexpected");
                        }
                    } else if (sval->IsConstant()) {
                        auto cst = std::dynamic_pointer_cast<Constant>(sval);
                        Assert(cst, "bad dynamic cast");

                        auto &&pk = xcval(cst->GetValue());
                        if (cst->GetBaseType()->FloatType()) {
                            onflt = true;
                            nwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);
                        } else {
                            nwvr = rl_pgrs_.valc_.alloc_imm(pk.v32);
                        }
                    } else {
                        panic("unexpected");
                    }

                    auto &&aval = llinst->GetStoreAddr();
                    if (aval->IsGlobalValue()) {
                        auto gval = std::dynamic_pointer_cast<GlobalValue>(aval);
                        auto &&gidx = gval->GetGlobalValueIdx();
                        if (onflt) {
                            // 这里未来需要指派一个非 zero 的寄存器
                            auto reserve = rl_pgrs_.valc_.alloc_zero();

                            auto op = std::make_unique<uop_fst_l>();
                            op->set_glb_idx(gidx);
                            op->set_rt(reserve);
                            op->set_rd(nwvr);

                            lst.push_back(std::move(op));
                        } else {
                            // 这里未来需要指派一个非 zero 的寄存器
                            auto reserve = rl_pgrs_.valc_.alloc_zero();

                            auto op = std::make_unique<uop_st_l>();
                            op->set_glb_idx(gidx);
                            op->set_rt(reserve);
                            op->set_rd(nwvr);

                            lst.push_back(std::move(op));
                        }
                    } else if (aval->IsVariable()) {
                        auto avar = std::dynamic_pointer_cast<Variable>(aval);
                        Assert(avar, "store addr should be var");
                        if (onflt) {
                            auto op = std::make_unique<uop_fst>();
                            op->set_rb(rl_pgrs_.valc_.get_reg(avar->GetVariableIdx()));
                            op->set_rd(nwvr);

                            // 目前认为 offset 都被计算包含在了 base 里
                            op->set_off(0);

                            lst.push_back(std::move(op));
                        } else {
                            auto op = std::make_unique<uop_st>();
                            op->set_rb(rl_pgrs_.valc_.get_reg(avar->GetVariableIdx()));
                            op->set_rd(nwvr);

                            // 目前认为 offset 都被计算包含在了 base 里
                            op->set_off(0);

                            lst.push_back(std::move(op));
                        }
                    } else {
                        panic("寄了");
                    }
                } break;

                case Load: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<LoadInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    // 先分析出需要保存的数值
                    auto &&res = llinst->GetResult();
                    Assert(not res->GetBaseType()->IsPointer(), "assuming it will not be a pointer");

                    virt_reg *nwvr = nullptr;
                    bool onflt = false;
                    if (auto rtype = res->GetBaseType(); rtype->FloatType()) {
                        onflt = true;
                        nwvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::FLT, res->GetVariableIdx());
                    } else if (rtype->IntType()) {
                        nwvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::INT, res->GetVariableIdx());
                    }

                    auto &&aval = llinst->GetOprand();
                    if (aval->IsGlobalValue()) {
                        auto gval = std::dynamic_pointer_cast<GlobalValue>(aval);
                        Assert(gval, "bad dynamic cast");
                        auto gidx = gval->GetGlobalValueIdx();

                        if (onflt) {
                            auto op = std::make_unique<uop_fld_l>();
                            op->set_glb_idx(gidx);
                            op->set_rd(nwvr);

                            lst.push_back(std::move(op));
                        } else {
                            auto op = std::make_unique<uop_ld_l>();
                            op->set_glb_idx(gidx);
                            op->set_rd(nwvr);

                            lst.push_back(std::move(op));
                        }
                    } else if (aval->IsVariable()) {
                        auto avar = std::dynamic_pointer_cast<Variable>(aval);
                        Assert(avar, "store addr should be var");

                        if (onflt) {
                            auto op = std::make_unique<uop_fld>();
                            op->set_rb(rl_pgrs_.valc_.get_reg(avar->GetVariableIdx()));
                            op->set_rd(nwvr);
                            op->set_off(0);

                            lst.push_back(std::move(op));
                        } else {
                            auto op = std::make_unique<uop_ld>();
                            op->set_rb(rl_pgrs_.valc_.get_reg(avar->GetVariableIdx()));
                            op->set_rd(nwvr);
                            op->set_off(0);

                            lst.push_back(std::move(op));
                        }
                    } else {
                        panic("unexpected");
                    }

                } break;

                case Gep: {
                    auto llinst = std::dynamic_pointer_cast<GetElementPtrInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&res = llinst->GetResult();
                    // 这里加载的是指针值，所以宽度用 8
                    auto nwvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::PTR, res->GetVariableIdx());

                    virt_reg *off = nullptr;
                    auto &&offlist = llinst->GetOffList();
                    auto &&offset = offlist.back();
                    if (offset->IsVariable()) {
                        auto var = std::dynamic_pointer_cast<Variable>(offset);
                        Assert(var, "bad dynamic cast");

                        auto tmp = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());

                        off = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::PTR, alloc_reg++);
                        auto tmp2 = rl_pgrs_.valc_.alloc_imm(2);

                        auto op0 = std::make_unique<uop_bin>();
                        op0->set_kind(IBIN_KIND::SLL);
                        op0->set_lhs(tmp);
                        op0->set_rhs(tmp2);
                        op0->set_rd(off);

                        lst.push_back(std::move(op0));

                    } else if (offset->IsConstant()) {
                        auto cst = std::dynamic_pointer_cast<Constant>(offset);
                        Assert(cst, "bad dynamic cast");

                        auto &&pk = xcval(cst->GetValue());

                        off = rl_pgrs_.valc_.alloc_imm(pk.v32 * 4);
                    } else {
                        panic("unexpected");
                    }

                    auto &&base = llinst->GetBaseAddr();
                    if (base->IsGlobalValue()) {
                        auto gval = std::dynamic_pointer_cast<GlobalValue>(base);
                        Assert(gval, "bad dynamic cast");

                        auto gvidx = gval->GetGlobalValueIdx();

                        glb_la_info gli;

                        if (off->kind() == VREG_KIND::IMM) {
                            gli = glb_la_info(gvidx, off->value());

                            if (auto fnd = glb_la_history.find(gli); fnd != glb_la_history.end()) {
                                rl_pgrs_.valc_.link(nwvr->value(), fnd->second);
                                break;
                            }
                        } else {
                            gli = glb_la_info(gvidx, 0);

                            if (auto fnd = glb_la_history.find(gli); fnd != glb_la_history.end()) {
                                auto tmp = rl_pgrs_.valc_.get_reg(fnd->second);

                                auto opx = std::make_unique<uop_bin>();
                                opx->set_kind(IBIN_KIND::ADD);
                                opx->set_lhs(tmp);
                                opx->set_rhs(off);
                                opx->set_rd(nwvr);
                                lst.push_back(std::move(opx));
                                break;
                            }
                        }

                        auto tmp = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::PTR, alloc_reg++);

                        auto op0 = std::make_unique<uop_lla>();
                        op0->set_glb_idx(gvidx);
                        op0->set_rd(tmp);

                        lst.push_back(std::move(op0));

                        auto op1 = std::make_unique<uop_bin>();
                        op1->set_kind(IBIN_KIND::ADD);
                        op1->set_lhs(tmp);
                        op1->set_rhs(off);
                        op1->set_rd(nwvr);

                        if (off->kind() == VREG_KIND::IMM) {
                            glb_la_history[gli] = nwvr->value();
                        } else {
                            glb_la_history[gli] = tmp->value();
                        }

                        lst.push_back(std::move(op1));
                    } else if (base->IsVariable()) {
                        auto avar = std::dynamic_pointer_cast<Variable>(base);
                        Assert(avar, "bad dynamic cast");

                        auto op = std::make_unique<uop_bin>();
                        op->set_kind(IBIN_KIND::ADD);
                        op->set_lhs(rl_pgrs_.valc_.get_reg(avar->GetVariableIdx()));
                        op->set_rhs(off);
                        op->set_rd(nwvr);

                        lst.push_back(std::move(op));
                    } else {
                        panic("unexpected");
                    }

                } break;

                case Call: {
                    rl_pgrs_.contain_funcall_ = true;

                    auto llinst = std::dynamic_pointer_cast<CallInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&params = llinst->GetParamList();
                    auto fnn = llinst->GetCalleeFunc()->GetFuncName();

                    auto op0 = std::make_unique<uop_call>();

                    pa = {0, 0};

                    // @attention 这里假设函数名称不会提早释放，请注意
                    op0->set_callee(llinst->GetCalleeFunc()->GetFuncName().c_str());

                    if (fnn.substr(0, 4) == "llvm") {
                        // 对 llvm 特定函数处理
                        if (fnn == "llvm.memset.p0i8.i64") {
                            op0->set_callee("memset");

                            for (auto &&prm : params) {
                                if (auto &&ptype = prm->GetBaseType(); ptype->IntType() or ptype->CharType()) {
                                    auto op1 = std::make_unique<uop_set_iparam>();
                                    op1->set_idx(pa.i);
                                    pa.i += 1;

                                    if (prm->IsVariable()) {
                                        auto var = std::dynamic_pointer_cast<Variable>(prm);
                                        Assert(var, "bad dynamic cast");

                                        op1->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                                    } else {
                                        auto cst = std::dynamic_pointer_cast<Constant>(prm);
                                        Assert(cst, "bad dynamic cast");

                                        auto &&pk = xcval(cst->GetValue());

                                        op1->set_rs(rl_pgrs_.valc_.alloc_imm(pk.v32));
                                    }

                                    lst.push_back(std::move(op1));
                                } else if (ptype->IsPointer()) {
                                    auto op1 = std::make_unique<uop_set_iparam>();
                                    op1->set_idx(pa.i);
                                    pa.i += 1;

                                    if (prm->IsVariable()) {
                                        auto var = std::dynamic_pointer_cast<Variable>(prm);
                                        Assert(var, "bad dynamic cast");

                                        op1->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                                    } else {
                                        auto cst = std::dynamic_pointer_cast<Constant>(prm);
                                        Assert(cst, "bad dynamic cast");

                                        auto &&pk = xcval(cst->GetValue());

                                        op1->set_rs(rl_pgrs_.valc_.alloc_imm(pk.v32));
                                    }

                                    lst.push_back(std::move(op1));
                                } else if (ptype->GetAttrType() == ATTR_TYPE::INT64) {
                                    auto op1 = std::make_unique<uop_set_iparam>();
                                    op1->set_idx(pa.i);
                                    pa.i += 1;

                                    if (prm->IsVariable()) {
                                        auto var = std::dynamic_pointer_cast<Variable>(prm);
                                        Assert(var, "bad dynamic cast");

                                        op1->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                                    } else {
                                        auto cst = std::dynamic_pointer_cast<Constant>(prm);
                                        Assert(cst, "bad dynamic cast");

                                        auto &&pk = xcval(cst->GetValue());

                                        op1->set_rs(rl_pgrs_.valc_.alloc_imm(pk.v32 + pk.v64));
                                    }

                                    lst.push_back(std::move(op1));
                                } else if (ptype->FloatType()) {
                                    auto op1 = std::make_unique<uop_set_fparam>();
                                    op1->set_idx(pa.f);
                                    pa.f += 1;

                                    if (prm->IsVariable()) {
                                        auto var = std::dynamic_pointer_cast<Variable>(prm);
                                        Assert(var, "bad dynamic cast");

                                        op1->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                                    } else {
                                        auto cst = std::dynamic_pointer_cast<Constant>(prm);
                                        Assert(cst, "bad dynamic cast");

                                        auto &&pk = xcval(cst->GetValue());

                                        op1->set_rs(rl_pgrs_.valc_.alloc_loc(pk.v32));
                                    }

                                    lst.push_back(std::move(op1));
                                } else {
                                    panic("unexpected");
                                }

                                // 这里早停一下，参数没那么多
                                if (pa.f + pa.i >= 3) {
                                    break;
                                }
                            }
                        }
                    } else if (not params.empty()) {
                        for (auto &&prm : params) {
                            if (auto &&ptype = prm->GetBaseType(); ptype->IntType()) {
                                auto op1 = std::make_unique<uop_set_iparam>();
                                op1->set_idx(pa.i);
                                if (pa.i > 7) {
                                    op1->set_pstk(pa.pstk);
                                    pa.pstk += 1;
                                }
                                pa.i += 1;

                                if (prm->IsVariable()) {
                                    auto var = std::dynamic_pointer_cast<Variable>(prm);
                                    Assert(var, "bad dynamic cast");

                                    op1->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                                } else {
                                    auto cst = std::dynamic_pointer_cast<Constant>(prm);
                                    Assert(cst, "bad dynamic cast");

                                    auto &&pk = xcval(cst->GetValue());

                                    op1->set_rs(rl_pgrs_.valc_.alloc_imm(pk.v32));
                                }

                                lst.push_back(std::move(op1));
                            } else if (ptype->IsPointer()) {
                                auto op1 = std::make_unique<uop_set_iparam>();
                                op1->set_idx(pa.i);
                                if (pa.i > 7) {
                                    op1->set_pstk(pa.pstk);
                                    pa.pstk += 1;
                                }
                                pa.i += 1;

                                if (prm->IsVariable()) {
                                    auto var = std::dynamic_pointer_cast<Variable>(prm);
                                    Assert(var, "bad dynamic cast");

                                    op1->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                                }

                                lst.push_back(std::move(op1));
                            } else if (ptype->FloatType()) {
                                auto op1 = std::make_unique<uop_set_fparam>();
                                op1->set_idx(pa.f);
                                if (pa.f > 7) {
                                    op1->set_pstk(pa.pstk);
                                    pa.pstk += 1;
                                }
                                pa.f += 1;

                                if (prm->IsVariable()) {
                                    auto var = std::dynamic_pointer_cast<Variable>(prm);
                                    Assert(var, "bad dynamic cast");

                                    op1->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                                } else {
                                    auto cst = std::dynamic_pointer_cast<Constant>(prm);
                                    Assert(cst, "bad dynamic cast");

                                    auto &&pk = xcval(cst->GetValue());

                                    op1->set_rs(rl_pgrs_.valc_.alloc_loc(pk.v32));
                                }

                                lst.push_back(std::move(op1));
                            } else {
                                panic("unexpected");
                            }
                        }
                    }

                    auto &&extra = rl_pgrs_.valc_.ex_argl;
                    auto &&pu = rl_pgrs_.puse_;

                    pu.i = std::max(pu.i, pa.i);
                    pu.f = std::max(pu.f, pa.f);
                    extra = std::max(extra, pa.pstk);

                    if (auto &&res = llinst->GetResult(); res != nullptr) {
                        virt_reg *retvr;
                        if (res->GetBaseType()->FloatType()) {
                            retvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::FLT, res->GetVariableIdx());

                            auto &&pu = rl_pgrs_.puse_;
                            pu.f = pu.f > 1 ? pu.f : 1;

                            // retvr->set_rregid(riscv::fa0);
                        } else {
                            retvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::INT, res->GetVariableIdx());

                            auto &&pu = rl_pgrs_.puse_;
                            pu.i = pu.i > 1 ? pu.i : 1;

                            // retvr->set_rregid(riscv::a0);
                        }
                        // retvr->set_confirm(true);
                        op0->set_retval(retvr);
                    }

                    op0->set_rec(llinst->GetCalleeFunc()->GetRecursive());
                    lst.push_back(std::move(op0));
                } break;

                case Phi: {
                    panic("No de-Phi");
                } break;

                case SiToFp: {
                    // lhs -> rhs
                    // si  -> fp
                    auto llinst = std::dynamic_pointer_cast<SitoFpInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&rhs = llinst->GetResult();

                    auto &&lhs = llinst->GetOprand();
                    auto var = std::dynamic_pointer_cast<Variable>(lhs);
                    Assert(var, "cvt should op on var");

                    auto op = std::make_unique<uop_cvtw2s>();
                    op->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                    op->set_rd(rl_pgrs_.valc_.alloc_reg(VREG_TYPE::FLT, rhs->GetVariableIdx()));

                    lst.push_back(std::move(op));
                } break;

                case FpToSi: {
                    // lhs -> rhs
                    // fp  -> si
                    auto llinst = std::dynamic_pointer_cast<FptoSiInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&rhs = llinst->GetResult();

                    auto &&lhs = llinst->GetOprand();
                    auto var = std::dynamic_pointer_cast<Variable>(lhs);
                    Assert(var, "cvt should op on var");

                    auto op = std::make_unique<uop_cvts2w>();
                    op->set_rs(rl_pgrs_.valc_.get_reg(var->GetVariableIdx()));
                    op->set_rd(rl_pgrs_.valc_.alloc_reg(VREG_TYPE::INT, rhs->GetVariableIdx()));

                    lst.push_back(std::move(op));
                } break;

                case BitCast: {
                    auto llinst = std::dynamic_pointer_cast<BitCastInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&oprand = llinst->GetOprand();
                    auto var = std::dynamic_pointer_cast<Variable>(oprand);
                    Assert(var, "bitcast should op on var");

                    auto &&res = llinst->GetResult();

                    rl_pgrs_.valc_.link(res->GetVariableIdx(), var->GetVariableIdx());
                } break;

                case Zext: {
                    auto llinst = std::dynamic_pointer_cast<ZextInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&oprand = llinst->GetOprand();
                    auto var = std::dynamic_pointer_cast<Variable>(oprand);
                    Assert(var, "zext should op on var");

                    auto &&res = llinst->GetResult();

                    rl_pgrs_.valc_.link(res->GetVariableIdx(), var->GetVariableIdx());
                } break;

                case OP_ADD:
                case OP_SUB:
                case OP_MUL:
                case OP_DIV:
                case OP_REM:
                case OP_LSHIFT:
                case OP_RSHIFT: {
                    auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&res = llinst->GetResult();
                    auto lhs = llinst->GetLHS();
                    auto rhs = llinst->GetRHS();

                    bool linkable = false;

                    if (auto &&rtype = res->GetBaseType(); rtype->IntType() or rtype->BoolType()) {
                        if (lhs->IsConstant() and rhs->IsConstant()) {
                            auto lcst = std::dynamic_pointer_cast<Constant>(lhs);
                            Assert(lcst, "bad dynamic cast");

                            auto &&lpk = xcval(lcst->GetValue());

                            auto rcst = std::dynamic_pointer_cast<Constant>(rhs);
                            Assert(rcst, "bad dynamic cast");

                            auto &&rpk = xcval(rcst->GetValue());

                            uint32_t out{};

                            switch (opcode) {
                                case OP_ADD:
                                    out = (int32_t)lpk.v32 + (int32_t)rpk.v32;
                                    break;
                                case OP_SUB:
                                    out = (int32_t)lpk.v32 - (int32_t)rpk.v32;
                                    break;
                                case OP_MUL:
                                    out = (int32_t)lpk.v32 * (int32_t)rpk.v32;
                                    break;
                                case OP_DIV:
                                    out = (int32_t)lpk.v32 / (int32_t)rpk.v32;
                                    break;
                                case OP_REM:
                                    out = (int32_t)lpk.v32 % (int32_t)rpk.v32;
                                    break;
                                case OP_LSHIFT:
                                    out = (int32_t)lpk.v32 << (int32_t)rpk.v32;
                                    break;
                                case OP_RSHIFT:
                                    out = (int32_t)lpk.v32 >> (int32_t)rpk.v32;
                                    break;
                                default:
                                    panic("unexpected");
                            }
                            auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::INT, res->GetVariableIdx());
                            virt_reg *nwvr = rl_pgrs_.valc_.alloc_imm(out);
                            auto op = std::make_unique<uop_mv>();
                            op->set_rd(resvr);
                            op->set_rs(nwvr);

                            lst.push_back(std::move(op));
                        } else {
                            virt_reg *lnwvr = nullptr;
                            virt_reg *rnwvr = nullptr;

                            if (lhs->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(lhs);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());
                                if (pk.v32 == 1 and opcode == OP_MUL) {
                                    linkable = true;
                                } else if (pk.v32 == 0 and opcode == OP_ADD) {
                                    linkable = true;
                                } else {
                                    lnwvr = rl_pgrs_.valc_.alloc_imm(pk.v32);
                                }
                            }

                            if (rhs->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(rhs);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());

                                if (pk.v32 == 1 and (opcode == OP_MUL or opcode == OP_DIV)) {
                                    linkable = true;
                                } else if (pk.v32 == 0 and (opcode == OP_ADD or opcode == OP_SUB or
                                                            opcode == OP_LSHIFT or opcode == OP_RSHIFT)) {
                                    linkable = true;
                                } else {
                                    rnwvr = rl_pgrs_.valc_.alloc_imm(pk.v32);
                                }
                            }

                            if (lhs->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(lhs);
                                Assert(var, "bad dynamic cast");

                                if (linkable) {
                                    Assert(rnwvr == nullptr, "rhs not imm");
                                    rl_pgrs_.valc_.link(res->GetVariableIdx(), var->GetVariableIdx());
                                    break;
                                }

                                lnwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                            }

                            if (rhs->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(rhs);
                                Assert(var, "bad dynamic cast");

                                if (linkable) {
                                    Assert(lnwvr == nullptr, "rhs not imm");
                                    rl_pgrs_.valc_.link(res->GetVariableIdx(), var->GetVariableIdx());
                                    break;
                                }

                                rnwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                            }

                            auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::INT, res->GetVariableIdx());
                            auto op = std::make_unique<uop_bin>();
                            op->set_kind((IBIN_KIND)opcode);
                            op->set_lhs(lnwvr);
                            op->set_rhs(rnwvr);
                            op->set_rd(resvr);

                            lst.push_back(std::move(op));
                        }

                    } else if (rtype->FloatType()) {
                        if (lhs->IsConstant() and rhs->IsConstant()) {
                            auto lcst = std::dynamic_pointer_cast<Constant>(lhs);
                            Assert(lcst, "bad dynamic cast");

                            auto &&lpk = xcval(lcst->GetValue());

                            auto rcst = std::dynamic_pointer_cast<Constant>(rhs);
                            Assert(rcst, "bad dynamic cast");

                            auto &&rpk = xcval(rcst->GetValue());

                            float cstval{};
                            switch (opcode) {
                                case OP_ADD:
                                    cstval = lpk.f32 + rpk.f32;
                                    break;
                                case OP_SUB:
                                    cstval = lpk.f32 - rpk.f32;
                                    break;
                                case OP_MUL:
                                    cstval = lpk.f32 * rpk.f32;
                                    break;
                                case OP_DIV:
                                    cstval = lpk.f32 / rpk.f32;
                                    break;
                                default:
                                    panic("unexpected");
                            }

                            auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::FLT, res->GetVariableIdx());

                            union {
                                float f;
                                uint32_t iu32;
                            } reintp;
                            reintp.f = cstval;
                            auto nwvr = rl_pgrs_.valc_.alloc_loc(reintp.iu32);

                            auto op = std::make_unique<uop_fmv>();
                            op->set_rd(resvr);
                            op->set_rs(nwvr);

                            lst.push_back(std::move(op));
                        } else {
                            bool addable = false;
                            [[maybe_unused]] bool mulable = false;

                            virt_reg *lnwvr = nullptr;
                            virt_reg *rnwvr = nullptr;

                            if (lhs->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(lhs);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());
                                if (pk.v32 == 0x3f800000 and opcode == OP_MUL) {
                                    linkable = true;
                                } else if (pk.v32 == 0x40000000 and (opcode == OP_MUL)) {
                                    addable = true;
                                    // lnwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);
                                } else if (pk.v32 == 0 and opcode == OP_ADD) {
                                    linkable = true;
                                } else {
                                    lnwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);
                                }
                            }

                            if (rhs->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(rhs);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());

                                if (pk.v32 == 0x3f800000 and (opcode == OP_MUL or opcode == OP_DIV)) {
                                    linkable = true;
                                } else if (pk.v32 == 0x40000000 and (opcode == OP_MUL)) {
                                    addable = true;
                                    // rnwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);
                                } else if (pk.v32 == 0 and (opcode == OP_ADD or opcode == OP_SUB)) {
                                    linkable = true;
                                } else if (opcode == OP_DIV) {
                                    // mulable = true;
                                    // pk.f32 = 1 / pk.f32;
                                    rnwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);
                                } else {
                                    rnwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);
                                }
                            }

                            if (lhs->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(lhs);
                                Assert(var, "bad dynamic cast");

                                if (linkable) {
                                    Assert(rnwvr == nullptr, "rhs not imm");
                                    rl_pgrs_.valc_.link(res->GetVariableIdx(), var->GetVariableIdx());
                                    break;
                                }

                                lnwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                                if (addable) {
                                    Assert(rnwvr == nullptr, "rhs not imm");
                                    rnwvr = lnwvr;

                                    auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::FLT, res->GetVariableIdx());
                                    auto op = std::make_unique<uop_fbin>();
                                    op->set_kind(FBIN_KIND::ADD);
                                    op->set_lhs(lnwvr);
                                    op->set_rhs(rnwvr);
                                    op->set_rd(resvr);

                                    lst.push_back(std::move(op));
                                    break;
                                }

                                if (mulable) {
                                    auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::FLT, res->GetVariableIdx());
                                    auto op = std::make_unique<uop_fbin>();
                                    op->set_kind(FBIN_KIND::MUL);
                                    op->set_lhs(lnwvr);
                                    op->set_rhs(rnwvr);
                                    op->set_rd(resvr);

                                    lst.push_back(std::move(op));
                                    break;
                                }
                            }

                            if (rhs->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(rhs);
                                Assert(var, "bad dynamic cast");

                                if (linkable) {
                                    Assert(lnwvr == nullptr, "lhs not imm");
                                    rl_pgrs_.valc_.link(res->GetVariableIdx(), var->GetVariableIdx());
                                    break;
                                }

                                rnwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                                if (addable) {
                                    Assert(lnwvr == nullptr, "lhs not imm");
                                    lnwvr = rnwvr;

                                    auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::FLT, res->GetVariableIdx());
                                    auto op = std::make_unique<uop_fbin>();
                                    op->set_kind(FBIN_KIND::ADD);
                                    op->set_lhs(lnwvr);
                                    op->set_rhs(rnwvr);
                                    op->set_rd(resvr);

                                    lst.push_back(std::move(op));
                                    break;
                                }
                            }

                            auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::FLT, res->GetVariableIdx());
                            auto op = std::make_unique<uop_fbin>();
                            op->set_kind((FBIN_KIND)opcode);
                            op->set_lhs(lnwvr);
                            op->set_rhs(rnwvr);
                            op->set_rd(resvr);

                            lst.push_back(std::move(op));
                        }
                    } else {
                        panic("unexpected");
                    }
                } break;

                case OP_LTH:
                case OP_GTH:
                case OP_LEQ:
                case OP_GEQ:
                case OP_EQU:
                case OP_NEQ: {
                    auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&res = llinst->GetResult();
                    auto lhs = llinst->GetLHS();
                    auto rhs = llinst->GetRHS();

                    if (auto &&ltype = lhs->GetBaseType(); ltype->IntType() or ltype->BoolType()) {
                        auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::INT, res->GetVariableIdx());

                        if (lhs->IsConstant() and rhs->IsConstant()) {
                            auto lcst = std::dynamic_pointer_cast<Constant>(lhs);
                            Assert(lcst, "bad dynamic cast");

                            auto &&lpk = xcval(lcst->GetValue());

                            auto rcst = std::dynamic_pointer_cast<Constant>(rhs);
                            Assert(rcst, "bad dynamic cast");

                            auto &&rpk = xcval(rcst->GetValue());

                            bool cmpres{};

                            switch (opcode) {
                                case OP_LTH:
                                    cmpres = (int32_t)lpk.v32 < (int32_t)rpk.v32;
                                    break;
                                case OP_GTH:
                                    cmpres = (int32_t)lpk.v32 > (int32_t)rpk.v32;
                                    break;
                                case OP_LEQ:
                                    cmpres = (int32_t)lpk.v32 <= (int32_t)rpk.v32;
                                    break;
                                case OP_GEQ:
                                    cmpres = (int32_t)lpk.v32 >= (int32_t)rpk.v32;
                                    break;
                                case OP_EQU:
                                    cmpres = (int32_t)lpk.v32 == (int32_t)rpk.v32;
                                    break;
                                case OP_NEQ:
                                    cmpres = (int32_t)lpk.v32 != (int32_t)rpk.v32;
                                    break;
                                default:
                                    panic("unexpected");
                            }
                            virt_reg *nwvr = rl_pgrs_.valc_.alloc_imm(cmpres);

                            auto op = std::make_unique<uop_mv>();
                            op->set_rd(resvr);
                            op->set_rs(nwvr);

                            lst.push_back(std::move(op));
                        } else {
                            virt_reg *lnwvr = nullptr;
                            virt_reg *rnwvr = nullptr;

                            if (lhs->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(lhs);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());
                                lnwvr = rl_pgrs_.valc_.alloc_imm(pk.v32);
                            } else if (lhs->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(lhs);
                                Assert(var, "bad dynamic cast");

                                lnwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                            } else {
                                panic("unexpected");
                            }

                            if (rhs->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(rhs);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());
                                rnwvr = rl_pgrs_.valc_.alloc_imm(pk.v32);
                            } else if (rhs->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(rhs);
                                Assert(var, "bad dynamic cast");

                                rnwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                            } else {
                                panic("unexpected");
                            }

                            auto op = std::make_unique<uop_icmp>();
                            op->set_kind((COMP_KIND)opcode);
                            op->set_lhs(lnwvr);
                            op->set_rhs(rnwvr);
                            op->set_rd(resvr);

                            lst.push_back(std::move(op));
                        }

                    } else if (ltype->FloatType()) {
                        auto resvr = rl_pgrs_.valc_.alloc_reg(VREG_TYPE::INT, res->GetVariableIdx());

                        if (lhs->IsConstant() and rhs->IsConstant()) {
                            auto lcst = std::dynamic_pointer_cast<Constant>(lhs);
                            Assert(lcst, "bad dynamic cast");

                            auto &&lpk = xcval(lcst->GetValue());

                            auto rcst = std::dynamic_pointer_cast<Constant>(rhs);
                            Assert(rcst, "bad dynamic cast");

                            auto &&rpk = xcval(rcst->GetValue());

                            bool cmpres{};
                            switch (opcode) {
                                case OP_LTH:
                                    cmpres = lpk.f32 < rpk.f32;
                                    break;
                                case OP_GTH:
                                    cmpres = lpk.f32 > rpk.f32;
                                    break;
                                case OP_LEQ:
                                    cmpres = lpk.f32 <= rpk.f32;
                                    break;
                                case OP_GEQ:
                                    cmpres = lpk.f32 >= rpk.f32;
                                    break;
                                case OP_EQU:
                                    cmpres = lpk.f32 == rpk.f32;
                                    break;
                                case OP_NEQ:
                                    cmpres = lpk.f32 != rpk.f32;
                                    break;
                                default:
                                    panic("unexpected");
                            }
                            auto nwvr = rl_pgrs_.valc_.alloc_imm(cmpres);

                            auto op = std::make_unique<uop_mv>();
                            op->set_rd(resvr);
                            op->set_rs(nwvr);

                            lst.push_back(std::move(op));
                        } else {
                            virt_reg *lnwvr = nullptr;
                            virt_reg *rnwvr = nullptr;

                            if (lhs->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(lhs);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());
                                lnwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);
                            } else if (lhs->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(lhs);
                                Assert(var, "bad dynamic cast");

                                lnwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                            } else {
                                panic("unexpected");
                            }

                            if (rhs->IsConstant()) {
                                auto cst = std::dynamic_pointer_cast<Constant>(rhs);
                                Assert(cst, "bad dynamic cast");

                                auto &&pk = xcval(cst->GetValue());
                                rnwvr = rl_pgrs_.valc_.alloc_loc(pk.v32);
                            } else if (rhs->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(rhs);
                                Assert(var, "bad dynamic cast");

                                rnwvr = rl_pgrs_.valc_.get_reg(var->GetVariableIdx());
                            } else {
                                panic("unexpected");
                            }

                            auto op = std::make_unique<uop_fcmp>();
                            op->set_kind((COMP_KIND)opcode);
                            op->set_lhs(lnwvr);
                            op->set_rhs(rnwvr);
                            op->set_rd(resvr);

                            lst.push_back(std::move(op));
                        }
                    } else {
                        panic("unexpected");
                    }
                } break;

                default:
                    panic("unexpected");
                    break;
            }
        }

        rl_pgrs_.bbs_.push_back(std::move(rlbb));
    }
}