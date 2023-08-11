#include "3tle3wa/pass/interprocedural/loop/loopunrolling.hh"
void LoopUnrolling::LoopUnrolling(NormalFuncPtr func) {
    auto &&loops = func->loops;
    ExpandLoop(loops);
}

void LoopUnrolling::ExpandLoop(Loop *loop) {
    for (auto &&sub_loop : loop->sub_structures) {
        ExpandLoop(static_cast<Loop *>(sub_loop));
    }
    if (loop->before_blk) {
        auto loop_cond = loop->GetCondBodyBlks();
        auto loop_blks = loop->GetLoopBodyBlks();
        if (loop_cond.size() > 1 || loop_blks.size() > 1) {
            return;
        }
        auto loop_time = LoopTime(loop);
        if (loop_time == 0 || loop_time > 500) {
            return;
        }
        FullyExpand(loop_time, loop);
    }
}

void LoopUnrolling::FullyExpand(int loop_time, Loop *loop) {
    auto &&before_blk = loop->before_blk;
    auto loop_blks = loop->GetEntireStructure();
    auto cond_begin = loop->cond_begin;
    auto loop_end = loop_blks.back();
    auto loop_exit = loop->loop_exit;

    // remove loop branch inst
    auto &&entry_terminator = before_blk->GetInstList().back();
    if (entry_terminator->IsJumpInst() == false) {
        return;
    }
    auto &&loop_branch = std::static_pointer_cast<BranchInst>(entry_terminator);
    auto &&out_loop_blk = loop_branch->GetFalseTarget();
    auto &&body_terminator = loop_end->GetInstList().back();
    if (body_terminator->IsJumpInst() == false) {
        return;
    }
    before_blk->RemoveInst(entry_terminator);
    loop_end->RemoveInst(body_terminator);

    // START
    // Record the mapping of the result of the phi instruction to its sources (from outside the loop
    // or within the loop) for updating instructions using the result during the loop iteration
    phi_results.clear();
    phi_source_defined_in_loop.clear();
    phi_source_defined_out_loop.clear();
    phi_source_updated.clear();
    old_to_new.clear();

    for (auto &&inst : cond_begin->GetInstList()) {
        if (inst->IsPhiInst()) {
            auto &&result = inst->GetResult();
            auto &&phi_inst = std::static_pointer_cast<PhiInst>(inst);
            auto &&data_list = phi_inst->GetDataList();
            auto &&data_source1 = data_list.front();
            auto &&data_source2 = *std::next(data_list.begin(), 1);
            auto &&source_value1 = data_source1.first;
            auto &&source_blk1 = data_source1.second;
            auto &&source_value2 = data_source2.first;
            auto &&source_blk2 = data_source2.second;

            phi_results.insert(result);
            if (std::find(loop_blks.begin(), loop_blks.end(), source_blk1) != loop_blks.end()) {
                phi_source_defined_in_loop[result] = source_value1;
                phi_source_defined_out_loop[result] = source_value2;
            } else if (std::find(loop_blks.begin(), loop_blks.end(), source_blk2) != loop_blks.end()) {
                phi_source_defined_in_loop[result] = source_value2;
                phi_source_defined_out_loop[result] = source_value1;
            } else {
                continue;
            }
        }
    }
    // ENDS

    // copy insts
    bool init_flag = true;
    for (int i = 0; i < loop_time; ++i) {
        for (auto &&inst : loop_end->GetInstList()) {
            if (inst->IsReturnInst() || inst->IsPhiInst()) {
                return;
            }
            if (inst->IsStoreInst() == false) {
                auto new_value = InstCopy(inst, before_blk, init_flag);
                if (new_value == nullptr) {
                    continue;
                }
                auto new_inst = new_value->GetParent();
                old_to_new[inst->GetResult()] = new_value;
            } else {
                auto &&binary_inst_ = std::static_pointer_cast<BinaryInstruction>(inst);
                auto &&lhs = binary_inst_->GetLHS();
                auto &&rhs = binary_inst_->GetRHS();
                lhs = OperandUpdate(lhs, init_flag);
                rhs = OperandUpdate(rhs, init_flag);
                StoreInst::DoStoreValue(lhs, rhs, cond_begin);
            }
        }
        init_flag = false;
        for (auto pair : old_to_new) {
            phi_source_updated[pair.first] = old_to_new[pair.second];
        }
    }

    // remove all loop blocks
    // for (auto &&blk : loop_blks) {
    //     RemoveNode(blk);
    // }

    before_blk->RmvSuccessor(cond_begin);
    loop_exit->RmvPredecessor(cond_begin);
    JumpInstPtr to_loop_exit = JumpInst::CreatePtr(loop_exit, before_blk);
    before_blk->InsertInstBack(to_loop_exit);
}

