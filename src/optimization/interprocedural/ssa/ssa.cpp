#include "ssa.hh"

StaticSingleAssignment::RenameData::RenameData(CfgNodePtr _node, CfgNodePtr _pred, ValueVector _v)
    : node(_node), pred(_pred), valuelist(_v) {}

StaticSingleAssignment::RenameDatePtr StaticSingleAssignment::RenameData::CreatePtr(CfgNodePtr _node, CfgNodePtr _pred,
                                                                                    ValueVector _valuelist) {
    return std::make_shared<RenameData>(_node, _pred, _valuelist);
}

void StaticSingleAssignment::SSAConstruction(CfgNodePtr entry, CfgNodeList allNodes, size_t var_idx) {
    DominanceAnalysis::ComputeDominanceInfo(entry, allNodes);
    DominanceAnalysis::ComputeDominanceFrontier(allNodes);

    std::vector<std::set<CfgNodePtr>> defBlocks;
    std::list<AllocaInstPtr> allocaInsts;
    std::unordered_map<AllocaInstPtr, int32_t> allocaLookUp;
    std::unordered_map<PhiInstPtr, int32_t> phi2AllocaMap;

    for (auto &&node : allNodes) {
        for (auto &&inst : node->GetInstList()) {
            if (inst->IsAllocaInst()) {
                auto &&alloca_inst = std::static_pointer_cast<AllocaInst>(inst);
                if (alloca_inst->GetAllocaType()->IsScalar()) {
                    defBlocks.push_back(std::set<CfgNodePtr>());
                    allocaInsts.push_back(alloca_inst);
                    allocaLookUp[alloca_inst] = allocaInsts.size() - 1;
                }
            }
        }
    }
    for (auto &&node : allNodes) {
        for (auto &&inst : node->GetInstList()) {
            if (inst->IsStoreInst()) {
                auto &&store_inst = std::static_pointer_cast<StoreInst>(inst);
                auto &&inst_gen_store_addr = store_inst->GetStoreAddr()->GetParent();
                if (inst_gen_store_addr && inst_gen_store_addr->IsAllocaInst()) {
                    auto &&alloca_inst = std::static_pointer_cast<AllocaInst>(inst_gen_store_addr);
                    if (allocaLookUp.find(alloca_inst) != allocaLookUp.end()) {
                        defBlocks[allocaLookUp[alloca_inst]].insert(node);
                    }
                }
            }
        }
    }

    auto &&inDefBlocks = [&defBlocks](int32_t index, CfgNodePtr node) {
        auto &&blockSet = defBlocks[index];
        if (blockSet.find(node) != blockSet.end()) {
            return true;
        }
        return false;
    };

    Variable::SetVarIdx(var_idx);
    std::queue<CfgNodePtr> W;
    for (auto &&alloca_inst : allocaInsts) {
        int32_t index = allocaLookUp[alloca_inst];
        BaseTypePtr type = alloca_inst->GetAllocaType();
        std::for_each(allNodes.begin(), allNodes.end(), [](const auto &node) { node->SetDirty(false); });
        std::for_each(defBlocks[index].begin(), defBlocks[index].end(), [&W](const auto &node) { W.push(node); });
        while (!W.empty()) {
            CfgNodePtr front = W.front();
            W.pop();
            for (auto &&df : front->GetDomFrontier()) {
                if (!df->GetDirty()) {
                    df->SetDirty(true);
                    auto &&phi_inst = PhiInst::CreatePtr(type, df);
                    phi2AllocaMap[phi_inst] = index;
                    if (!inDefBlocks(index, df)) {
                        W.push(df);
                    }
                }
            }
        }
    }

    ValueVector valuelist;
    std::for_each(allocaInsts.begin(), allocaInsts.end(), [&valuelist](const auto &inst) {
        valuelist.push_back(inst->GetAllocaType()->IntType() ? zero_int32 : zero_float);
    });
    std::for_each(allNodes.begin(), allNodes.end(), [](const auto &node) { node->SetDirty(false); });

    std::stack<RenameDatePtr> renameDateStack;
    renameDateStack.push(RenameData::CreatePtr(entry, nullptr, valuelist));
    while (!renameDateStack.empty()) {
        RenameDatePtr top = renameDateStack.top();
        renameDateStack.pop();
        ValueVector cur_valuelist = top->valuelist;

        for (auto &&inst : top->node->GetInstList()) {
            if (inst->IsPhiInst()) {
                auto &&phi_inst = std::static_pointer_cast<PhiInst>(inst);
                if (phi2AllocaMap.find(phi_inst) != phi2AllocaMap.end()) {
                    PhiInst::InsertPhiData(phi_inst, top->valuelist[phi2AllocaMap[phi_inst]], top->pred);
                }
            }
        }

        if (top->node->GetDirty()) continue;
        top->node->SetDirty(true);

        auto &&inst_list = top->node->GetInstList();
        for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
            auto &&inst = (*iter);
            if (inst->IsAllocaInst()) {
                auto &&alloca_inst = std::static_pointer_cast<AllocaInst>(inst);
                if (allocaLookUp.find(alloca_inst) != allocaLookUp.end()) {
                    iter = inst_list.erase(iter);
                    continue;
                }
            } else if (inst->IsLoadInst()) {
                auto &&load_inst = std::static_pointer_cast<LoadInst>(inst);
                auto &&inst_gen_load_addr = load_inst->GetOprand()->GetParent();
                if (inst_gen_load_addr && inst_gen_load_addr->IsAllocaInst()) {
                    auto &&alloca_inst = std::static_pointer_cast<AllocaInst>(inst_gen_load_addr);
                    int32_t allocaIndex = allocaLookUp[alloca_inst];
                    auto &&load_result = load_inst->GetResult();
                    ReplaceSRC(load_inst->GetResult(), cur_valuelist[allocaIndex]);
                    RemoveInst(load_inst);
                    iter = inst_list.erase(iter);
                    continue;
                }
            } else if (inst->IsStoreInst()) {
                auto &&store_inst = std::static_pointer_cast<StoreInst>(inst);
                auto &&inst_gen_store_addr = store_inst->GetStoreAddr()->GetParent();
                if (inst_gen_store_addr && inst_gen_store_addr->IsAllocaInst()) {
                    auto &&alloca_inst = std::static_pointer_cast<AllocaInst>(inst_gen_store_addr);
                    int32_t allocaIndex = allocaLookUp[alloca_inst];
                    cur_valuelist[allocaIndex] = store_inst->GetStoreValue();
                    RemoveInst(store_inst);
                    iter = inst_list.erase(iter);
                    continue;
                }
            } else if (inst->IsPhiInst()) {
                auto &&phi_inst = std::static_pointer_cast<PhiInst>(inst);
                int allocaIndex = phi2AllocaMap[phi_inst];
                cur_valuelist[allocaIndex] = phi_inst->GetResult();
            }
            ++iter;
        }

        for (auto &&succ : top->node->GetSuccessors()) {
            renameDateStack.push(RenameData::CreatePtr(succ, top->node, cur_valuelist));
        }
    }
}
