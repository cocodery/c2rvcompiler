#include "3tle3wa/pass/interprocedural/dvnt/dvnt.hh"

#include "3tle3wa/ir/instruction/instruction.hh"

bool GVN::VNExpr::operator==(const VNExpr &e) const {
    if (opcode != e.opcode) {
        return false;
    }
    return (Value::ValueCompare(lhs, e.lhs) && Value::ValueCompare(rhs, e.rhs)) ||
           (IsCommutative(opcode) && Value::ValueCompare(lhs, e.rhs) && Value::ValueCompare(rhs, e.lhs));
}

size_t GVN::VNExprHasher::operator()(const VNExpr &e) const {
    auto l = reinterpret_cast<uint64_t>(e.lhs.get());
    auto r = reinterpret_cast<uint64_t>(e.rhs.get());
    auto p = (l >> __builtin_ctzll(l)) * (r >> __builtin_ctzll(r));
    auto o = e.opcode;
    return p >> o | p << (64 - o);
}

GVN::VNScope::VNScope(VNScope *outer) : outer(outer) {}

BaseValuePtr GVN::VNScope::Get(BinaryInstPtr inst) {
    VNExpr expr{inst->GetOpCode(), inst->GetLHS(), inst->GetRHS()};
    for (auto &&iter = this; iter != nullptr; iter = iter->outer) {
        if (iter->map.count(expr)) {
            return iter->map[expr];
        }
    }
    return nullptr;
}

void GVN::VNScope::Set(BinaryInstPtr inst) {
    VNExpr expr{inst->GetOpCode(), inst->GetLHS(), inst->GetRHS()};
    map[expr] = inst->GetResult();
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
            inst->ReplaceSRC(oprand, vn);
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
                assert(inst->ReplaceSRC(oprand, vn));
            }
        }
        if (auto [replacee, replacer] = inst->DoFlod(); replacee != nullptr && replacer != nullptr) {
            ReplaceSRC(replacee, replacer);

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
        }
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