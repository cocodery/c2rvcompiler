#include "inline.hh"

BaseValuePtr Inline::InstCopy(InstPtr inst_, CfgNodePtr parent) {
    assert(!(inst_->IsReturnInst() || inst_->IsPhiInst()));
    BaseValuePtr result = nullptr;
    OpCode opcode = inst_->GetOpCode();
    if (inst_->IsOneOprandInst()) {
        auto &&unary_inst_ = std::static_pointer_cast<UnaryInstruction>(inst_);
        auto &&oprand = value_map[unary_inst_->GetOprand()];
        if (opcode == Load) {
            result = LoadInst::DoLoadValue(oprand, parent);
        } else if (opcode == BitCast) {
            result = BitCastInst::DoBitCast(oprand, parent);
        } else if (opcode == SiToFp) {
            result = SitoFpInst::DoSitoFp(oprand, parent);
        } else if (opcode == FpToSi) {
            auto type = unary_inst_->GetResult()->GetBaseType()->GetAttrType();
            result = FptoSiInst::DoFptoSi(type, oprand, parent);
        } else if (opcode == Zext) {
            result = ZextInst::DoZeroExt(oprand, parent);
        } else {
            assert(false);
        }
    } else if (inst_->IsTwoOprandInst()) {
        auto &&binary_inst_ = std::static_pointer_cast<BinaryInstruction>(inst_);
        auto &&lhs = binary_inst_->GetLHS()->IsConstant() ? binary_inst_->GetLHS() : value_map[binary_inst_->GetLHS()];
        auto &&rhs = binary_inst_->GetRHS()->IsConstant() ? binary_inst_->GetRHS() : value_map[binary_inst_->GetRHS()];
        if (binary_inst_->IsIBinaryInst()) {
            result = IBinaryInst::DoIBinOperate(opcode, lhs, rhs, parent);
        } else if (binary_inst_->IsFBinaryInst()) {
            result = FBinaryInst::DoFBinOperate(opcode, lhs, rhs, parent);
        } else if (binary_inst_->IsICmpInst()) {
            result = ICmpInst::DoICompare(opcode, lhs, rhs, parent);
        } else if (binary_inst_->IsFCmpInst()) {
            result = FCmpInst::DoFCompare(opcode, lhs, rhs, parent);
        } else {
            assert(false);
        }
        // } else if (inst_->IsAllocaInst()) {
        //     auto alloca_inst_ = std::static_pointer_cast<AllocaInst>(inst_);
        //     result = AllocaInst::DoAllocaAddr(alloca_inst_->GetAllocaType(),
        //     alloca_inst_->GetAllocaAddr()->GetBaseType(),
        //                                       parent);
    } else if (inst_->IsGepInst()) {
        auto &&gep_inst_ = std::static_pointer_cast<GetElementPtrInst>(inst_);
        BaseValueList off_list;
        for (auto &&off : gep_inst_->GetOffList()) {
            if (off->IsConstant())
                off_list.push_back(off);
            else
                off_list.push_back(value_map[off]);
        }
        result = GetElementPtrInst::DoGetPointer(gep_inst_->GetStoreType(), value_map[gep_inst_->GetBaseAddr()],
                                                 off_list, parent);
    } else if (inst_->IsCallInst()) {
        auto &&call_inst_ = std::static_pointer_cast<CallInst>(inst_);
        ParamList param_list;
        for (auto &&param : call_inst_->GetParamList()) {
            if (param->IsConstant())
                param_list.push_back(param);
            else
                param_list.push_back(value_map[param]);
        }
        result = CallInst::DoCallFunction(call_inst_->GetRetType(), call_inst_->GetCalleeFunc(), param_list, parent);
    }
    assert((result != nullptr) || (inst_->IsCallInst()));
    return result;
}

