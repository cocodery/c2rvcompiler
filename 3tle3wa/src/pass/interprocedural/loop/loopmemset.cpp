#include "3tle3wa/pass/interprocedural/loop/loopmemset.hh"

void LoopMemset::LoopMemset(NormalFuncPtr func) {
    BaseValuePtr array = nullptr;
    BaseValuePtr init_num = nullptr;
    auto structure = func->loops->sub_structures;
    int begin_level = 0;
    LoopTreeTraversal(func->loops, array, init_num, begin_level);
}

// post-order travel loop tree
void LoopMemset::LoopTreeTraversal(Loop *loop, BaseValuePtr &array, BaseValuePtr &init_num, int &level) {
    auto &&sub_loops = loop->sub_structures;
    if (sub_loops.size() > 0) {
        for (auto &&iter = sub_loops.begin(); iter != sub_loops.end();) {
            auto &&sub_loop = static_cast<Loop *>(*iter);
            LoopTreeTraversal(sub_loop, array, init_num, level);
            ++iter;
        }
    } else if (sub_loops.size() == 0) {
        if (loop->before_blk == nullptr) {
            return;
        }
        // if isArrayAssignment is true, level++
        auto array_and_init = GetArrayInit(loop);
        if (array_and_init.first == nullptr) {
            return;
        } else {
            level++;
            array = array_and_init.first;
            init_num = array_and_init.second;
        }
    }
    if (loop->before_blk == nullptr) {
        return;
    }

    if (array == nullptr) {
        return;
    } else {
        // judge if loopbody equals entire subloop structure
        if (sub_loops.size() != 0) {
            if (sub_loops.size() > 1) {
                level = 0;
                array = nullptr;
                return;
            }
            auto &&sub_loop = static_cast<Loop *>(sub_loops.front());
            std::set<CfgNodePtr> loop_body_set;
            std::set<CfgNodePtr> entire_sub_loop_set;
            for (auto &&blk : loop->GetLoopBodyBlks()) {
                loop_body_set.insert(blk);
            }
            entire_sub_loop_set.insert(sub_loop->before_blk);
            entire_sub_loop_set.insert(sub_loop->loop_exit);
            for (auto &&blk : sub_loop->GetEntireStructure()) {
                entire_sub_loop_set.insert(blk);
            }
            if (loop_body_set != entire_sub_loop_set) {
                level = 0;
                array = nullptr;
                return;
            }
            if (sub_loop->before_blk->GetInstList().size() > 3 || sub_loop->loop_exit->GetInstList().size() > 3) {
                level = 0;
                array = nullptr;
                return;
            }
        }

        // check if loop step is 1
        if (CheckLoopStep(loop) == false) {
            level = 0;
            array = nullptr;
            return;
        }

        // judge if level is array dimension and if loop time equals upper of this dimension
        // true:memset
        // false:return
        int array_dimension;
        auto &&list = std::static_pointer_cast<ListType>(array->GetBaseType());
        array_dimension = list->GetArrDims().size();
        auto loop_time = LoopUnrolling::LoopTime(loop);
        int dimension_upper = list->GetArrDims().at(array_dimension - level);
        if (dimension_upper != loop_time) {
            level = 0;
            array = nullptr;
            return;
        }

        if (level == array_dimension) {
            // loop->PrintStructure();
            // std::cout << loop->body_begin->tollvmIR() << std::endl;
            //  MemsetArray(array, init_num);
            // std::cout << "\n\n\n";
            level = 0;
            array = nullptr;
            return;
        } else {
            level++;
            return;
        }
        return;
    }
}

