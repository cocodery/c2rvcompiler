#include "linearscan.hh"

#include "../../spec/riscv/riscv-asm.hh"
#include "../../spec/riscv/riscv-assigner.hh"

static inline bool lsp_sort_begin(const livespan &lhs, const livespan &rhs) { return lhs.begin > rhs.begin; };

static inline bool lsp_sort_end(const livespan &lhs, const livespan &rhs) { return lhs.end > rhs.end; };

LinearScan::LinearScan(NormalFuncPtr &xfunc, NameValueMap &xnvmap) : func(xfunc), nvmap(xnvmap) {
    reindex();
    generate_intervals();
}

void LinearScan::reindex() {
    auto &&topo = func->TopoSortFromEntry();

    size_t confirm_idx = 1;

    for (auto &&cfg : topo) {
        auto &&blkidx = cfg->GetBlockIdx();
        Assert(confirm_idx <= blkidx, "topo idx not seqential");

        label_reindex[blkidx] = inst_reindex.size();
        inst_reindex.push_back({reindex_inst::LABEL, nullptr, blkidx});

        for (auto &&inst : cfg->GetInstList()) {
            inst_reindex.push_back({reindex_inst::INST, inst});
        }

        confirm_idx = blkidx + 1;
    }
}

void LinearScan::generate_intervals() {
    size_t phi_stk_off = 0;

    for (size_t i = 0; i < inst_reindex.size(); ++i) {
        if (inst_reindex[i].type == reindex_inst::INST) {
            auto &&iptr = inst_reindex[i].instptr;
            auto &&dst = iptr->GetResult();

            if (dst != nullptr && dst->IsVariable()) {
                auto vidx = dst->GetVariableIdx();

                // check if variable is defined
                Assert(live_interval.find(vidx) == live_interval.end() || live_interval.at(vidx).undefined,
                       "Not SSA ???");

                live_interval[vidx].regidx = vidx;
                live_interval[vidx].begin = i;
                live_interval[vidx].undefined = false;
            }

            auto &&srcs = iptr->GetOprands();

            bool is_phi = iptr->GetOpCode() == Phi;

            for (auto &&src : srcs) {
                if (src != nullptr && src->IsVariable()) {
                    auto vsrc = std::dynamic_pointer_cast<Variable>(src);
                    auto &&vidx = vsrc->GetVariableIdx();
                    live_interval[vidx].end = i;
                    live_interval.at(vidx).is_phisrc = is_phi;
                    live_interval.at(vidx).phioff = phi_stk_off;
                }
            }

            if (is_phi) {
                phi_stk_off += 4;
            }
        }
    }
}

using lspcomp = bool (*)(const livespan &lhs, const livespan &rhs);

using lsppq_t = std::priority_queue<livespan, std::vector<livespan>, lspcomp>;

void LinearScan::plan(ABBProg *prog) {
    prog->name = func->GetFuncName();
    generate_asm(*prog);
}

