#pragma once

#include <list>
#include <memory>
#include <unordered_set>

#include "IR.hh"
#include "glb_value.hh"
#include "progress.hh"

class asm_env {
    std::unordered_set<uint32_t> lc_pool_;

    std::list<std::unique_ptr<glb_value>> gvals_;

    std::list<std::unique_ptr<progress>> pgrs_;

   public:
    void make_gvals(GlobalValuePtr &gvptr);
    void make_prog(NormalFuncList &flst);

    void gen_asm(std::fstream &fs);
};