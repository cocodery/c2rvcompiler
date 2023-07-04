#include <thread>

#include "3tle3wa/backend/AsmGen.hh"
#include "3tle3wa/backend/AsmGlobalValue.hh"
#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/BackendIRGen.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/utils/Logs.hh"

RLGen::RLGen(CompilationUnit &comp_unit) : asm_gen_(std::make_unique<AsmGen>()) {}

RLGen::~RLGen() {}

void RLGen::Register(CompilationUnit &comp_unit_) {
    auto &&nvmap = comp_unit_.getGlbTable().GetNameValueMap();
    for (auto &&[name, value] : nvmap) {
        if (value->IsGlobalValue()) {
            auto gvp = dynamic_cast<GlobalValue *>(value.get());
            Assert(gvp, "bad dynamic cast");
            registerGlobalValue(gvp, name);
        }
    }

    registerNormalFunction(comp_unit_.GetNormalFuncTable());
}

std::unique_ptr<AsmGen> &RLGen::ExportAsmGen() { return asm_gen_; }

void RLGen::registerGlobalValue(GlobalValue *gvp, const std::string &name) {
    auto &&init_val_ptr = gvp->GetInitValue().get();

    if (init_val_ptr->IsUnInitVar()) {
        auto &&uninit_val_ptr = dynamic_cast<UnInitVar *>(init_val_ptr);
        Assert(uninit_val_ptr, "bad dynamic cast");

        if (uninit_val_ptr->GetBaseType()->IsArray()) {
            auto &&uninit_array_type_ptr = dynamic_cast<ListType *>(uninit_val_ptr->GetBaseType().get());
            Assert(uninit_array_type_ptr, "bad dynamic cast");

            auto agv = std::make_unique<AsmGlobalValue>(name, uninit_array_type_ptr->GetCapacity() * 4, true, 0);

            auto result = gv_map_.emplace(gvp->GetGlobalValueIdx(), agv.get());
            Assert(result.second, "insert global variable fail!");

            asm_gen_->PushAsmGlobalValue(agv);
            return;
        }

        auto agv = std::make_unique<AsmGlobalValue>(name, 4, true, 0);

        auto result = gv_map_.emplace(gvp->GetGlobalValueIdx(), agv.get());
        Assert(result.second, "insert global variable fail!");

        asm_gen_->PushAsmGlobalValue(agv);
        return;
    }

    if (init_val_ptr->GetBaseType()->IsArray()) {
        auto init_array_ptr = dynamic_cast<ConstArray *>(init_val_ptr);
        Assert(init_array_ptr, "bad dynamic cast");

        auto init_array_type_ptr = dynamic_cast<ListType *>(init_array_ptr->GetBaseType().get());
        Assert(init_array_type_ptr, "bad dynamic cast");

        auto capacity = init_array_type_ptr->GetCapacity();
        auto agv = std::make_unique<AsmGlobalValue>(name, capacity * 4, false, capacity);
        
        // extract const array
        auto push_init_value = [init_array_ptr, &agv]() {
            auto &&cstarr = init_array_ptr->GetConstArr();

            for (auto &&v : cstarr) {
                auto cinfo = XConstValue(v->GetValue());

                // no need to check for now
                agv->Push(cinfo.v32_.u32_);
            }
        };
        push_init_value();

        auto result = gv_map_.emplace(gvp->GetGlobalValueIdx(), agv.get());
        Assert(result.second, "insert global variable fail!");

        asm_gen_->PushAsmGlobalValue(agv);
        return;
    }

    auto init_val = dynamic_cast<Constant *>(init_val_ptr);
    auto cinfo = XConstValue(init_val->GetValue());
    auto agv = std::make_unique<AsmGlobalValue>(name, 4, false, 1);
    agv->Push(cinfo.v32_.u32_);

    auto result = gv_map_.emplace(gvp->GetGlobalValueIdx(), agv.get());
    Assert(result.second, "insert global variable fail!");

    asm_gen_->PushAsmGlobalValue(agv);
}

void RLGen::registerNormalFunction(NormalFuncList &nflst) {
    for (auto &&nf: nflst) {
        auto itunit = std::make_unique<InternalTranslation>(nf, lc_map_, gv_map_);
        translation_tasks_.push_back(std::move(itunit));
    }
}

void RLGen::SerialGenerate() {
    for (auto &&task: translation_tasks_) {
        task->DoTranslation();
    }
}

void RLGen::ParallelGenerate() {
    std::vector<std::unique_ptr<std::thread>> trds;

    for (auto &&task: translation_tasks_) {
        auto trd = std::make_unique<std::thread>([&task]() {
            task->DoTranslation();
        });
        trds.push_back(std::move(trd));
    }

    for (auto &&trd : trds) {
        trd->join();
    }
}