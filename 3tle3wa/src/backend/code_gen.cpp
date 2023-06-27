#include "3tle3wa/backend/code_gen.hh"

code_gen::code_gen(CompilationUnit &comp_unit)
    : env_(std::make_unique<asm_env>()), comp_unit_(comp_unit) {
    Assert(fs_, "can not open file %s", path);
}

code_gen::~code_gen() {
    fs_.flush();
    fs_.close();
}

void code_gen::gen_env() {
    auto &&nvmap = comp_unit_.getGlbTable().GetNameValueMap();
    for (auto &&pair : nvmap) {
        if (pair.second->IsGlobalValue()) {
            auto gvp = std::dynamic_pointer_cast<GlobalValue>(pair.second);
            Assert(gvp, "bad dynamic cast");
            env_->make_gvals(gvp, pair.first);
            
        }
    }
    env_->make_prog(comp_unit_.GetNormalFuncTable());
}

std::unique_ptr<asm_env> &code_gen::exports() { return env_; }