ArrayAndInit LoopMemset::GetArrayInit(Loop *loop) {
    ArrayAndInit result;
    int store_num = 0;
    result.first = nullptr;
    result.second = nullptr;
    auto &&sub_loops = loop->sub_structures;
    auto &&loop_body = loop->body_begin;
    auto inst_list = loop_body->GetInstList();
    inst_list.pop_back();
    GepInstPtr gep_inst = nullptr;
    StoreInstPtr store_inst = nullptr;
    if (sub_loops.size() != 0) {
        // not the deepest loop
        return result;
    }
    if (loop->IsSimpleLoop() == false) {
        // not simple loop
        return result;
    }
    if (loop->GetLoopBodyBlks().size() != 1) {
        // loop body size more than 1
        return result;
    }
    for (auto &&inst : loop_body->GetInstList()) {
        if (inst->IsStoreInst()) {
            store_inst = std::static_pointer_cast<StoreInst>(inst);
            store_num++;
            inst_list.remove(inst);
        }
    }
    if (store_num != 1) {
        // store_num != 1
        return result;
    }
    auto &&store_addr = store_inst->GetStoreAddr();
    auto &&store_value = store_inst->GetStoreValue();
    if (store_value->IsConstant() == false) {
        // store value is not constant
        return result;
    }
    auto &&addr_parent = store_addr->GetParent();
    if (addr_parent->IsGepInst() == false) {
        // not array
        return result;
    }
    gep_inst = std::static_pointer_cast<GetElementPtrInst>(addr_parent);
    inst_list.remove(addr_parent);
    auto &&offset_list = gep_inst->GetOffList();
    auto &&base_addr = gep_inst->GetBaseAddr();
    if (base_addr->GetBaseType()->IsArray() == false) {
        auto base_addr_parent = base_addr->GetParent();
        if (base_addr_parent->IsGepInst() == false) {
            // not array
            return result;
        }
        auto base_addr_parent_gep = std::static_pointer_cast<GetElementPtrInst>(base_addr_parent);
        base_addr = base_addr_parent_gep->GetBaseAddr();
    }
    for (auto &&offset : offset_list) {
        if (offset->IsConstant()) {
            continue;
        }
        auto &&offset_init_inst = offset->GetParent();
        for (auto &&operand : offset_init_inst->GetOprands()) {
            if (operand->IsConstant()) {
                continue;
            }
            auto operand_parent = operand->GetParent();
            bool in_body_blk = FindInst(operand_parent, loop_body);
            bool in_cond_blk = FindInst(operand_parent, loop->cond_begin);
            bool in_before_blk = FindInst(operand_parent, loop->before_blk);
            if ((in_body_blk || in_cond_blk || in_before_blk) == false) {
                // defined out loop
                return result;
            }
            if (in_body_blk) {
                inst_list.remove(operand_parent);
            }
        }
        if (FindInst(offset_init_inst, loop_body)) {
            inst_list.remove(offset_init_inst);
        }
    }

    if (inst_list.size() != 0) {
        if (inst_list.size() > 1) {
            return result;
        } else {
            auto last_inst = inst_list.front();
            for (auto operand : last_inst->GetOprands()) {
                if (operand->IsConstant()) {
                    continue;
                }
                auto operand_parent = operand->GetParent();
                bool in_body_blk = FindInst(operand_parent, loop_body);
                bool in_cond_blk = FindInst(operand_parent, loop->cond_begin);
                bool in_before_blk = FindInst(operand_parent, loop->before_blk);
                if ((in_body_blk || in_cond_blk || in_before_blk) == false) {
                    return result;
                }
            }
        }
    }

    result.first = base_addr;
    result.second = store_value;
    return result;
}

bool LoopMemset::FindInst(InstPtr inst, CfgNodePtr cfgNode) {
    auto &&inst_list = cfgNode->GetInstList();
    if (std::find(inst_list.begin(), inst_list.end(), inst) == inst_list.end()) {
        return false;
    } else {
        return true;
    }
}