int LoopUnrolling::LoopTime(Loop *loop) {
    int32_t time = 0;
    auto loop_blks = loop->GetLoopBodyBlks();
    auto cond_begin = loop->cond_begin;
    auto loop_end = loop_blks.back();
    if (cond_begin->GetPredecessors().size() > 2) {
        return 0;
    }
    if (loop_end->GetSuccessors().size() > 1) {
        return 0;
    }

    auto cond_end = loop->cond_end;
    auto last_inst = cond_end->GetInstList().back();
    if (last_inst->IsBranchInst() && last_inst->GetOprands().size() != 0) {
        auto &&last_inst_oprands = last_inst->GetOprands();
        auto &&end_value = last_inst_oprands.front();
        auto &&cmp_inst = end_value->GetParent();
        if (cmp_inst == nullptr) {
            std::cout << "br: " << last_inst->tollvmIR() << std::endl;
            std::cout << "cmp_inst is nullptr" << std::endl;
            return 0;
        }
        auto opcode = cmp_inst->GetOpCode();
        if (opcode < OP_LTH) {
            std::cout << "Opcode is less than LTH" << std::endl;
            return 0;
        }

        int32_t limit;
        int32_t init;
        int32_t change_val = 0;
        int const_check = ConstCheck(cmp_inst);
        if (const_check == 0) {
            std::cout << "both cmp operands are not constant" << std::endl;
            std::cout << cmp_inst->tollvmIR() << std::endl;
            return 0;
        }

        auto operands_of_cmp_vector = InstOperandsInVector(cmp_inst);

        // key:obtain key of the very compare instruction
        // limit:obtain loop boundary value,restricted to type int
        auto key = operands_of_cmp_vector.at(2 - const_check);
        auto &&cmp_value = operands_of_cmp_vector.at(const_check - 1);
        if (cmp_value->IsConstant() == false || cmp_value->GetBaseType()->IntType() == false) {
            std::cout << cmp_inst->tollvmIR() << std::endl;
            std::cout << "cmp_value is not int" << std::endl;
            return 0;
        }
        auto &&constant_value = std::static_pointer_cast<Constant>(cmp_value);
        auto const_val = constant_value->GetValue();
        if (!std::holds_alternative<int32_t>(const_val) && !std::holds_alternative<int64_t>(const_val)) {
            std::cout << "when get compare limit, type is not int32_t or int64_t" << std::endl;
            return 0;
        }
        limit = std::get<int32_t>(const_val);

        // Change the form of the comparison instruction to
        // "VARIANT OPCODE VALUE"
        // VARIANT:variant key in LoopTime function
        // VALUE:variant limit in LoopTime function
        if (const_check == 1) {
            opcode = FlipComparisonOp(opcode);
        }

        auto key_parent = key->GetParent();
        if (key_parent == nullptr) {
            return 0;
        }
        if (key_parent->IsPhiInst() == false) {
            std::cout << "compare key is not from phi inst" << std::endl;
            return 0;
        }

        auto &&phi_inst = std::static_pointer_cast<PhiInst>(key_parent);
        // std::cout << phi_inst->tollvmIR() << std::endl;

        // start to get init value of key
        auto &&data_list = phi_inst->GetDataList();
        auto &&data_source1 = data_list.front();
        auto &&data_source2 = *std::next(data_list.begin(), 1);
        auto &&source_value1 = data_source1.first;
        auto &&source_blk1 = data_source1.second;
        auto &&source_value2 = data_source2.first;
        auto &&source_blk2 = data_source2.second;

        InstPtr phi_part_defined_in_loop;
        BaseValuePtr value_defined_in_loop;
        if (std::find(loop_blks.begin(), loop_blks.end(), source_blk1) != loop_blks.end()) {
            value_defined_in_loop = source_value1;
            phi_part_defined_in_loop = source_value1->GetParent();
            auto const_init_val = std::static_pointer_cast<Constant>(source_value2);
            if (const_init_val == nullptr) {
                return 0;
            }
            if (!std::holds_alternative<int32_t>(const_init_val->GetValue()) &&
                !std::holds_alternative<int64_t>(const_init_val->GetValue())) {
                std::cout << "init of key is not int" << std::endl;
                std::cout << source_value2->tollvmIR() << std::endl;
                return 0;
            }
            init = std::get<int32_t>(const_init_val->GetValue());
        } else if (std::find(loop_blks.begin(), loop_blks.end(), source_blk2) != loop_blks.end()) {
            value_defined_in_loop = source_value2;
            phi_part_defined_in_loop = source_value2->GetParent();
            auto const_init_val = std::static_pointer_cast<Constant>(source_value1);
            if (const_init_val == nullptr) {
                return 0;
            }
            if (!std::holds_alternative<int32_t>(const_init_val->GetValue()) &&
                !std::holds_alternative<int64_t>(const_init_val->GetValue())) {
                std::cout << "init of key is not int" << std::endl;
                std::cout << source_value1->tollvmIR() << std::endl;
                return 0;
            }
            init = std::get<int32_t>(const_init_val->GetValue());
        } else {
            std::cout << "both phi oprands are not from the loop" << std::endl;
            return 0;
        }
        // end of finding init value of key

        // obtain the change in value of the key during loop iteration
        while (value_defined_in_loop != key) {
            if (phi_part_defined_in_loop == nullptr) {
                return 0;
            }
            if (phi_part_defined_in_loop->GetOpCode() == OP_ADD) {
                auto operands = InstOperandsInVector(phi_part_defined_in_loop);
                const_check = ConstCheck(phi_part_defined_in_loop);
                auto const_val_in_phi = operands.at(const_check - 1);
                if (const_val_in_phi->IsConstant() == false) {
                    return 0;
                }
                auto constant_in_phi = std::static_pointer_cast<Constant>(const_val_in_phi)->GetValue();
                if (!std::holds_alternative<int32_t>(constant_in_phi) &&
                    !std::holds_alternative<int64_t>(constant_in_phi)) {
                    std::cout << "change value of key is not int" << std::endl;
                    return 0;
                }
                change_val += std::get<int32_t>(constant_in_phi);
                value_defined_in_loop = operands.at(2 - const_check);
            } else if (phi_part_defined_in_loop->GetOpCode() == OP_SUB) {
                auto operands = InstOperandsInVector(phi_part_defined_in_loop);
                const_check = ConstCheck(phi_part_defined_in_loop);
                auto const_val_in_phi = operands.at(const_check - 1);
                if (const_val_in_phi->IsConstant() == false) {
                    return 0;
                }
                auto constant_in_phi = std::static_pointer_cast<Constant>(const_val_in_phi)->GetValue();
                if (!std::holds_alternative<int32_t>(constant_in_phi) &&
                    !std::holds_alternative<int64_t>(constant_in_phi)) {
                    std::cout << "change value is not int" << std::endl;
                    return 0;
                }
                change_val -= std::get<int32_t>(constant_in_phi);
                value_defined_in_loop = operands.at(2 - const_check);
            } else {
                return 0;
            }
        }
        // obtain the change in value of the key during loop iteration ENDs

        int32_t gap = init - limit;
        switch (opcode) {
            case OP_GEQ:
                if (gap < 0) {
                    return -1;
                } else {
                    if (change_val >= 0) {
                        return 0;
                    }
                    while (gap >= 0) {
                        gap += change_val;
                        time++;
                    }
                    return time;
                }
                break;
            case OP_LEQ:
                if (gap > 0) {
                    return -1;
                } else {
                    if (change_val <= 0) {
                        return 0;
                    }
                    while (gap <= 0) {
                        gap += change_val;
                        time++;
                    }
                    return time;
                }
                break;
            case OP_GTH:
                if (gap <= 0) {
                    return -1;
                } else {
                    if (change_val >= 0) {
                        return 0;
                    }
                    while (gap >= 0) {
                        gap += change_val;
                        time++;
                    }
                    return time;
                }
                break;
            case OP_LTH:
                if (gap >= 0) {
                    return -1;
                } else {
                    if (change_val <= 0) {
                        return 0;
                    }
                    while (gap < 0) {
                        gap += change_val;
                        time++;
                    }
                    return time;
                }
                break;
            default:
                return 0;
                break;
        }
    } else {
        return 0;
    }
    return 0;
}

