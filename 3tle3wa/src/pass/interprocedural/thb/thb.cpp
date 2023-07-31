#include "3tle3wa/pass/interprocedural/thb/thb.h"


void THB::THB(const NormalFuncPtr &func) {
    auto allNodes = func->TopoSortFromEntry();
    std::vector<InstPtr> addInsts;
    std::map<BaseValuePtr, int, cmp> operands;
    BaseValuePtr lastDef = nullptr;

    Log("tree height balance");


    for (auto &&node: allNodes) {
//        for (auto &&inst: node->GetInstList()) {

        int cnt = 0;
        for (auto &&inst_itr = node->GetInstList().begin(); inst_itr != node->GetInstList().end(); ++inst_itr) {
            bool flag = false;
            cnt++;
//            Log("inst #%d/%zu: %s", cnt, node->GetInstList().size(), (*inst_itr)->tollvmIR().c_str());
            auto inst = *inst_itr;
            if (inst->GetOpCode() == OpCode::OP_ADD || inst->GetOpCode() == OpCode::OP_SUB) {
//                Log("flag1");
                auto bin_inst = static_cast<BinaryInstruction *>(inst.get());
                if (bin_inst->GetLHS()->IsVariable()) continue;
                if (!bin_inst->GetLHS()->GetBaseType()->IntType() || !bin_inst->GetRHS()->GetBaseType()->IntType())
                    continue;
                auto tmp = inst_itr;
                auto next = *(tmp++);
                auto bin_next = static_cast<BinaryInstruction *>(next.get());
                auto def = bin_inst->GetLHS();
//                Log("flag2");
                if (!lastDef) {
                    if ((next->GetOpCode() == OpCode::OP_ADD || next->GetOpCode() == OpCode::OP_SUB) &&
                        (bin_next->GetLHS() == def)) {
                        lastDef = def;
                        operands[bin_inst->GetLHS()]++;
                        if (bin_inst->GetOpCode() == OpCode::OP_ADD) {
                            operands[bin_inst->GetRHS()]++;
                        } else {
                            operands[bin_inst->GetRHS()]--;
                        }
                        addInsts.push_back(static_cast<InstPtr>(bin_inst));
//                        Log("flag3.1");
                    }
                } else {
                    if (bin_inst->GetLHS() == lastDef && bin_inst->GetLHS()->GetUserList().size() == 1) {
                        addInsts.push_back(static_cast<InstPtr>(bin_inst));
                        lastDef = bin_inst->GetLHS();
                        if (bin_inst->GetOpCode() == OpCode::OP_ADD) {
                            operands[bin_inst->GetRHS()]++;
                        } else {
                            operands[bin_inst->GetRHS()]--;
                        }
                    } else {
                        flag = true;
                    }
//                    Log("flag3.2");
                }
//                Log("flag4");
            } else if (lastDef) {
                flag = true;
            }
//            Log("flag5, flag: %d", flag);
            if (flag) {
//                Log("before convert");
                convert(operands, addInsts);
//                Log("after convert");
                if (!operands.empty()) operands.clear();
                if (!addInsts.empty()) {
//                    Log("addInsts clear");
                    for (const auto& it: addInsts) {
//                        Log("addInsts clear: %s", it->tollvmIR().c_str());
                    }
//                    Log("addInsts log end, addInst size: %zu", addInsts.size());
                    addInsts.clear();
//                    std::vector<Instruction *>().swap(addInsts);
//                    Log("addInsts clear end");
                }
                lastDef = nullptr;
//                Log("thb convert end");
            }
        }
    }
}

