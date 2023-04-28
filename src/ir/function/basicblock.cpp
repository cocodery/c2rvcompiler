#include "basicblock.hh"

size_t BasicBlock::blk_idx = 1;

BasicBlock::BasicBlock() : idx(blk_idx++), inst_list(InstListType()) {}

size_t BasicBlock::GetBlockIdx() { return idx; }

InstListType &BasicBlock::GetInstList() { return inst_list; }

void BasicBlock::InsertInstBack(InstPtr inst) { inst_list.push_back(inst); }
void BasicBlock::InsertInstFront(InstPtr inst) { inst_list.push_front(inst); }
void BasicBlock::RemoveInst(InstPtr inst) { inst_list.remove(inst); }

void BasicBlock::ResetBlkIdx() { blk_idx = 1; }
size_t BasicBlock::GetBlkIdx() { return blk_idx; }
void BasicBlock::SetBlkIdx(size_t _blk_idx) { blk_idx = _blk_idx; }