int LoopUnrolling::ConstCheck(InstPtr inst) {
    // Determine if the operands of the specific comparison instruction are constants.
    // Returns:
    // - 0 if neither of the operands is a constant
    // - 1 if the left operand is a constant
    // - 2 if the right operand is a constant
    // It's unlikely for both operands to be constants.
    if (inst->IsTwoOprandInst() == false) {
        throw("not twoOperand instruction");
    }
    auto &&inst_oprands = inst->GetOprands();
    auto lhs = *inst_oprands.begin();
    auto rhs = *std::next(inst_oprands.begin(), 1);
    if (lhs->IsConstant() && rhs->IsConstant() == false) {
        return 1;
    } else if (rhs->IsConstant() && lhs->IsConstant() == false) {
        return 2;
    }
    return 0;
}

OpCode LoopUnrolling::FlipComparisonOp(OpCode opcode) {
    switch (opcode) {
        case OpCode::OP_LTH:
            return OpCode::OP_GTH;
        case OpCode::OP_LEQ:
            return OpCode::OP_GEQ;
        case OpCode::OP_GTH:
            return OpCode::OP_LTH;
        case OpCode::OP_GEQ:
            return OpCode::OP_LEQ;
        default:
            return opcode;
    }
}

Operands LoopUnrolling::InstOperandsInVector(InstPtr inst) {
    Operands operands_in_vector;
    for (auto &&operand : inst->GetOprands()) {
        operands_in_vector.push_back(operand);
    }
    return operands_in_vector;
}

