#pragma once

#include <list>
#include <memory>

#include "instruction.hh"

class BasicBlock;

using InstList = std::list<InstPtr>;
using BlockPtr = std::shared_ptr<BasicBlock>;

enum BlockAttr {
    NORMAL,
    ENTRY,
    LOOPBEGIN,
    LOOPEND,
    LOOPOUT,
    BREAK,
    CONTINUE,
    GORETURN,
    EXIT,
};

std::string AttrToStr(BlockAttr);

class BasicBlock {
   protected:
    size_t idx;
    InstList inst_list;

    BlockAttr block_attr;

    static size_t blk_idx;

   public:
    BasicBlock(BlockAttr);
    ~BasicBlock() = default;

    size_t GetBlockIdx();

    InstList &GetInstList();

    size_t GetInstCnt() const;

    InstPtr &GetLastInst();

    void InsertInstBack(InstPtr);
    void InsertInstFront(InstPtr);
    void RemoveInst(InstPtr);

    void SetBlockAttr(BlockAttr);
    BlockAttr GetBlockAttr() const;

    static void ResetBlkIdx();
    static size_t GetBlkIdx();
    static void SetBlkIdx(size_t);

    virtual std::string tollvmIR() = 0;
};
