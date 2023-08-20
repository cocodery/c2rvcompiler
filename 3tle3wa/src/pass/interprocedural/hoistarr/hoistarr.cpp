#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <queue>
#include <unordered_map>
#include <variant>

#include "3tle3wa/ir/function/basicblock.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/memoryInst.hh"
#include "3tle3wa/ir/instruction/otherInst.hh"
#include "3tle3wa/ir/value/baseValue.hh"
#include "3tle3wa/ir/value/constant.hh"
#include "3tle3wa/ir/value/constarray.hh"
#include "3tle3wa/ir/value/globalvalue.hh"
#include "3tle3wa/ir/value/type/baseType.hh"
#include "3tle3wa/ir/value/type/listType.hh"
#include "3tle3wa/ir/value/use.hh"
#include "3tle3wa/pass/interprocedural/hositarr/hositarr.hh"

void HoistLocalArray::HoistLocalArray(NormalFuncPtr &func, SymbolTable &glb_table) {
    std::map<CtrlFlowGraphNode *, InstList> del_lists;
    std::map<BaseValuePtr, BaseValuePtr> replace;
    // find const local array
    for (auto &&node : func->GetSequentialNodes()) {
        // assume a array which can hoist is
        // mannualy initialized after allcate
        // and no more assignment, each slot assign only one time
        std::unordered_map<Instruction *, bool> visit;
        for (auto &&inst : node->GetInstList()) {
            if (inst->IsAllocaInst()) {
                bool can_hoist = true;  // assume can hoist for each alloca
                InstList inst_list;

                auto &&alloca = std::static_pointer_cast<AllocaInst>(inst);
                auto &&address = alloca->GetResult();
                auto list_type = std::static_pointer_cast<ListType>(alloca->GetAllocaType());

                auto &&zero = list_type->IntType() ? ConstantAllocator::FindConstantPtr(static_cast<int32_t>(0))
                                                   : ConstantAllocator::FindConstantPtr(static_cast<float>(0));

                ConstArr const_array;
                for (size_t idx = 0, size = list_type->GetArrSize(); idx < size; ++idx) {
                    const_array.push_back(zero);
                }
                while (const_array.size() % ARR_SIZE_ALIGN != 0) {
                    const_array.push_back(zero);
                }

                // each offset store one time at most
                // mannully store to constant offset
                for (auto &&user1 : address->GetUserList()) {
                    if (user1->IsGepInst()) {
                        auto &&gep = std::static_pointer_cast<GetElementPtrInst>(user1);
                        auto &&off = gep->GetOffList().back();
                        // get all related gep
                        std::list<GepInstPtr> gep_list;
                        std::queue<GepInstPtr> queue;
                        queue.push(gep);
                        while (!queue.empty()) {
                            gep_list.push_back(gep);
                            for (auto &&user : gep->GetResult()->GetUserList()) {
                                if (user->IsGepInst()) {
                                    queue.push(std::static_pointer_cast<GetElementPtrInst>(user));
                                }
                            }
                        }
                        // check store out of `node`
                        for (auto &&gep : gep_list) {
                            for (auto &&user : gep->GetResult()->GetUserList()) {
                                if (user->IsStoreInst() && user->GetParent() != node) {
                                    can_hoist = false;
                                }
                            }
                        }
                        if (can_hoist == false) {
                            break;
                        }
                        for (auto &&gep : gep_list) {
                            for (auto &&user : gep->GetResult()->GetUserList()) {
                                if (user->IsStoreInst()) {  // store to unknown offset
                                    can_hoist = false;
                                    break;
                                } else if (user->IsCallInst()) {
                                    auto &&callee = std::static_pointer_cast<CallInst>(user)->GetCalleeFunc();
                                    if (callee->GetSideEffect()) {  // used by func with side-effect
                                        can_hoist = false;
                                        break;
                                    }
                                }
                            }
                        }

                        if (off->IsVariable()) {
                            for (auto &&user2 : gep->GetResult()->GetUserList()) {
                                if (user2->IsStoreInst()) {  // store to unknown offset
                                    can_hoist = false;
                                    break;
                                } else if (user2->IsCallInst()) {
                                    auto &&callee = std::static_pointer_cast<CallInst>(user2)->GetCalleeFunc();
                                    if (callee->GetSideEffect()) {  // used by func with side-effect
                                        can_hoist = false;
                                        break;
                                    }
                                }
                            }
                        } else if (off->IsConstant()) {
                            size_t store_cnt = 0;
                            StoreInstPtr store_inst = nullptr;
                            int32_t offset = std::get<int32_t>(std::static_pointer_cast<Constant>(off)->GetValue());

                            for (auto &&user2 : gep->GetResult()->GetUserList()) {
                                if (user2->IsStoreInst()) {
                                    store_inst = std::static_pointer_cast<StoreInst>(user2);
                                    store_cnt += 1;
                                } else if (user2->IsBitCast()) {
                                    inst_list.push_back(user2);  // bitcast
                                    assert(user2->GetResult()->GetUserList().size() == 1);
                                    inst_list.push_back(*user2->GetResult()->GetUserList().begin());  // memset
                                } else if (user2->IsCallInst()) {
                                    auto &&callee = std::static_pointer_cast<CallInst>(user2)->GetCalleeFunc();
                                    if (callee->GetSideEffect()) {  // used by func with side-effect
                                        can_hoist = false;
                                        break;
                                    }
                                }
                            }
                            if (store_cnt > 1) {
                                can_hoist = false;
                            } else if (store_inst != nullptr) {
                                if (store_inst->GetParent() != node) {
                                    can_hoist = false;
                                    break;
                                }
                                auto &&store_value = store_inst->GetStoreValue();
                                if (store_value->IsVariable()) {
                                    can_hoist = false;
                                } else if (store_value->IsConstant()) {
                                    inst_list.push_back(store_inst);
                                    const_array[offset] = std::static_pointer_cast<Constant>(store_value);
                                } else {
                                    assert(false);
                                }
                            }
                        } else {
                            assert(false);
                        }
                        if (can_hoist == false) {
                            break;
                        }
                    } else {
                        assert(false);
                    }
                }
                if (can_hoist) {
                    auto &&corr_del_list = del_lists[node.get()];
                    corr_del_list.push_back(alloca);
                    corr_del_list.insert(corr_del_list.end(), inst_list.begin(), inst_list.end());

                    auto arr_dim = list_type->GetArrDims();
                    auto &&type = ListType::CreatePtr((list_type->IntType() ? INT32 : FLOAT), IMMUTABLE, POINTER, ARRAY,
                                                      GLOBAL, arr_dim);
                    auto &&const_arr = ConstArray::CreatePtr(list_type, const_array);
                    GlobalValuePtr glb_value = GlobalValue::CreatePtr(type, const_arr);
                    std::string name = "array.hoist." + func->GetFuncName() + glb_value->tollvmIR();
                    glb_table.InsertSymbol(name, glb_value);
                    glb_value->InsertUser(func.get());

                    replace[alloca->GetResult()] = glb_value;  // later replace alloca-result with global-value
                }
            }
        }
    }
    if (replace.size() > 0) {
        for (auto &&node : func->GetSequentialNodes()) {
            for (auto &&inst : node->GetInstList()) {
                if (inst->IsGepInst()) {
                    auto &&gep = std::static_pointer_cast<GetElementPtrInst>(inst);
                    auto &&ori_base_addr = gep->GetBaseAddr();
                    if (replace.count(ori_base_addr)) {
                        auto &&new_base_addr = replace[ori_base_addr];

                        ori_base_addr->RemoveUser(gep);
                        gep->SetBaseAddr(new_base_addr);
                        new_base_addr->InsertUser(gep);
                    }
                }
            }
        }
    }
    for (auto &&[node, del_list] : del_lists) {
        auto &&inst_list = node->GetInstList();
        for (auto &&inst : del_list) {
            assert(std::find(inst_list.begin(), inst_list.end(), inst) != inst_list.end());
            RemoveInst(inst);
            inst_list.remove(inst);
        }
    }
    return;
}