BaseValuePtr LoopUnrolling::InstCopy(InstPtr &inst_, CfgNodePtr &parent, bool init_flag) {
    assert(!(inst_->IsReturnInst() || inst_->IsPhiInst()));
    BaseValuePtr result = nullptr;
    OpCode opcode = inst_->GetOpCode();
    if (inst_->IsOneOprandInst()) {
        auto &&unary_inst_ = std::static_pointer_cast<UnaryInstruction>(inst_);
        auto &&oprand = unary_inst_->GetOprand();
        oprand = OperandUpdate(oprand, init_flag);
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
        } else if (opcode == FNeg) {
            result = FNegInst::DoFloatNeg(oprand, parent);
        } else {
            assert(false);
        }
    } else if (inst_->IsTwoOprandInst()) {
        auto &&binary_inst_ = std::static_pointer_cast<BinaryInstruction>(inst_);
        auto &&lhs = binary_inst_->GetLHS();
        auto &&rhs = binary_inst_->GetRHS();
        lhs = OperandUpdate(lhs, init_flag);
        rhs = OperandUpdate(rhs, init_flag);
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
    } else if (inst_->IsGepInst()) {
        auto &&gep_inst_ = std::static_pointer_cast<GetElementPtrInst>(inst_);
        OffsetList off_list;
        for (auto &&off : gep_inst_->GetOffList()) {
            auto result = OperandUpdate(off, init_flag);
            off_list.push_back(result);
        }
        auto new_addr = OperandUpdate(gep_inst_->GetBaseAddr(), init_flag);
        result = GetElementPtrInst::DoGetPointer(gep_inst_->GetStoreType(), new_addr, off_list, parent);
    } else if (inst_->IsCallInst()) {
        auto &&call_inst_ = std::static_pointer_cast<CallInst>(inst_);
        ParamList param_list;
        for (auto &&param : call_inst_->GetParamList()) {
            auto result = OperandUpdate(param, init_flag);
            param_list.push_back(result);
        }
        result = CallInst::DoCallFunction(call_inst_->GetRetType(), call_inst_->GetCalleeFunc(), param_list, parent);
    }
    assert((result != nullptr) || (inst_->IsCallInst()));
    return result;
}

BaseValuePtr LoopUnrolling::OperandUpdate(BaseValuePtr operand, bool init_flag) {
    BaseValuePtr result = nullptr;
    if (phi_results.find(operand) == phi_results.end()) {
        return operand;
    }
    if (init_flag) {
        if (phi_source_defined_out_loop[operand] != nullptr) {
            result = phi_source_defined_out_loop[operand];
        }
    }
    auto old_value = phi_source_defined_in_loop[operand];
    if (old_value != nullptr) {
        if (old_to_new[old_value] != nullptr) {
            result = old_to_new[old_value];
        }
    }
    old_to_new[operand] = result;
    return result;
}