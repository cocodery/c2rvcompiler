#include "basicblock.hh"

std::string AttrToStr(BlockAttr attr) {
    switch (attr) {
        case NORMAL:
            return "normal";
        case ENTRY:
            return "entry";
        case LOOPBEGIN:
            return "loop-begin";
        case LOOPEND:
            return "loop-end";
        case LOOPOUT:
            return "loop-out";
        case BREAK:
            return "break";
        case CONTINUE:
            return "continue";
        case GORETURN:
            return "go-return";
        case EXIT:
            return "exit";

        default:
            assert(false);
    }
}

size_t BasicBlock::blk_idx = 1;

BasicBlock::BasicBlock(BlockAttr _attr) : idx(blk_idx++), inst_list(InstList()), block_attr(_attr) {}

size_t BasicBlock::GetBlockIdx() { return idx; }

InstList &BasicBlock::GetInstList() { return inst_list; }

size_t BasicBlock::GetInstCnt() const { return inst_list.size(); }

InstPtr &BasicBlock::GetLastInst() { return (*inst_list.rbegin()); }

void BasicBlock::InsertInstBack(InstPtr inst) { inst_list.push_back(inst); }
void BasicBlock::InsertInstFront(InstPtr inst) { inst_list.push_front(inst); }
void BasicBlock::RemoveInst(InstPtr inst) { inst_list.remove(inst); }

void BasicBlock::SetBlockAttr(BlockAttr _attr) { block_attr = _attr; }
BlockAttr BasicBlock::GetBlockAttr() const { return block_attr; }

void BasicBlock::ResetBlkIdx() { blk_idx = 1; }
size_t BasicBlock::GetBlkIdx() { return blk_idx; }
void BasicBlock::SetBlkIdx(size_t _blk_idx) { blk_idx = _blk_idx; }
