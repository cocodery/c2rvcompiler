#include "lltorl.hh"

void FunctoProg(NormalFunction *func, ABBProg *prog) {
    rl_lltorl_instance torl{func, prog};
    torl();
}

rl_lltorl_instance::rl_lltorl_instance(NormalFunction *func, ABBProg *prog)
    : func_(func), prog_(prog), rlp(std::make_unique<rl_progress>()) {
    rlp->pnm = func->GetFuncName();
    tpfx = "L." + func->GetFuncName() + ".";

    for (auto &&param : func_->GetParamList()) {
        auto var = std::dynamic_pointer_cast<Variable>(param);
        Assert(var, "input param is not variable");
        if (auto &&vartype = var->GetBaseType(); vartype->IsPointer()) {
            auto nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::PRM, 8, var->GetVariableIdx());
            rlp->params.push_back(nwvreg);
        } else if (vartype->IntType() or vartype->FloatType()) {
            auto nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::PRM, 4, var->GetVariableIdx());
            rlp->params.push_back(nwvreg);
        } else {
            panic("Unexpected parameter type");
        }
    }
}

void rl_lltorl_instance::torl() {
    for (auto &&bb : func_->TopoSortFromEntry()) {
        auto &&rlbb = std::make_unique<rl_basicblock>();
        auto &&lst = rlbb->ops;
        rlbb->lbid = bb->GetBlockIdx();
        rlbb->tag = tpfx;
        rlp->lbmap[rlbb->lbid].bbp = rlbb.get();

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
                        auto op0 = std::make_unique<uop_set_param>();
                        op0->idx = 0;

                        if (retval->IsVariable()) {
                            // 如果返回值是 变量

                            // 转换，并按照该变量信息构造一个虚拟寄存器
                            // 该虚拟寄存器应该已经存在
                            auto var = std::dynamic_pointer_cast<Variable>(retval);
                            Assert(var, "bad dynamic cast");

                            // 保证该变量是存在的
                            auto evreg = rlp->vreg_alloc.getREG(var->GetVariableIdx());

                            // 注册到返回值
                            op0->src = evreg;
                        } else if (retval->IsConstant()) {
                            // 如果返回值是 常量

                            // 转换，并按照该变量信息构造一个虚拟寄存器
                            // 该虚拟寄存器应该是一个包含整数的虚拟寄存器
                            auto cst = std::dynamic_pointer_cast<Constant>(retval);
                            Assert(cst, "bad dynamic cast");

                            auto &&pk = extractcst(cst);

                            // 注册到返回值
                            if (pk.isfloat) {
                                auto nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::LOC, 4, pk.value);
                                op0->src = nwvreg;
                            } else {
                                auto nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::IMM, 4, pk.value);
                                op0->src = nwvreg;
                            }
                        } else {
                            // 不考虑其他情况

                            panic("寄了");
                        }

                        op0->src->beset |= 0x1;
                        lst.push_back(std::move(op0));
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

                    // 设置目的地标签
                    op->lbid = llinst->GetTarget()->GetBlockIdx();
                    op->pfx = tpfx;
                    rlp->lbmap[op->lbid].refs.push_back(op.get());

                    // 将操作置入当前块中
                    lst.push_back(std::move(op));

                } break;

                case Branch: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<BranchInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    // 获取条件，构造标签
                    auto &&cond = llinst->GetCondition();
                    auto &&tgid = llinst->GetFalseTarget()->GetBlockIdx();

                    if (cond->IsVariable()) {
                        // 如果条件是一个变量
                        // 转换类型，检查转换结果
                        auto var = std::dynamic_pointer_cast<Variable>(cond);
                        Assert(var, "bad dynamic cast");

                        // 构造指令
                        auto op = std::make_unique<uop_b>();

                        // 条件应当已经存在
                        auto evreg = rlp->vreg_alloc.getREG(var->GetVariableIdx());
                        op->cond = evreg;

                        op->pfx = tpfx;
                        op->lbid = tgid;
                        rlp->lbmap[tgid].refs.push_back(op.get());

                        // 将操作置入当前块中
                        lst.push_back(std::move(op));

                    } else if (cond->IsConstant()) {
                        auto cst = std::dynamic_pointer_cast<Constant>(cond);
                        Assert(cst, "bad dynamic cast");

                        auto &&pk = extractcst(cst);

                        if (pk.value != 0) {
                            // 构造 J 指令
                            auto op = std::make_unique<uop_j>();

                            // 设置目的地标签
                            op->pfx = tpfx;
                            op->lbid = tgid;
                            rlp->lbmap[tgid].refs.push_back(op.get());

                            // 将操作置入当前块中
                            lst.push_back(std::move(op));
                        }
                    } else {
                        panic("寄了");
                    }
                } break;

                case Alloca: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<AllocaInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    // 该指令主要用于在栈上开辟一段内存使用
                    // 为了压缩栈的大小，应该对类型做严格的检查
                    // 其中，指针类型占 8 个字节，其余类型占 4 个字节，这需要注意

                    virt_reg *nwvreg = nullptr;

                    if (auto &&type = llinst->GetAllocaType(); type->IsScalar()) {
                        auto sidx = rlp->vski_alloc.alloc(VSTK_KIND::VAL, 4);

                        nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::STK, 4, sidx);
                    } else if (type->IsArray()) {
                        auto lstty = std::dynamic_pointer_cast<ListType>(type);
                        Assert(lstty, "bad dynamic cast");

                        auto len = 4 * lstty->GetCapacity();
                        auto sidx = rlp->vski_alloc.alloc(VSTK_KIND::VAL, len);
                        nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::STK, len, sidx);
                    }

                    // 该 mv 操作应该翻译成一个 add 或者 addi 操作
                    // 注意，是 64 位操作
                    auto recv = rlp->vreg_alloc.alloc(VREG_KIND::REG, 8, llinst->GetResult()->GetVariableIdx());
                    auto op = std::make_unique<uop_mv>();
                    op->dst = recv;
                    op->src = nwvreg;

                    lst.push_back(std::move(op));
                } break;

                case Store: {
                    // 转换类型，检查转换结果
                    auto llinst = std::dynamic_pointer_cast<StoreInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    // 先分析出需要保存的数值
                    virt_reg *nwvreg = nullptr;
                    auto &&sval = llinst->GetStoreValue();
                    if (sval->IsVariable()) {
                        auto var = std::dynamic_pointer_cast<Variable>(sval);
                        Assert(var, "bad dynamic cast");

                        if (var->GetBaseType()->IsPointer()) {
                            nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::REG, 8, var->GetVariableIdx());
                        } else {
                            nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::REG, 4, var->GetVariableIdx());
                        }

                    } else if (sval->IsConstant()) {
                        auto cst = std::dynamic_pointer_cast<Constant>(sval);
                        Assert(cst, "bad dynamic cast");

                        auto &&pk = extractcst(cst);
                        if (pk.isfloat) {
                            nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::LOC, 4, pk.value);
                        } else {
                            nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::IMM, 4, pk.value);
                        }
                    } else {
                        panic("寄了");
                    }

                    auto &&aval = llinst->GetStoreAddr();
                    if (aval->IsGlobalValue()) {
                        auto gval = std::dynamic_pointer_cast<GlobalValue>(aval);
                        auto &&label = "LG" + std::to_string(gval->GetGlobalValueIdx());
                        auto op = std::make_unique<uop_store_tag>();
                        op->dst = label;
                        op->src = nwvreg;

                        // 这里需要随便指派一个非 zero 的寄存器
                        auto rt = rlp->vreg_alloc.alloc(VREG_KIND::ZERO, 0, 0);
                        op->tgt = rt;
                        lst.push_back(std::move(op));
                    } else if (aval->IsVariable()) {
                        auto avar = std::dynamic_pointer_cast<Variable>(aval);
                        Assert(avar, "store addr should be var");
                        auto op = std::make_unique<uop_store>();

                        op->base = rlp->vreg_alloc.getREG(avar->GetVariableIdx());
                        op->src = nwvreg;

                        // 目前认为 offset 都被计算包含在了 base 里
                        op->off = 0;

                        lst.push_back(std::move(op));
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
                    Assert(!res->GetBaseType()->IsPointer(), "assuming it will not be a pointer");
                    virt_reg *nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::REG, 4, res->GetVariableIdx());

                    auto &&aval = llinst->GetOprand();
                    if (aval->IsGlobalValue()) {
                        auto gval = std::dynamic_pointer_cast<GlobalValue>(aval);
                        auto &&label = "LG" + std::to_string(gval->GetGlobalValueIdx());
                        auto op = std::make_unique<uop_load_tag>();
                        op->src = label;
                        op->dst = nwvreg;
                        lst.push_back(std::move(op));
                    } else if (aval->IsVariable()) {
                        auto avar = std::dynamic_pointer_cast<Variable>(aval);
                        Assert(avar, "store addr should be var");
                        auto op = std::make_unique<uop_load>();

                        op->base = rlp->vreg_alloc.getREG(avar->GetVariableIdx());
                        op->dst = nwvreg;

                        // 目前认为 offset 都被计算包含在了 base 里
                        op->off = 0;

                        lst.push_back(std::move(op));
                    } else {
                        panic("寄了");
                    }

                } break;

                case Gep: {
                    auto llinst = std::dynamic_pointer_cast<GetElementPtrInst>(inst);
                    Assert(llinst, "bad dynamic cast");
                    auto &&res = llinst->GetResult();
                    // 这里加载的是指针值，所以宽度用 8
                    auto dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, 8, res->GetVariableIdx());

                    auto &&base = llinst->GetBaseAddr();
                    if (base->IsGlobalValue()) {
                        auto gval = std::dynamic_pointer_cast<GlobalValue>(base);
                        auto &&gname = "LG" + std::to_string(gval->GetGlobalValueIdx());
                        auto op = std::make_unique<uop_load_tag>();
                        op->dst = dst;
                        op->src = gname;
                        lst.push_back(std::move(op));
                    } else if (base->IsVariable()) {
                        auto avar = std::dynamic_pointer_cast<Variable>(base);
                        Assert(avar, "store addr should be var");
                        auto op = std::make_unique<uop_load>();

                        auto base = rlp->vreg_alloc.getREG(avar->GetVariableIdx());

                        op->dst = dst;
                        op->base = base;
                        op->off = 0;

                        lst.push_back(std::move(op));
                    } else {
                        panic("寄了");
                    }

                } break;

                case Call: {
                    auto llinst = std::dynamic_pointer_cast<CallInst>(inst);
                    Assert(llinst, "bad dynamic cast");
                    auto &&params = llinst->GetParamList();

                    if (!params.empty()) {
                        size_t idx = 0;
                        size_t len = 0;
                        for (auto &&param : params) {
                            auto op0 = std::make_unique<uop_set_param>();

                            op0->idx = idx;
                            auto usemask = 0x1 << idx;
                            virt_reg *nwvreg = nullptr;

                            if (param->IsVariable()) {
                                auto var = std::dynamic_pointer_cast<Variable>(param);
                                nwvreg = rlp->vreg_alloc.getREG(var->GetVariableIdx());

                                len += nwvreg->len;
                            } else if (param->IsConstant()) {
                                len += 4;

                                auto &&cst = std::dynamic_pointer_cast<Constant>(param);

                                auto &&pk = extractcst(cst);

                                if (pk.isfloat) {
                                    nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::LOC, 4, pk.value);
                                } else {
                                    nwvreg = rlp->vreg_alloc.alloc(VREG_KIND::IMM, 4, pk.value);
                                }
                            } else {
                                panic("寄了");
                            }

                            op0->src = nwvreg;
                            op0->src->beset |= usemask;
                            lst.push_back(std::move(op0));

                            idx += 1;
                        }
                        mxpsiz = std::max(mxpsiz, len);
                    }

                    auto op1 = std::make_unique<uop_call>();
                    op1->callee = llinst->GetCalleeFunc()->GetFuncName();
                    op1->retval = nullptr;

                    if (auto &&res = llinst->GetResult(); res != nullptr) {
                        op1->retval = rlp->vreg_alloc.alloc(VREG_KIND::REG, 4, res->GetVariableIdx());
                    }

                    lst.push_back(std::move(op1));
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
                    op->src = rlp->vreg_alloc.getREG(var->GetVariableIdx());
                    op->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, 4, rhs->GetVariableIdx());

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
                    op->src = rlp->vreg_alloc.getREG(var->GetVariableIdx());
                    op->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, 4, rhs->GetVariableIdx());

                    lst.push_back(std::move(op));
                } break;

                case BitCast: {
                    auto llinst = std::dynamic_pointer_cast<BitCastInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&oprand = llinst->GetOprand();
                    auto var = std::dynamic_pointer_cast<Variable>(oprand);
                    Assert(var, "zext should op on var");

                    auto &&res = llinst->GetResult();

                    rlp->vreg_alloc.link(res->GetVariableIdx(), var->GetVariableIdx());
                } break;

                case Zext: {
                    auto llinst = std::dynamic_pointer_cast<ZextInst>(inst);
                    Assert(llinst, "bad dynamic cast");

                    auto &&oprand = llinst->GetOprand();
                    auto var = std::dynamic_pointer_cast<Variable>(oprand);
                    Assert(var, "zext should op on var");

                    auto &&res = llinst->GetResult();

                    rlp->vreg_alloc.link(res->GetVariableIdx(), var->GetVariableIdx());
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

                    if (res->GetBaseType()->IsPointer() || res->GetBaseType()->IntType() ||
                        res->GetBaseType()->BoolType()) {
                        auto op = std::make_unique<uop_bin>();

                        op->kind = (IBIN_KIND)opcode;

                        auto lhs = llinst->GetLHS();
                        if (lhs->IsVariable()) {
                            auto lvar = std::dynamic_pointer_cast<Variable>(lhs);
                            Assert(lvar, "bad dynamic cast");

                            auto rhs = llinst->GetRHS();
                            op->lhs = rlp->vreg_alloc.getREG(lvar->GetVariableIdx());

                            if (rhs->IsVariable()) {
                                auto rvar = std::dynamic_pointer_cast<Variable>(rhs);
                                Assert(rvar, "bad dynamic cast");

                                op->rhs = rlp->vreg_alloc.getREG(rvar->GetVariableIdx());
                                op->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(op->lhs->len, op->rhs->len),
                                                                res->GetVariableIdx());

                                lst.push_back(std::move(op));
                            } else if (rhs->IsConstant()) {
                                auto rcst = std::dynamic_pointer_cast<Constant>(rhs);
                                Assert(rcst, "bad dynamic cast");

                                auto &&pk = extractcst(rcst);
                                Assert(!pk.isfloat, "bad constant");

                                op->rhs = rlp->vreg_alloc.alloc(VREG_KIND::LOC, 4, pk.value);
                                op->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(op->lhs->len, op->rhs->len),
                                                                res->GetVariableIdx());
                                lst.push_back(std::move(op));
                            } else {
                                panic("寄了");
                            }
                        } else if (lhs->IsConstant()) {
                            auto lcst = std::dynamic_pointer_cast<Constant>(lhs);
                            Assert(lcst, "bad dynamic cast");

                            auto &&pk = extractcst(lcst);
                            Assert(!pk.isfloat, "bad constant");
                            op->lhs = rlp->vreg_alloc.alloc(VREG_KIND::IMM, 4, pk.value);

                            auto rhs = llinst->GetRHS();
                            auto rvar = std::dynamic_pointer_cast<Variable>(rhs);
                            Assert(rvar, "assuming this time rhs must be variable");

                            op->rhs = rlp->vreg_alloc.getREG(rvar->GetVariableIdx());
                            op->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(op->lhs->len, op->rhs->len),
                                                            res->GetVariableIdx());
                            lst.push_back(std::move(op));
                        } else {
                            panic("寄了");
                        }
                    } else if (res->GetBaseType()->FloatType()) {
                        auto fop = std::make_unique<uop_fbin>();
                        fop->kind = (FBIN_KIND)opcode;

                        auto lhs = llinst->GetLHS();
                        if (lhs->IsVariable()) {
                            auto lvar = std::dynamic_pointer_cast<Variable>(lhs);
                            Assert(lvar, "bad dynamic cast");

                            auto rhs = llinst->GetRHS();
                            fop->lhs = rlp->vreg_alloc.getREG(lvar->GetVariableIdx());

                            if (rhs->IsVariable()) {
                                auto rvar = std::dynamic_pointer_cast<Variable>(rhs);
                                Assert(rvar, "bad dynamic cast");

                                fop->rhs = rlp->vreg_alloc.getREG(rvar->GetVariableIdx());
                                fop->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(fop->lhs->len, fop->rhs->len),
                                                                 res->GetVariableIdx());

                                lst.push_back(std::move(fop));
                            } else if (rhs->IsConstant()) {
                                auto rcst = std::dynamic_pointer_cast<Constant>(rhs);
                                Assert(rcst, "bad dynamic cast");

                                auto &&pk = extractcst(rcst);
                                Assert(pk.isfloat, "bad constant");

                                fop->rhs = rlp->vreg_alloc.alloc(VREG_KIND::IMM, 4, pk.value);
                                fop->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(fop->lhs->len, fop->rhs->len),
                                                                 res->GetVariableIdx());
                                lst.push_back(std::move(fop));
                            } else {
                                panic("寄了");
                            }
                        } else if (lhs->IsConstant()) {
                            auto lcst = std::dynamic_pointer_cast<Constant>(lhs);
                            Assert(lcst, "bad dynamic cast");

                            auto &&pk = extractcst(lcst);
                            Assert(pk.isfloat, "bad constant");
                            fop->lhs = rlp->vreg_alloc.alloc(VREG_KIND::LOC, 4, pk.value);

                            auto rhs = llinst->GetRHS();
                            auto rvar = std::dynamic_pointer_cast<Variable>(rhs);
                            Assert(rvar, "assuming this time rhs must be variable");

                            fop->rhs = rlp->vreg_alloc.getREG(rvar->GetVariableIdx());
                            fop->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(fop->lhs->len, fop->rhs->len),
                                                             res->GetVariableIdx());
                            lst.push_back(std::move(fop));
                        } else {
                            panic("寄了");
                        }
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

                    if (res->GetBaseType()->IsPointer() || res->GetBaseType()->IntType() ||
                        res->GetBaseType()->BoolType()) {
                        auto op = std::make_unique<uop_cmp>();

                        op->kind = (COMP_KIND)opcode;
                        op->onflt = false;

                        auto lhs = llinst->GetLHS();
                        if (lhs->IsVariable()) {
                            auto lvar = std::dynamic_pointer_cast<Variable>(lhs);
                            Assert(lvar, "bad dynamic cast");

                            auto rhs = llinst->GetRHS();
                            op->lhs = rlp->vreg_alloc.getREG(lvar->GetVariableIdx());

                            if (rhs->IsVariable()) {
                                auto rvar = std::dynamic_pointer_cast<Variable>(rhs);
                                Assert(rvar, "bad dynamic cast");

                                op->rhs = rlp->vreg_alloc.getREG(rvar->GetVariableIdx());
                                op->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(op->lhs->len, op->rhs->len),
                                                                res->GetVariableIdx());

                                lst.push_back(std::move(op));
                            } else if (rhs->IsConstant()) {
                                auto rcst = std::dynamic_pointer_cast<Constant>(rhs);
                                Assert(rcst, "bad dynamic cast");

                                auto &&pk = extractcst(rcst);
                                Assert(!pk.isfloat, "bad constant");

                                op->rhs = rlp->vreg_alloc.alloc(VREG_KIND::IMM, 4, pk.value);
                                op->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(op->lhs->len, op->rhs->len),
                                                                res->GetVariableIdx());
                                lst.push_back(std::move(op));
                            } else {
                                panic("寄了");
                            }
                        } else if (lhs->IsConstant()) {
                            auto lcst = std::dynamic_pointer_cast<Constant>(lhs);
                            Assert(lcst, "bad dynamic cast");

                            auto &&pk = extractcst(lcst);
                            Assert(!pk.isfloat, "bad constant");
                            op->lhs = rlp->vreg_alloc.alloc(VREG_KIND::LOC, 4, pk.value);

                            auto rvar = std::dynamic_pointer_cast<Variable>(lhs);
                            Assert(rvar, "assuming this time rhs must be variable");

                            op->rhs = rlp->vreg_alloc.getREG(rvar->GetVariableIdx());
                            op->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(op->lhs->len, op->rhs->len),
                                                            res->GetVariableIdx());
                            lst.push_back(std::move(op));
                        } else {
                            panic("寄了");
                        }
                    } else if (res->GetBaseType()->FloatType()) {
                        auto fop = std::make_unique<uop_cmp>();
                        fop->kind = (COMP_KIND)opcode;

                        auto lhs = llinst->GetLHS();
                        if (lhs->IsVariable()) {
                            auto lvar = std::dynamic_pointer_cast<Variable>(lhs);
                            Assert(lvar, "bad dynamic cast");

                            auto rhs = llinst->GetRHS();
                            fop->lhs = rlp->vreg_alloc.getREG(lvar->GetVariableIdx());

                            if (rhs->IsVariable()) {
                                auto rvar = std::dynamic_pointer_cast<Variable>(rhs);
                                Assert(rvar, "bad dynamic cast");

                                fop->rhs = rlp->vreg_alloc.getREG(rvar->GetVariableIdx());
                                fop->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(fop->lhs->len, fop->rhs->len),
                                                                 res->GetVariableIdx());

                                lst.push_back(std::move(fop));
                            } else if (rhs->IsConstant()) {
                                auto rcst = std::dynamic_pointer_cast<Constant>(rhs);
                                Assert(rcst, "bad dynamic cast");

                                auto &&pk = extractcst(rcst);
                                Assert(pk.isfloat, "bad constant");

                                fop->rhs = rlp->vreg_alloc.alloc(VREG_KIND::LOC, 4, pk.value);
                                fop->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(fop->lhs->len, fop->rhs->len),
                                                                 res->GetVariableIdx());
                                lst.push_back(std::move(fop));
                            } else {
                                panic("寄了");
                            }
                        } else if (lhs->IsConstant()) {
                            auto lcst = std::dynamic_pointer_cast<Constant>(lhs);
                            Assert(lcst, "bad dynamic cast");

                            auto &&pk = extractcst(lcst);
                            Assert(pk.isfloat, "bad constant");
                            fop->lhs = rlp->vreg_alloc.alloc(VREG_KIND::LOC, 4, pk.value);

                            auto rvar = std::dynamic_pointer_cast<Variable>(lhs);
                            Assert(rvar, "assuming this time rhs must be variable");

                            fop->rhs = rlp->vreg_alloc.getREG(rvar->GetVariableIdx());
                            fop->dst = rlp->vreg_alloc.alloc(VREG_KIND::REG, std::max(fop->lhs->len, fop->rhs->len),
                                                             res->GetVariableIdx());
                            lst.push_back(std::move(fop));
                        } else {
                            panic("寄了");
                        }
                    }
                } break;

                default:
                    panic("unexpected");
                    break;
            }
        }

        rlp->bbs.push_back(std::move(rlbb));
    }
}

void rl_lltorl_instance::operator()() {
    // 将 LLVM IR 转换为 RISC LANG IR
    torl();

    // 删除冗余的跳转语句
    rmx();

    // 加载参数
    loadparam();

    // 重排序指令
    // 同时计算生存区间和引用次数
    ireidx();

    // 打印分析后的信息
    pinfo();

    // 打印 RISC LANG IR
    pir();
}