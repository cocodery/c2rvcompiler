#include "3tle3wa/pass/interprocedural/dce/dce.hh"

#include <memory>

#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/otherInst.hh"

void DCE::EliminateUselessCode(NormalFuncPtr func) {
    auto allNodes = func->TopoSortFromEntry();

    std::set<InstPtr> visitSet;
    std::queue<InstPtr> WorkList;

    auto Mark = [&visitSet, &allNodes, &WorkList]() {
        for (auto &&node : allNodes) {
            for (auto &&inst : node->GetInstList()) {
                if (inst->IsCriticalOperation()) {
                    visitSet.insert(inst);
                    WorkList.push(inst);
                }
            }
        }
        while (!WorkList.empty()) {
            auto &&front = WorkList.front();
            WorkList.pop();
            for (auto &&value : front->GetOprands()) {
                auto &&inst_def_value = value->GetParent();
                if (inst_def_value == nullptr) continue;
                if (visitSet.find(inst_def_value) == visitSet.end()) {
                    visitSet.insert(inst_def_value);
                    WorkList.push(inst_def_value);
                }
            }
        }
        assert(WorkList.empty());
    };

    auto Sweep = [&visitSet, &allNodes]() {
        for (auto &&node : allNodes) {
            auto &&inst_list = node->GetInstList();
            for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
                auto &&inst = (*iter);
                if (visitSet.find(inst) == visitSet.end()) {
                    RemoveInst(inst);
                    iter = inst_list.erase(iter);
                    continue;
                }
                ++iter;
            }
        }
    };

    Mark();
    Sweep();
}

