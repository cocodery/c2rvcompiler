#include "3tle3wa/pass/interprocedural/instcomb/instcomb.hh"

#include <fcntl.h>

#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <variant>

#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/opCode.hh"
#include "3tle3wa/ir/value/constant.hh"

InstComb::BinType InstComb::GetBinType(const BaseValue *lhs, const BaseValue *rhs) {
    if (lhs->IsVariable() && rhs->IsVariable()) {  // bin_inst = variable OP variable
        return BinType::LVRV;
    } else if (lhs->IsVariable() && rhs->IsConstant()) {  // bin_inst = variable OP constant
        return BinType::LVRC;
    } else if (lhs->IsConstant() && rhs->IsVariable()) {  // bin_inst = constant OP variable
        return BinType::LCRV;
    } else if (lhs->IsConstant() && rhs->IsConstant()) {  // bin_inst = constant OP constant
        return BinType::LCRC;
    }
    assert(false);
}

void InstComb::InstCombine(NormalFuncPtr func) {
    auto &&all_ndoes = func->TopoSortFromEntry();

    // move constant or low-addr oprand to rhs
    for (auto &&node : all_ndoes) {
        for (auto &&inst : node->GetInstList()) {
            if (inst->IsTwoOprandInst()) {
                std::static_pointer_cast<BinaryInstruction>(inst)->SwapOprand();
            }
        }
    }

    for (auto &&node : all_ndoes) {
        auto &&inst_list = node->GetInstList();
        for (auto &&iter = inst_list.begin(); iter != inst_list.end(); ++iter) {
            auto &&inst = (*iter);

            // only combine binary-inst
            if (inst->IsTwoOprandInst() == false) {
                continue;
            }
            // process binary-inst
            auto &&bin_inst = std::static_pointer_cast<BinaryInstruction>(inst);
            auto bin_opcode = bin_inst->GetOpCode();
            auto &&bin_lhs = bin_inst->GetLHS();
            auto &&bin_rhs = bin_inst->GetRHS();

            auto &&lhs_inst = bin_lhs->GetParent();
            auto &&rhs_inst = bin_rhs->GetParent();

            // both lhs and rhs come from constant or parameter
            if (lhs_inst == nullptr && rhs_inst == nullptr) {
                continue;
            }

            auto bin_type = GetBinType(bin_lhs.get(), bin_rhs.get());
            if (bin_type == LVRV) {
                // bin_inst = variable OP variable
                if (lhs_inst && rhs_inst) {
                } else if (lhs_inst) {
                } else if (rhs_inst) {
                } else {
                    assert(false);
                }
            } else if (bin_type == LVRC && lhs_inst && lhs_inst->IsTwoOprandInst() && lhs_inst->GetParent() == node) {
                // bin_inst = variable OP constant
                auto &&bin_rhs_constant = std::static_pointer_cast<Constant>(bin_rhs);

                auto &&bin_lhs_inst = std::static_pointer_cast<BinaryInstruction>(lhs_inst);
                auto bin_lhs_opcode = bin_lhs_inst->GetOpCode();
                auto &&bin_lhs_lhs = bin_lhs_inst->GetLHS();
                auto &&bin_lhs_rhs = bin_lhs_inst->GetRHS();
                assert(bin_lhs == bin_lhs_inst->GetResult());

                if (!((OP_ADD <= bin_opcode && bin_opcode <= OP_DIV) &&
                      (OP_ADD <= bin_lhs_opcode && bin_lhs_opcode <= OP_DIV))) {
                    continue;
                }
                assert((bin_inst->IsIBinaryInst() && bin_lhs_inst->IsIBinaryInst()) ||
                       (bin_inst->IsFBinaryInst() && bin_lhs_inst->IsFBinaryInst()));

                auto lhs_bin_type = GetBinType(bin_lhs_lhs.get(), bin_lhs_rhs.get());
                if (lhs_bin_type == LVRC) {
                    auto &&bin_lhs_rhs_constant = std::static_pointer_cast<Constant>(bin_lhs_rhs);

                    ConstType value;
                    bool combine = true;
                    bool iop = (bin_inst->IsIBinaryInst() && bin_lhs_inst->IsIBinaryInst());

                    std::visit(
                        [&value, &combine, &iop, op1 = bin_opcode, op2 = bin_lhs_opcode](auto &&lhs, auto &&rhs) {
                            using type_l = std::decay_t<decltype(lhs)>;
                            using type_r = std::decay_t<decltype(rhs)>;
                            assert((std::is_same_v<type_l, type_r>));

                            if ((op1 == OP_ADD && op2 == OP_ADD) || (op1 == OP_SUB && op2 == OP_SUB)) {
                                value = lhs + rhs;
                            } else if ((op1 == OP_ADD && op2 == OP_SUB) || (op1 == OP_SUB && op2 == OP_ADD)) {
                                value = lhs - rhs;
                            } else if (op1 == OP_MUL && op2 == OP_MUL) {
                                value = lhs * rhs;
                            } else if (op1 == OP_MUL && op2 == OP_DIV) {
                                if (iop) {
                                    combine = false;
                                } else {
                                    value = lhs / rhs;
                                }
                            } else if (op1 == OP_DIV && op2 == OP_MUL) {
                                if (rhs < lhs) {
                                    combine = false;
                                } else {
                                    value = rhs / lhs;
                                }
                            } else if (op1 == OP_DIV && op2 == OP_DIV) {
                                value = lhs * rhs;
                            } else {
                                combine = false;
                            }
                        },
                        bin_rhs_constant->GetValue(), bin_lhs_rhs_constant->GetValue());

                    if (combine) {
                        auto constant = ConstantAllocator::FindConstantPtr(value);
                        bin_lhs_lhs->RemoveUser(bin_lhs_inst);
                        bin_lhs_lhs->InsertUser(bin_inst);
                        bin_inst->SetLHS(bin_lhs_lhs);
                        bin_inst->SetRHS(constant);
                    }
                }
            } else if (bin_type == LCRV && rhs_inst && rhs_inst->IsTwoOprandInst() && rhs_inst->GetParent() == node) {
                // bin_inst = constant OP variable
                auto &&bin_rhs_inst = std::static_pointer_cast<BinaryInstruction>(rhs_inst);
                auto bin_rhs_opcode = bin_rhs_inst->GetOpCode();
                auto &&bin_rhs_lhs = bin_rhs_inst->GetLHS();
                auto &&bin_rhs_rhs = bin_rhs_inst->GetRHS();
            } else if (bin_type == LCRC) {
                // bin_inst = constant OP constant
                assert(false);
            }  // else nothing to do
        }
    }
}
