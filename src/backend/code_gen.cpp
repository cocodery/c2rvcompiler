#include "code_gen.hh"

#include "Logs.hh"

code_gen::code_gen(const char *path, CompilationUnit &comp_unit)
    : env_(), fs_(path, std::ios::out), comp_unit_(comp_unit) {
    Assert(fs_, "can not open file %s", path);

    std::fstream fs("dbg.ir.s", std::ios::out);
    fs << "# for ir debug" << std::endl;
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
            env_.make_gvals(gvp);
        }
    }
    env_.make_prog(comp_unit_.GetNormalFuncTable());
}

void code_gen::gen_asm() { env_.gen_asm(fs_); }