bool LoopMemset::CheckLoopStep(Loop *loop) {
    int32_t change_val = 0;
    int32_t init = 0;
    auto branch = loop->cond_begin->GetInstList().back();
    if (branch->IsBranchInst() == false) {
        return false;
    }
    auto branch_inst = std::static_pointer_cast<BranchInst>(branch);
    auto cmp_inst = branch_inst->GetCondition()->GetParent();
    if (cmp_inst == nullptr) {
        return false;
    }
    auto opcode = cmp_inst->GetOpCode();
    if (opcode < OP_LTH) {
        return false;
    }
    int const_check = LoopUnrolling::ConstCheck(cmp_inst);
    if (const_check == 0) {
        return false;
    }
    auto operands_of_cmp_vector = LoopUnrolling::InstOperandsInVector(cmp_inst);
    auto key = operands_of_cmp_vector.at(2 - const_check);
    auto cmp_value = operands_of_cmp_vector.at(const_check - 1);
    if (cmp_value->IsConstant() == false || cmp_value->GetBaseType()->IntType() == false) {
        return false;
    }
    auto key_parent = key->GetParent();
    if (key_parent == nullptr) {
        return false;
    }
    if (key_parent->IsPhiInst() == false) {
        return false;
    }

    auto phi_inst = std::static_pointer_cast<PhiInst>(key_parent);
    auto loop_blks = loop->GetLoopBodyBlks();

    // start to get init value of key
    auto data_list = phi_inst->GetDataList();
    auto data_source1 = data_list.front();
    auto data_source2 = *std::next(data_list.begin(), 1);
    auto source_value1 = data_source1.first;
    auto source_blk1 = data_source1.second;
    auto source_value2 = data_source2.first;
    auto source_blk2 = data_source2.second;

    InstPtr phi_part_defined_in_loop;
    BaseValuePtr value_defined_in_loop;
    if (std::find(loop_blks.begin(), loop_blks.end(), source_blk1) != loop_blks.end()) {
        value_defined_in_loop = source_value1;
        phi_part_defined_in_loop = source_value1->GetParent();
        auto const_init_val = std::static_pointer_cast<Constant>(source_value2);
        if (const_init_val == nullptr) {
            return false;
        }
        if (!std::holds_alternative<int32_t>(const_init_val->GetValue()) &&
            !std::holds_alternative<int64_t>(const_init_val->GetValue())) {
            return false;
        }
        init = std::get<int32_t>(const_init_val->GetValue());
    } else if (std::find(loop_blks.begin(), loop_blks.end(), source_blk2) != loop_blks.end()) {
        value_defined_in_loop = source_value2;
        phi_part_defined_in_loop = source_value2->GetParent();
        auto const_init_val = std::static_pointer_cast<Constant>(source_value1);
        if (const_init_val == nullptr) {
            return false;
        }
        if (!std::holds_alternative<int32_t>(const_init_val->GetValue()) &&
            !std::holds_alternative<int64_t>(const_init_val->GetValue())) {
            return false;
        }
        init = std::get<int32_t>(const_init_val->GetValue());
    } else {
        return false;
    }

    if (init != 0) {
        // not init from 0
        return false;
    }

    while (value_defined_in_loop != key) {
        if (phi_part_defined_in_loop == nullptr) {
            return false;
        }
        if (phi_part_defined_in_loop->GetOpCode() == OP_ADD) {
            auto operands = LoopUnrolling::InstOperandsInVector(phi_part_defined_in_loop);
            const_check = LoopUnrolling::ConstCheck(phi_part_defined_in_loop);
            auto const_val_in_phi = operands.at(const_check - 1);
            if (const_val_in_phi->IsConstant() == false) {
                return false;
            }
            auto constant_in_phi = std::static_pointer_cast<Constant>(const_val_in_phi)->GetValue();
            if (!std::holds_alternative<int32_t>(constant_in_phi) &&
                !std::holds_alternative<int64_t>(constant_in_phi)) {
                // change value of key is not int
                return false;
            }
            change_val += std::get<int32_t>(constant_in_phi);
            value_defined_in_loop = operands.at(2 - const_check);
        } else if (phi_part_defined_in_loop->GetOpCode() == OP_SUB) {
            auto operands = LoopUnrolling::InstOperandsInVector(phi_part_defined_in_loop);
            const_check = LoopUnrolling::ConstCheck(phi_part_defined_in_loop);
            auto const_val_in_phi = operands.at(const_check - 1);
            if (const_val_in_phi->IsConstant() == false) {
                return false;
            }
            auto constant_in_phi = std::static_pointer_cast<Constant>(const_val_in_phi)->GetValue();
            if (!std::holds_alternative<int32_t>(constant_in_phi) &&
                !std::holds_alternative<int64_t>(constant_in_phi)) {
                // change value is not int
                return false;
            }
            change_val -= std::get<int32_t>(constant_in_phi);
            value_defined_in_loop = operands.at(2 - const_check);
        } else {
            return false;
        }
    }
    if (change_val != 1) {
        return false;
    }
    return true;
}

void RemoveLoop(Loop *loop) {
    
}

void LoopMemset::MemsetArray(BaseValuePtr array, BaseValuePtr init_num) {}
