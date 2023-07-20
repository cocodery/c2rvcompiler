#pragma once

#include <list>
#include <memory>

#include "3tle3wa/ir/instruction/instruction.hh"

class BasicBlock;

using InstList = std::list<InstPtr>;
using BlockPtr = std::shared_ptr<BasicBlock>;

using BlockAttr = size_t;

class BaseFunction;

constexpr const BlockAttr NORMAL = (1ul << 0x0000);
constexpr const BlockAttr ENTRY = (1ul << 0x0001);
constexpr const BlockAttr LOOPBEGIN = (1ul << 0x0002);
constexpr const BlockAttr LOOPEND = (1ul << 0x0003);
constexpr const BlockAttr LOOPOUT = (1ul << 0x0004);
constexpr const BlockAttr BREAK = (1ul << 0x0005);
constexpr const BlockAttr CONTINUE = (1ul << 0x0006);
constexpr const BlockAttr GORETURN = (1ul << 0x0007);
constexpr const BlockAttr EXIT = (1ul << 0x0008);

std::string AttrToStr(BlockAttr);

class BasicBlock {
   protected:
    size_t idx;
    InstList inst_list;

    BlockAttr block_attr;

    BaseFunction *parent;

    static size_t blk_idx;

   public:
    BasicBlock(BlockAttr, BaseFunction *);
    ~BasicBlock() = default;

    size_t GetBlockIdx();

    InstList &GetInstList();

    size_t GetInstCnt() const;

    InstPtr &GetLastInst();
    void RemoveLastInst();

    void InsertInstBack(InstPtr);
    void InsertInstFront(InstPtr);
    void RemoveInst(InstPtr);

    bool FindBlkAttr(BlockAttr);
    void AppendBlkAttr(BlockAttr);
    void ClearSpecAttr(BlockAttr);
    BlockAttr GetBlockAttr() const;

    BaseFunction *GetParent();

    static void ResetBlkIdx();
    static size_t GetBlkIdx();
    static void SetBlkIdx(size_t);

    virtual std::string tollvmIR() = 0;
};
