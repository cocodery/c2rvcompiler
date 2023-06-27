#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <unordered_map>

#include "3tle3wa/backend/asm/inst.hh"
#include "3tle3wa/backend/utils.hh"

class progress;
class glb_value;

// asm basic block progress block
class pblock {
    // 全局块号
    size_t lbidx_;

    // 块内的指令
    std::list<std::unique_ptr<asm_inst>> insts_;

    void opm_rm_needless_ls();
    void opm_rm_needless_li();

   public:
    pblock(size_t lbidx, size_t reserve, progress *father);

    const progress *father_;

    void gen_asm(std::fstream &fs);

    void push(asm_inst *inst);

    void do_optimize();

    std::list<std::unique_ptr<asm_inst>> &ilst();
};

// asm basic block progress
class progress {
    // 过程标签
    std::string label_;

    // 过程内基本块
    std::list<std::unique_ptr<pblock>> pblks_;

    const std::unordered_map<size_t, glb_value *> &gname_map_;

   public:
    progress(std::string label, size_t reserve, const std::unordered_map<size_t, glb_value *> &gname_map);

    void gen_asm(std::fstream &fs);

    void push(std::unique_ptr<pblock> &pblk);

    void do_optimize();

    glb_value *ginfo(size_t gidx) const;

    std::unique_ptr<pblock> &front();
    std::unique_ptr<pblock> &back();
};