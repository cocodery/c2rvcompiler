#include "basicblock.hh"

size_t BasicBlock::blk_idx = 1;

BasicBlock::BasicBlock() : idx(blk_idx++), inst_list(InstListType()) {}

size_t BasicBlock::getBlockIdx() { return this->idx; }

InstListType &BasicBlock::getInstList() { return this->inst_list; }

void BasicBlock::insertInst(InstPtr inst) { this->inst_list.push_back(inst); }

void BasicBlock::ResetBlkIdx() { blk_idx = 1; }
size_t BasicBlock::GetBlkIdx() { return blk_idx; }
void BasicBlock::SetBlkIdx(size_t _blk_idx) { blk_idx = _blk_idx; }
