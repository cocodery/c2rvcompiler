#include "3tle3wa/pass/intraprocedural/gvl/gvl.hh"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <queue>

#include "3tle3wa/ir/function/function.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/memoryInst.hh"
#include "3tle3wa/ir/value/constant.hh"
#include "3tle3wa/ir/value/globalvalue.hh"
#include "3tle3wa/ir/value/type/baseType.hh"
#include "3tle3wa/ir/value/type/scalarType.hh"
#include "3tle3wa/ir/value/use.hh"
#include "3tle3wa/ir/value/variable.hh"

void GVL::GlbValueLocalization(CompilationUnit &comp_unit) {
    auto &&glb_table = comp_unit.getGlbTable().GetNameValueMap();

    // remove useless global-value
    // for (auto &&iter = glb_table.begin(); iter != glb_table.end();) {
    //     auto [_, value] = (*iter);

    //     if (value->IsGlobalValue()) {
    //         auto &&glb_value = std::static_pointer_cast<GlobalValue>(value);

    //         if (glb_value->IsBeenUsed() == false) {
    //             iter = glb_table.erase(iter);
    //             continue;
    //         }
    //     }
    //     ++iter;
    // }

    // localization global-value
    for (auto &&iter = glb_table.begin(); iter != glb_table.end();) {
        auto &&[_, value] = (*iter);

        if (value->IsGlobalValue()) {
            auto &&glb_value = std::static_pointer_cast<GlobalValue>(value);

            auto &&define_in = glb_value->GetDefineIn();
            auto &&used_in = glb_value->GetUsedIn();

            if (define_in.size() == 0) {  // only initilize no more assign
            } else if (define_in.size() == 1 && used_in.size() == 1) {
                auto &&define_func = (*define_in.begin());
                auto &&used_func = (*used_in.begin());
                assert(!define_func->IsLibFunction() && !used_func->IsLibFunction());

                // only defined and used in main can be localization
                if (define_func == used_func && define_func->GetFuncName() == "main") {
                    auto &&normal_func = static_cast<NormalFunction *>(define_func);

                    BaseTypePtr base_type = glb_value->GetBaseType();
                    if (base_type->IsArray() == false) {
                        Variable::SetVarIdx(normal_func->GetVarIdx());
                        BasicBlock::SetBlkIdx(normal_func->GetBlkIdx());

                        bool int_type = base_type->IntType();
                        auto &&entry = normal_func->GetEntryNode();

                        auto &&type_stored = int_type ? type_int_L : type_float_L;
                        auto &&type_alloca = int_type ? type_int_ptr_L : type_float_ptr_L;

                        VariablePtr address = Variable::CreatePtr(type_alloca, nullptr);
                        auto &&alloca_inst = AllocaInst::CreatePtr(type_stored, address, entry);
                        address->SetParent(alloca_inst);

                        auto &&init_value = glb_value->GetInitValue();
                        if (init_value->IsUnInitVar()) {
                            init_value = int_type ? ConstantAllocator::FindConstantPtr(static_cast<int32_t>(0))
                                                  : ConstantAllocator::FindConstantPtr(static_cast<float>(0));
                        }

                        auto &&store_inst = StoreInst::CreatePtr(address, init_value, entry);

                        entry->InsertInstFront(store_inst);
                        entry->InsertInstFront(alloca_inst);

                        ReplaceSRC(glb_value, address);

                        for (auto &&inst : address->GetUserList()) {
                            if (inst->IsGepInst()) {
                                ReplaceSRC(inst->GetResult(), address);
                            }
                        }

                        normal_func->SetVarIdx(Variable::GetVarIdx());
                        normal_func->SetBlkIdx(BasicBlock::GetBlkIdx());

                        iter = glb_table.erase(iter);
                        continue;
                    }
                }
            }
        }
        ++iter;
    }
}
