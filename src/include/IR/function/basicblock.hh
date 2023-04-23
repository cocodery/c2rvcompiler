#pragma once

#include <list>
#include <memory>

#include "../instruction/instruction.hh"

class BasicBlock;

using InstListType = std::list<InstPtr>;
using BlockPtr = std::shared_ptr<BasicBlock>;

class BasicBlock {
   protected:
    size_t idx;
    InstListType inst_list;

    static size_t blk_idx;

   public:
    BasicBlock();
    ~BasicBlock() = default;

    size_t getBlockIdx();

    InstListType &getInstList();

    void insertInst(InstPtr);

    static void resetBlkIdx();

    virtual std::string tollvmIR() = 0;
};
