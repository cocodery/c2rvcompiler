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
        int looptime = LoopTime(loop);
        std::cout << "loop time is " << looptime << std::endl;
    }
}

int LoopUnrolling::LoopTime(Loop *loop) {
    int32_t time = 0;
    auto loop_blks = loop->GetEntireStructure();
    auto cond_begin = loop->cond_begin;
    auto loop_end = loop->loop_exit;
    if (cond_begin->GetPredecessors().size() > 2) {
        return 0;
    }
    if (loop_end->GetSuccessors().size() > 1) {
        return 0;
    }

    auto cond_end = loop->cond_end;
    auto last_inst = cond_end->GetLastInst();
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
            std::cout << "\n\n";
            std::cout << "both cmp operands are not constant" << std::endl;
            std::cout << cmp_inst->tollvmIR() << std::endl;
            std::cout << "\n\n";
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
            // case OP_NEQ:
            //     if (gap == 0) {
            //         return -1;
            //     } else {
            //         if (gap % change_val != 0) {
            //             return 0;
            //         }
            //         return gap / change_val;
            //     }
            //     break;
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