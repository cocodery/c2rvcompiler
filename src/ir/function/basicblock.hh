#pragma once

#include <list>
#include <memory>

#include "instruction.hh"

class BasicBlock;

using InstList = std::list<InstPtr>;
using BlockPtr = std::shared_ptr<BasicBlock>;

class BasicBlock {
   protected:
    size_t idx;
    InstList inst_list;

    static size_t blk_idx;

   public:
    BasicBlock();
    ~BasicBlock() = default;

    size_t GetBlockIdx();

    InstList &GetInstList();

    void InsertInstBack(InstPtr);
    void InsertInstFront(InstPtr);
    void RemoveInst(InstPtr);

    static void ResetBlkIdx();
    static size_t GetBlkIdx();
    static void SetBlkIdx(size_t);

    virtual std::string tollvmIR() = 0;
};
