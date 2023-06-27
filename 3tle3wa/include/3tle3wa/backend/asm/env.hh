#pragma once

#include <list>
#include <memory>
#include <thread>
#include <unordered_set>

#include "3tle3wa/backend/asm/glb_value.hh"
#include "3tle3wa/backend/asm/progress.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/utils/Logs.hh"

class asm_env {
    std::unordered_set<uint32_t> lc_pool_;

    std::list<std::unique_ptr<glb_value>> gvals_;

    std::list<std::unique_ptr<progress>> pgrs_;

    std::unordered_map<size_t, glb_value *> gname_map_;

   public:
    void make_gvals(GlobalValuePtr &gvptr, const std::string &name);
    void make_prog(NormalFuncList &flst);

    void do_optimize();

    void gen_asm(std::fstream &fs);
};