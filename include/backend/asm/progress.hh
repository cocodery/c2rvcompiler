#pragma once

#include <fstream>
#include <list>
#include <memory>

#include "backend/utils.hh"
#include "inst.hh"

// asm basic block progress block
class pblock {
    // 全局块号
    size_t lbidx_;

    // 块内的指令
    std::list<std::unique_ptr<asm_inst>> insts_;

   public:
    pblock(size_t lbidx, size_t reserve);

    void gen_asm(std::fstream &fs);

    void push(asm_inst *inst);

    std::list<std::unique_ptr<asm_inst>> &ilst();
};

// asm basic block progress
class progress {
    // 过程标签
    std::string label_;

    // 过程内基本块
    std::list<std::unique_ptr<pblock>> pblks_;

   public:
    progress(std::string label, size_t reserve);

    void gen_asm(std::fstream &fs);

    void push(std::unique_ptr<pblock> &pblk);

    std::unique_ptr<pblock> &front();
    std::unique_ptr<pblock> &back();
};