void DCE::EliminateUselessControlFlow(NormalFuncPtr func) {
    // when processing a node
    // it cannot be a phi parameter
    std::map<CtrlFlowGraphNode *, bool> PhiParamBlock;
    std::map<CfgNodePtr, CfgNodePtr> NodeMap;
    for (auto node : func->TopoSortFromEntry()) {
        NodeMap[node] = node;

        for (auto inst : node->GetInstList()) {
            if (inst->IsPhiInst()) {
                auto phi_inst = std::static_pointer_cast<PhiInst>(inst);
                for (auto [value, block] : phi_inst->GetDataList()) {
                    PhiParamBlock[block.get()] = true;
                }
            }
        }
    }

    [[maybe_unused]] auto FlodRedundantBranch = [&PhiParamBlock](const BaseValue *cond, const CfgNodePtr &node,
                                                                 const CfgNodePtr &iftrue, const CfgNodePtr &iffalse) {
        bool changed = false;
        bool phi_iftrue = PhiParamBlock[iftrue.get()];
        bool phi_iffalse = PhiParamBlock[iffalse.get()];
        if ((iftrue == iffalse) && (phi_iftrue == false)) {
            node->RemoveLastInst();
            node->InsertInstBack(JumpInst::CreatePtr(iftrue, node));
            changed = true;
        } else if (cond->IsConstant()) {
            auto conditon = std::get<bool>(static_cast<const Constant *>(cond)->GetValue());
            if (conditon == true && phi_iftrue == false) {
                node->RemoveLastInst();
                node->InsertInstBack(JumpInst::CreatePtr(iftrue, node));
                node->RmvSuccessor(iffalse);
                iffalse->RmvPredecessor(node);
                changed = true;
            } else if (conditon == false && phi_iffalse == false) {
                node->RemoveLastInst();
                node->InsertInstBack(JumpInst::CreatePtr(iffalse, node));
                node->RmvSuccessor(iftrue);
                iftrue->RmvPredecessor(node);
                changed = true;
            }
        }
        return changed;
    };

    [[maybe_unused]] auto RemoveEmptyBlock = [](const CfgNodePtr &i, const CfgNodePtr &j) {};

    [[maybe_unused]] auto CombineBlocks = [&func](const CfgNodePtr &i, const CfgNodePtr &j) {
        i->RemoveLastInst();
        auto &&i_inst_list = i->GetInstList();
        auto &&j_inst_list = j->GetInstList();
        for_each(i_inst_list.begin(), i_inst_list.end(), [&j](const auto &inst) { inst->SetParent(j); });
        i_inst_list.insert(i_inst_list.end(), j_inst_list.begin(), j_inst_list.end());
        j_inst_list = std::move(i_inst_list);

        j->AppendBlkAttr(i->GetBlockAttr());

        if (j->FindBlkAttr(ENTRY)) {
            func->SetEntryNode(j);
        }

        // ReplacePredSucc(i, j);
        for (auto &&pred : i->GetPredecessors()) {
            pred->AddSuccessor(j);
            j->AddPredecessor(pred);
            pred->GetLastInst()->ReplaceTarget(i, j);
        }
        RemoveNode(i);
    };

    auto HoistBranch = [](CfgNodePtr i, CfgNodePtr j) {
        auto &&j_branch_inst = std::static_pointer_cast<BranchInst>(j->GetLastInst());
        auto &&i_branch_inst = BranchInst::CreatePtr(j_branch_inst->GetCondition(), j_branch_inst->GetTrueTarget(),
                                                     j_branch_inst->GetFalseTarget(), i);
        i->RemoveLastInst();
        i->InsertInstBack(i_branch_inst);
        i->GetSuccessors().remove(j);
        j->GetPredcessors().remove(i);
    };

    auto OnePass = [&](CfgNodeList &post_order) {
        bool changed = false;
        for (auto &&iter = post_order.begin(); iter != post_order.end();) {
            auto i = (*iter);

            // if i act as a phi parameter, cannot be processed
            if (PhiParamBlock[i.get()] == true) {
                ++iter;
                continue;
            }
            auto last_inst = i->GetLastInst();
            if (last_inst->IsBranchInst()) {
                // case 1, fold redundant branch
                auto br_inst = std::static_pointer_cast<BranchInst>(last_inst);
                auto cond = br_inst->GetCondition().get();
                auto iftrue = br_inst->GetTrueTarget();
                auto iffalse = br_inst->GetFalseTarget();

                changed = FlodRedundantBranch(cond, i, iftrue, iffalse);
            }
            last_inst = i->GetLastInst();
            if (last_inst->IsJumpInst()) {
                auto jump_inst = std::static_pointer_cast<JumpInst>(last_inst);
                auto j = jump_inst->GetTarget();

                if (PhiParamBlock[j.get()] == false) {
                    // case 2, remove empty block

                    // case 3, combine i and j
                    if (j->GetPredecessors().size() == 1) {
                        CombineBlocks(i, j);
                        NodeMap[i] = j;
                        changed = true;
                        iter = post_order.erase(iter);
                        continue;
                    }
                    // case 4, hoist a branch
                }
            }
            ++iter;
        }
        return changed;
    };

    while (true) {
        auto &&post_order = func->TopoSortFromExit();
        if (!OnePass(post_order)) break;
    }

    // modify origin-alloca parent which been merged
    for (auto &&node : func->TopoSortFromEntry()) {
        for (auto &&inst : node->GetInstList()) {
            if (inst->IsPhiInst()) {
                auto &&phi_inst = std::static_pointer_cast<PhiInst>(inst);
                auto &&origin_alloca = phi_inst->GetOriginAlloca();

                origin_alloca->SetParent(NodeMap[origin_alloca->GetParent()]);
            } else {
                break;
            }
        }
    }
}

void DCE::EliminateUnreachableCode(NormalFuncPtr func) {
    auto exit = func->GetExitNode();
    auto allNodes = func->TopoSortFromEntry();
    // Solve the control flow graph from exit
    // Identify the unreachable nodes in the control flow
    std::queue<CfgNodePtr> nodeQueue;
    std::unordered_map<CfgNodePtr, bool> visitMap;
    std::set<CfgNodePtr> delNodeSet;
    nodeQueue.push(exit);
    while (!nodeQueue.empty()) {
        auto &&front = nodeQueue.front();
        nodeQueue.pop();
        if (visitMap[front] == false) {
            visitMap[front] = true;
            for (auto &&node : front->GetPredecessors()) {
                nodeQueue.push(node);
                if (std::find(allNodes.begin(), allNodes.end(), node) == allNodes.end()) {
                    delNodeSet.insert(node);
                }
            }
        }
    }
    assert(nodeQueue.empty());

    // remove dead-node
    std::for_each(delNodeSet.begin(), delNodeSet.end(), RemoveNode);
}

void DCE::DCE(NormalFuncPtr func) {
    EliminateUselessCode(func);
    EliminateUselessControlFlow(func);
    EliminateUnreachableCode(func);
}