void THB::convert(std::map<BaseValuePtr, int, cmp> operands, std::vector<InstPtr> addInsts) {
//    Log("convert start");
    auto lastInst = *(addInsts.rbegin());
    auto lastBinInst = static_cast<BinaryInstruction *>(lastInst.get());
    auto oldRes = lastBinInst->GetLHS();
    auto type = oldRes->GetBaseType();
    std::vector<InstPtr> newInsts;
    std::vector<BaseValuePtr> addDests;
    std::vector<BaseValuePtr> subDests;
    auto block = addInsts[0]->GetParent();
    double val = 0;
    for (auto it: operands) {
        if (it.first->IsConstant()) {
//            Log("constant");
            auto constant = static_cast<Constant *>(it.first.get());
            const int *i = std::get_if<int>(&constant->GetValue());
            if (i != nullptr) {
                val += *i * it.second;
            }
        } else if (it.second != 1 && it.second != -1) {
//            Log("not constant");
            auto src1 = it.first;
            auto num = abs(it.second);
            auto src2_t = new Constant(type_const_int, num);
            std::shared_ptr<BaseValue> src2(src2_t);
            if (type->FloatType()) {
                // ?
            }
            auto dest = new Variable(type_int_L, nullptr);
            auto inst = new IBinaryInst(std::shared_ptr<Variable>(dest), OpCode::OP_MUL, src1, src2, block);
            auto inst_ptr = static_cast<InstPtr>(inst);
            dest->SetParent(inst_ptr);
            newInsts.push_back(inst_ptr);
            if (it.second > 0) {
                addDests.push_back(std::shared_ptr<BaseValue>(dest));
            } else {
                subDests.push_back(std::shared_ptr<BaseValue>(dest));
            }
        } else if (it.second == 1) {
            addDests.push_back(it.first);
        } else if (it.second == -1) {
            subDests.push_back(it.first);
        }
    }
    if (val != 0) {
//        Log("val != 0");
        auto imm = new Constant(type_const_int, float(val));
        addDests.push_back(static_cast<BaseValuePtr>(imm));
    }
    BaseValuePtr res;
    if (!addDests.empty()) {
//        Log("addDests not empty");
        if (addDests.size() == 1) {
//            Log("addDests size == 1");
            res = addDests[0];
        } else {
//            Log("addDests size > 1");
            auto res_var = new Variable(type_int_L, nullptr);
            res = static_cast<BaseValuePtr>(res_var);
            auto inst = new IBinaryInst(std::shared_ptr<Variable>(res_var), OpCode::OP_ADD, addDests[0], addDests[1],
                                        block);
            newInsts.push_back(static_cast<InstPtr>(inst));
            for (auto it = addDests.begin() + 2; it != addDests.end(); ++it) {
                auto newRes = new Variable(type_int_L, nullptr);
                auto in = new IBinaryInst(std::shared_ptr<Variable>(newRes), OpCode::OP_ADD, res, *it, block);
                newInsts.push_back(static_cast<InstPtr>(in));
                res = static_cast<BaseValuePtr>(newRes);
            }
        }
    } else {
        auto res_const = new Constant(type_const_int, 0);
        res = static_cast<BaseValuePtr>(res_const);
    }
    if (!subDests.empty()) {
//        Log("subDests not empty");
        for (auto &subDest: subDests) {
            auto newRes = new Variable(type_int_L, nullptr);
            auto in = new IBinaryInst(std::shared_ptr<Variable>(newRes), OpCode::OP_SUB, res, subDest, block);
            newInsts.push_back(static_cast<InstPtr>(in));
            res = static_cast<BaseValuePtr>(newRes);
        }
    }
//    Log("replace");
    while (!oldRes->GetUserList().empty())
        oldRes->GetUserList().begin()->get()->ReplaceSRC(oldRes, res);
    auto inst_list = addInsts[0]->GetParent()->GetInstList();
//    Log("inst_list size: %zu", inst_list.size());
//    auto prevInst = addInsts[0]->GetInstIdx();
    auto first_it = std::find(inst_list.begin(), inst_list.end(), static_cast<InstPtr>(addInsts[0]));
//    Log("first_it: %s", (*first_it)->tollvmIR().c_str());
    first_it++;
    inst_list.insert(first_it, newInsts.begin(), newInsts.end());
//    Log("end");
}