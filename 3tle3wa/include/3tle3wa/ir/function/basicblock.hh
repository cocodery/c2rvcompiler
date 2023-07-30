#pragma once

#include <cstddef>
#include <cstring>
#include <list>
#include <memory>

#include "3tle3wa/ir/instruction/instruction.hh"

class BasicBlock;

using InstList = std::list<InstPtr>;
using BlockPtr = std::shared_ptr<BasicBlock>;

class BaseFunction;

struct BlkAttr {
    typedef size_t BlkType;
    static const BlkType Normal = (1ul << 0x0000);
    static const BlkType Entry = (1ul << 0x0001);
    static const BlkType Exit = (1ul << 0x0002);

    static const BlkType LoopTag = (1ul << 0x000e);
    static const BlkType BranchTag = (1ul << 0x000f);

    BlkType blk_type;

    bool before_blk;

    bool cond_begin;
    bool cond_end;

    bool body_begin;
    bool body_end;

    bool iftrue_begin;
    bool iftrue_end;

    bool iffalse_begin;
    bool iffalse_end;

    bool structure_out;

    BlkAttr(BlkType _type = Normal)
        : blk_type(_type),
          before_blk(false),
          cond_begin(false),
          cond_end(false),
          body_begin(false),
          body_end(false),
          iftrue_begin(false),
          iftrue_end(false),
          iffalse_begin(false),
          iffalse_end(false),
          structure_out(false) {}

    bool CheckBlkType(BlkType _blk_type) { return ((blk_type & _blk_type) != 0); }
};

class BasicBlock {
   protected:
    size_t idx;
    InstList inst_list;

    BaseFunction *parent;

    static size_t blk_idx;

   public:
    BlkAttr blk_attr;

    BasicBlock(BaseFunction *, BlkAttr::BlkType blk_tpye = BlkAttr::Normal);
    ~BasicBlock() = default;

    size_t GetBlockIdx();

    InstList &GetInstList();

    size_t GetInstCnt() const;

    InstPtr &GetLastInst();
    void RemoveLastInst();

    void InsertInstBack(InstPtr);
    void InsertInstFront(InstPtr);
    void RemoveInst(InstPtr);

    BaseFunction *GetParent();

    static void ResetBlkIdx();
    static size_t GetBlkIdx();
    static void SetBlkIdx(size_t);

    void AppBlkType(BlkAttr::BlkType blk_type) { blk_attr.blk_type |= blk_type; }
    void ClrBlkType(BlkAttr::BlkType blk_type) { blk_attr.blk_type &= (~blk_type); }

    virtual std::string tollvmIR() = 0;
};
