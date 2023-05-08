#include "gvngcm.hh"

bool GVN::VNExpr::operator==(const VNExpr &e) const {
    if (opcode != e.opcode) {
        return false;
    }
    return (Value::ValueCompare(lhs, e.lhs) && Value::ValueCompare(rhs, e.rhs)) ||
           (IsCommutative(opcode) && Value::ValueCompare(lhs, e.rhs) && Value::ValueCompare(rhs, e.lhs));
}

size_t GVN::VNExprHasher::operator()(const VNExpr &e) const {
    return reinterpret_cast<size_t>(e.lhs.get()) ^
           (reinterpret_cast<size_t>(e.rhs.get()) >> static_cast<size_t>(e.opcode) |
            reinterpret_cast<size_t>(e.rhs.get()) << static_cast<size_t>(64 - (e.opcode)));
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
    assert(inst->IsPhiInst());
    auto &&phi_oprands = inst->GetOprands();
    if (phi_oprands.size() < 2) {
        return true;
    }
    auto BeginVN = GetVN((*phi_oprands.begin()));
    return std::all_of(phi_oprands.begin(), phi_oprands.end(), [&](auto &i) -> bool { return BeginVN == GetVN(i); });
}

bool GVN::IsRedundant(CfgNodePtr node, InstPtr inst) {
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

void GVN::DoDVNT(CfgNodePtr node, VNScope *outer) {
    VNScope scope = VNScope(outer);
    auto &&inst_list = node->GetInstList();

    // for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
    //     auto inst = (*iter);
    //     if (inst->IsPhiInst()) {
    //         auto result = inst->GetResult();
    //         if (IsMeaingLess(inst) || IsRedundant(node, inst)) {
    //             assert(inst->GetOprands().size() > 0);
    //             VN[result] = GetVN(result);
    //             iter = inst_list.erase(iter);
    //             continue;
    //         } else {
    //             VN[result] = result;
    //         }
    //     }
    //     ++iter;
    // }

    for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
        auto inst = (*iter);

        for (auto oprand : inst->GetOprands()) {
            if (auto vn = GetVN(oprand); vn != nullptr && vn != oprand) {
                assert(inst->ReplaceSRC(oprand, vn));
            }
        }
        if (auto [replacee, replacer] = inst->DoFlod(); replacee != nullptr && replacer != nullptr) {
            ReplaceSRC(replacee, replacer);
            iter = inst_list.erase(iter);
            continue;
        }
        if (inst->IsTwoOprandInst() && inst->GetOpCode() <= OP_RSHIFT) {
            auto bin_inst = std::static_pointer_cast<BinaryInstruction>(inst);
            auto result = bin_inst->GetResult();

            if (auto res = scope.Get(bin_inst)) {
                VN[result] = res;
                iter = inst_list.erase(iter);
                continue;
            } else {
                VN[result] = result;
                scope.Set(bin_inst);
            }
        }
        ++iter;
    }

    for (auto child : node->GetDominateChildren()) {
        DoDVNT(child, &scope);
    }
}

void GVN::DVNT(NormalFuncPtr func) {
    for (auto param : func->GetParamList()) {
        VN[param] = param;
    }
    DoDVNT(func->GetEntryNode(), nullptr);
}