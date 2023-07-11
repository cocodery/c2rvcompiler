#include "3tle3wa/pass/interprocedural/dvnt/dvnt.hh"

bool GVN::BinVNExpr::operator==(const BinVNExpr &e) const {
    if (opcode != e.opcode) {
        return false;
    }
    return (Value::ValueCompare(lhs, e.lhs) && Value::ValueCompare(rhs, e.rhs)) ||
           (IsCommutative(opcode) && Value::ValueCompare(lhs, e.rhs) && Value::ValueCompare(rhs, e.lhs));
}

size_t GVN::BinVNExprHasher::operator()(const BinVNExpr &e) const {
    auto l = reinterpret_cast<uint64_t>(e.lhs.get());
    auto r = reinterpret_cast<uint64_t>(e.rhs.get());
    auto p = (l >> __builtin_ctzll(l)) * (r >> __builtin_ctzll(r));
    auto o = e.opcode;
    return p >> o | p << (64 - o);
}

bool GVN::GepVNExpr::operator==(const GepVNExpr &e) const {
    if (off_size != e.off_size) {
        return false;
    }
    return (base_addr == e.base_addr) && (last_off == e.last_off);
}

size_t GVN::GepVNExprHasher::operator()(const GepVNExpr &e) const {
    return ((std::hash<void *>()(e.base_addr) >> e.off_size) & (std::hash<void *>()(e.last_off) << e.off_size));
}

bool GVN::LoadVNExpr::operator==(const LoadVNExpr &e) const { return (load_addr == e.load_addr); }

size_t GVN::LoadVNExprHasher::operator()(const LoadVNExpr &e) const { return (std::hash<void *>()(e.load_addr)); }

GVN::VNScope::VNScope(VNScope *outer) : outer(outer) {}

BaseValuePtr GVN::VNScope::Get(InstPtr inst) {
    if (inst->IsBranchInst()) {
        auto bin_inst = std::static_pointer_cast<BinaryInstruction>(inst);

        BinVNExpr expr{bin_inst->GetOpCode(), bin_inst->GetLHS(), bin_inst->GetRHS()};
        for (auto &&iter = this; iter != nullptr; iter = iter->outer) {
            if (iter->bin_map.count(expr)) {
                return iter->bin_map[expr];
            }
        }
    } else if (inst->IsGepInst()) {
        auto gep_inst = std::static_pointer_cast<GetElementPtrInst>(inst);
        auto off_list = gep_inst->GetOffList();
        GepVNExpr expr{off_list.size(), gep_inst->GetBaseAddr().get(), off_list.back().get()};
        for (auto &&iter = this; iter != nullptr; iter = iter->outer) {
            if (iter->gep_map.count(expr)) {
                return iter->gep_map[expr];
            }
        }
    }
    // else if (inst->IsLoadInst()) {
    //     auto load_inst = std::static_pointer_cast<LoadInst>(inst);
    //     LoadVNExpr expr{load_inst->GetOprand().get()};
    //     for (auto &&iter = this; iter != nullptr; iter = iter->outer) {
    //         if (iter->load_map.count(expr)) {
    //             return iter->load_map[expr];
    //         }
    //     }
    // }
    return nullptr;
}

void GVN::VNScope::Set(InstPtr inst) {
    if (inst->IsBranchInst()) {
        auto bin_inst = std::static_pointer_cast<BinaryInstruction>(inst);
        BinVNExpr expr{inst->GetOpCode(), bin_inst->GetLHS(), bin_inst->GetRHS()};
        bin_map[expr] = inst->GetResult();
    } else if (inst->IsGepInst()) {
        auto gep_inst = std::static_pointer_cast<GetElementPtrInst>(inst);
        auto off_list = gep_inst->GetOffList();
        GepVNExpr expr{off_list.size(), gep_inst->GetBaseAddr().get(), off_list.back().get()};
        gep_map[expr] = inst->GetResult();
    }
    // else if (inst->IsLoadInst()) {
    //     auto load_inst = std::static_pointer_cast<LoadInst>(inst);
    //     LoadVNExpr expr{load_inst->GetOprand().get()};
    //     load_map[expr] = inst->GetResult();
    // }
}

BaseValuePtr GVN::GetVN(BaseValuePtr v) { return VN[v]; }

bool GVN::IsMeaingLess(InstPtr inst) {
    // all inputs have same value-number
    assert(inst->IsPhiInst());
    auto &&oprands = inst->GetOprands();
    if (oprands.size() < 2) {
        return true;
    }
    auto BeginVN = GetVN((*oprands.begin()));
    if (BeginVN == nullptr) return false;
    // TODO: need to fix
    return std::all_of(oprands.begin(), oprands.end(),
                       [&](auto &i) -> bool { return Value::ValueCompare(BeginVN, GetVN(i)); });
}