void LinearScan::generate_asm(ABBProg &prog) {
    std::string label_prefix = "L." + func->GetFuncName() + ".";

    // build heap
    std::vector<livespan> vls;
    for (auto &&span : live_interval) {
        if (span.second.is_phisrc) continue;

        vls.push_back(span.second);
    }

    lsppq_t heap_begin(vls.begin(), vls.end(), lsp_sort_begin);
    lsppq_t heap_end(vls.begin(), vls.end(), lsp_sort_end);

    // cur living
    std::unordered_map<size_t, size_t> inreg;

    // cur kick out
    std::unordered_set<size_t> instake;

    // initial
    // for push {ra, s0}
    auto abbptr = std::make_shared<ABBProg::ABBlock>();
    abbptr->tagname = label_prefix + "start_proc";
    prog.abbs.push_back(abbptr);
    auto &&start_proc __attribute_maybe_unused__ = prog.abbs.back();

    abbptr = nullptr;

    RISCVAssigner assign;

    // start linear scan algorithm
    for (size_t i = 0; i < inst_reindex.size(); ++i) {
        auto &&inst = inst_reindex[i];

        // kick out all reg if they will die at this inst
        while (!heap_end.empty() && heap_end.top().end <= i) {
            assign.release(heap_end.top().regidx);
            inreg.erase(heap_end.top().regidx);
            heap_end.pop();
        }

        switch (inst.type) {
            case reindex_inst::LABEL: {
                if (abbptr != nullptr) {
                    prog.abbs.push_back(abbptr);
                }
                abbptr = std::make_shared<ABBProg::ABBlock>();
                abbptr->tagname = label_prefix + std::to_string(inst.label_idx);
            } break;

            case reindex_inst::INST: {
                Assert(abbptr, "no basic block ptr set");
                auto &&lst = abbptr->asminsts;

                //
                // ===== ====== Big  Switch ===== =====
                //

                switch (inst.instptr->GetOpCode()) {
                    case None: {
                        auto rvinst = std::make_unique<RV_NOP>();
                        lst.push_back(std::move(rvinst));
                    } break;

                    case Ret: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<ReturnInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_RET>();
                        lst.push_back(std::move(rvinst));

                    } break;

                    case Jump: {
                        auto llinst = std::dynamic_pointer_cast<JumpInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto &&target = llinst->GetTarget();
                        auto &&label = label_prefix + std::to_string(target->GetBlockIdx());
                        auto &&rvinst = std::make_unique<RV_J$>(label.c_str());
                        lst.push_back(std::move(rvinst));
                    } break;

                    case Branch: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BranchInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto &&cond = llinst->GetCondition();
                        auto &&label = label_prefix + std::to_string(llinst->GetFalseTarget()->GetBlockIdx());

                        do {
                            // if cond is constant
                            if (cond->IsConstant()) {
                                auto cstcond = std::dynamic_pointer_cast<Constant>(cond);
                                bool flag = false;

                                std::visit(
                                    [&flag](auto &&arg) {
                                        using T = std::decay_t<decltype(arg)>;
                                        if constexpr (std::is_same_v<T, float>) {
                                            double double_value = arg;
                                            flag = double_value == 0;
                                        } else {
                                            flag = static_cast<int64_t>(arg) == 0;
                                        }
                                    },
                                    cstcond->GetValue());

                                if (flag) break;

                                auto rvinst = std::make_unique<RV_J$>(label.c_str());
                                lst.push_back(std::move(rvinst));
                                break;
                            }

                            if (cond->IsVariable()) {
                                auto varcond = std::dynamic_pointer_cast<Variable>(cond);
                                auto vidx = varcond->GetVariableIdx();

                                if (auto &&res = inreg.find(vidx); res != inreg.end()) {
                                    auto rvinst = std::make_unique<RV_BEQZ$>(res->second, label.c_str());
                                    lst.push_back(std::move(rvinst));
                                    break;
                                }
                            }
                        } while (0);

                    } break;

                    case Alloca: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<AllocaInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto type = llinst->GetAllocaType();
                        if (type->IsScalar()) {
                        }

                    } break;

                    case Store: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<StoreInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SW$>(0, "fake");
                        lst.push_back(std::move(rvinst));

                    } break;

                    case Gep: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<GetElementPtrInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_LA$>(0, "fake");
                        lst.push_back(std::move(rvinst));

                    } break;

                    case Call: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<CallInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_CALL$>(llinst->GetCalleeFunc()->GetFuncName().c_str());
                        lst.push_back(std::move(rvinst));

                    } break;

                    case Phi: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<PhiInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_NOP>();
                        lst.push_back(std::move(rvinst));

                    } break;

                    case Load: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<LoadInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_LW$>(0, "fake");
                        lst.push_back(std::move(rvinst));

                    } break;

                    case SiToFp: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<SitoFpInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_FCVT_S_W>(32, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case FpToSi: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<FptoSiInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_FCVT_W_S>(0, 32);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case Zext: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<ZextInst>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_MV>(0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_ADD: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_ADDW>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_SUB: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SUBW>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_MUL: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_MULW>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_DIV: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_DIVW>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_REM: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_REMW>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_LSHIFT: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SLLW>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_RSHIFT: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SRAW>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_LTH: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SLTI>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_LEQ: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SLTI>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_GTH: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SLTI>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_GEQ: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SLTI>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_EQU: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SLTI>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    case OP_NEQ: {
                        // TODO
                        auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst.instptr);
                        Assert(llinst, "bad dynamic cast");
                        auto rvinst = std::make_unique<RV_SLTI>(0, 0, 0);
                        lst.push_back(std::move(rvinst));

                    } break;

                    default:
                        panic("unexpected");
                }

                //
                // ===== ====== ===== ===== ===== =====
                //

            } break;

            default:
                panic("unexpected");
        }
    }
    if (abbptr != nullptr) {
        prog.abbs.push_back(abbptr);
    }
}