std::pair<BaseValuePtr, CfgNodePtr> Inline::Inline(NormalFuncPtr caller, NormalFuncPtr callee, ParamList &param_list,
                                                   NameValueMap &glb_table, CfgNodePtr cur_block, bool in_loop,
                                                   CfgNodePtr out_loop_block) {
    assert(value_map.empty() && block_map.empty());

    BaseValuePtr ret_value = nullptr;
    CfgNodePtr ret_block = nullptr;

    // init value-map with global-value and func-parameter
    for (auto &&[_, value] : glb_table) value_map[value] = value;
    for (size_t size = param_list.size(), idx = 0; idx < size; ++idx)
        value_map[callee->GetParamList()[idx]] = param_list[idx];

    // terminate-inst store here wait for jump-target
    std::list<std::pair<JumpInstPtr, JumpInstPtr>> jump_list;
    std::list<std::pair<BranchInstPtr, BranchInstPtr>> branch_list;

    for (auto node : callee->TopoSortFromEntry()) {
        block_map[node] = cur_block;

        for (auto inst_ : node->GetInstList()) {
            if (inst_->IsReturnInst()) {  // inst->ret-inst, set ret_value and block
                auto &&ret_inst = std::static_pointer_cast<ReturnInst>(inst_);
                ret_value = value_map[ret_inst->GetRetValue()];
                ret_block = cur_block;
            } else if (inst_->IsJumpInst()) {
                auto &&jump_inst = JumpInst::CreatePtr(nullptr, cur_block);
                cur_block->InsertInstBack(jump_inst);
                jump_list.push_back({jump_inst, std::static_pointer_cast<JumpInst>(inst_)});
            } else if (inst_->IsBranchInst()) {
                auto &&branch_inst_ = std::static_pointer_cast<BranchInst>(inst_);
                auto &&cond = branch_inst_->GetCondition()->IsConstant() ? branch_inst_->GetCondition()
                                                                         : value_map[branch_inst_->GetCondition()];
                auto &&branch_inst = BranchInst::CreatePtr(cond, nullptr, nullptr, cur_block);
                cur_block->InsertInstBack(branch_inst);
                branch_list.push_back({branch_inst, branch_inst_});
            } else if (inst_->IsStoreInst()) {
                auto &&store_inst = std::static_pointer_cast<StoreInst>(inst_);
                auto &&store_value = store_inst->GetStoreValue()->IsConstant() ? store_inst->GetStoreValue()
                                                                               : value_map[store_inst->GetStoreValue()];
                StoreInst::DoStoreValue(value_map[store_inst->GetStoreAddr()], store_value, cur_block);
            } else if (inst_->IsAllocaInst()) {
                auto alloca_inst_ = std::static_pointer_cast<AllocaInst>(inst_);
                VariablePtr result = Variable::CreatePtr(alloca_inst_->GetAllocaAddr()->GetBaseType(), nullptr);
                auto &&inst = AllocaInst::CreatePtr(alloca_inst_->GetAllocaType(), result, nullptr);
                result->SetParent(inst);
                if (in_loop) {
                    out_loop_block->GetInstList().insert(--(out_loop_block->GetInstList().end()), inst);
                    inst->SetParent(out_loop_block);
                } else {
                    cur_block->InsertInstBack(inst);
                    inst->SetParent(cur_block);
                }
                value_map[inst_->GetResult()] = result;
            } else {  // insert inst->copy to cur_block
                value_map[inst_->GetResult()] = InstCopy(inst_, cur_block);
            }
        }
        cur_block = caller->CreateCfgNode(node->GetBlockAttr() & ~(ENTRY | EXIT | GORETURN));
    }
    // fill jump-target
    for (auto &&[jump1, jump2] : jump_list) {
        jump1->SetTarget(block_map[jump2->GetTarget()]);
    }
    for (auto &&[branch1, branch2] : branch_list) {
        branch1->SetTrueTarget(block_map[branch2->GetTrueTarget()]);
        branch1->SetFalseTarget(block_map[branch2->GetFalseTarget()]);
    }
    // after inline, clear
    value_map.clear();
    block_map.clear();

    return {ret_value, ret_block};
}