bool GVN::IsRedundant(CfgNodePtr node, InstPtr inst) {
    // compute same value as another phi-function
    assert(inst->IsPhiInst());
    auto phi_vn = GetVN(inst->GetResult());
    for (auto &&inst_ : node->GetInstList()) {
        if (inst_->IsPhiInst()) {
            if (inst == inst_) continue;
            if (phi_vn != nullptr && GetVN(inst_->GetResult()) == phi_vn) return true;
        }
    }
    return false;
}

bool GVN::IsPhiOprandSame(InstPtr inst) {
    assert(inst->IsPhiInst());
    auto &&oprands = inst->GetOprands();
    auto begin = (*oprands.begin());
    return std::all_of(oprands.begin(), oprands.end(), [&](auto &i) -> bool { return Value::ValueCompare(begin, i); });
}

void GVN::AdjustPhiInst(CfgNodePtr node, PhiInstPtr inst) {
    if (auto oprand = inst->FindInComingUse(node)) {
        if (auto vn = GetVN(oprand)) {
            ReplaceSRC(oprand, vn);
        }
    }
}

void GVN::DoDVNT(CfgNodePtr node, VNScope *outer) {
    VNScope Scope = VNScope(outer);
    auto &&inst_list = node->GetInstList();

    for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
        auto inst = (*iter);
        if (inst->IsPhiInst()) {
            auto result = inst->GetResult();
            if (IsMeaingLess(inst) || IsRedundant(node, inst)) {
                assert(inst->GetOprands().size() > 0);
                VN[result] = GetVN((*inst->GetOprands().begin()));

                RemoveInst(inst);
                iter = inst_list.erase(iter);
                continue;
            } else if (IsPhiOprandSame(inst)) {
                VN[result] = (*inst->GetOprands().begin());

                RemoveInst(inst);
                iter = inst_list.erase(iter);
                continue;
            } else {
                VN[result] = result;
            }
        }
        ++iter;
    }

    for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
        auto inst = (*iter);

        auto oprands = inst->GetOprands();
        std::unordered_map<BaseValuePtr, BaseValuePtr> map;
        for (auto &&it = oprands.begin(); it != oprands.end(); ++it) {
            auto &&oprand = (*it);
            if (auto vn = GetVN(oprand); vn != nullptr && map[oprand] == nullptr && vn != oprand) {
                map[oprand] = vn;
                ReplaceSRC(oprand, vn);
            }
        }
        if (auto replacer = inst->DoFlod(); replacer != nullptr) {
            ReplaceSRC(inst->GetResult(), replacer);

            RemoveInst(inst);
            iter = inst_list.erase(iter);
            continue;
        }
        if (inst->IsTwoOprandInst()) {
            auto bin_inst = std::static_pointer_cast<BinaryInstruction>(inst);
            auto result = bin_inst->GetResult();

            if (auto res = Scope.Get(bin_inst)) {
                VN[result] = res;

                RemoveInst(inst);
                iter = inst_list.erase(iter);
                continue;
            } else {
                VN[result] = result;
                Scope.Set(bin_inst);
            }
        } else if (inst->IsGepInst()) {
            auto gep_inst = std::static_pointer_cast<GetElementPtrInst>(inst);
            auto result = gep_inst->GetResult();

            if (auto res = Scope.Get(gep_inst)) {
                VN[result] = res;

                RemoveInst(inst);
                iter = inst_list.erase(iter);
                continue;
            } else {
                VN[result] = result;
                Scope.Set(gep_inst);
            }
        }
        // else if (inst->IsLoadInst()) {
        //     auto load_inst = std::static_pointer_cast<LoadInst>(inst);
        //     auto result = load_inst->GetResult();

        //     if (auto res = Scope.Get(load_inst)) {
        //         VN[result] = res;

        //         RemoveInst(inst);
        //         iter = inst_list.erase(iter);
        //         continue;
        //     } else {
        //         VN[result] = result;
        //         Scope.Set(load_inst);
        //     }
        // }
        ++iter;
    }
    for (auto succ : node->GetSuccessors()) {
        for (auto inst : succ->GetInstList()) {
            if (inst->IsPhiInst()) {
                AdjustPhiInst(node, std::static_pointer_cast<PhiInst>(inst));
            }
        }
    }

    for (auto child : node->GetDominateChildren()) {
        DoDVNT(child, &Scope);
    }
}

void GVN::DVNT(NormalFuncPtr func) {
    assert(VN.empty());
    for (auto param : func->GetParamList()) {
        VN[param] = param;
    }
    DoDVNT(func->GetEntryNode(), nullptr);
    VN.clear();
}