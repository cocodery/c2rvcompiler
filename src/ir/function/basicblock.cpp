#include "basicblock.hh"

std::string AttrToStr(BlockAttr attr) {
    std::stringstream ss;

    if (attr & NORMAL) ss << " normal";
    if (attr & ENTRY) ss << " entry";
    if (attr & LOOPBEGIN) ss << " loop-begin";
    if (attr & LOOPEND) ss << " loop-end";
    if (attr & LOOPEND) ss << " loop-out";
    if (attr & BREAK) ss << " break";
    if (attr & CONTINUE) ss << " continue";
    if (attr & GORETURN) ss << " go-return";
    if (attr & EXIT) ss << " exit";

    return ss.str();
}

size_t BasicBlock::blk_idx = 1;

BasicBlock::BasicBlock(BlockAttr _attr) : idx(blk_idx++), inst_list(InstList()), block_attr(_attr) {}

size_t BasicBlock::GetBlockIdx() { return idx; }

InstList &BasicBlock::GetInstList() { return inst_list; }

size_t BasicBlock::GetInstCnt() const { return inst_list.size(); }

InstPtr &BasicBlock::GetLastInst() {
    auto &&last_inst = (*inst_list.rbegin());
    assert(last_inst->IsJumpInst() || last_inst->IsBranchInst() || last_inst->IsReturnInst());
    return last_inst;
}

void BasicBlock::RemoveLastInst() {
    auto &&last_inst = GetLastInst();
    assert(last_inst->IsJumpInst() || last_inst->IsBranchInst());
    inst_list.pop_back();
}

void BasicBlock::InsertInstBack(InstPtr inst) { inst_list.push_back(inst); }
void BasicBlock::InsertInstFront(InstPtr inst) { inst_list.push_front(inst); }
void BasicBlock::RemoveInst(InstPtr inst) { inst_list.remove(inst); }

bool BasicBlock::FindBlkAttr(BlockAttr _attr) { return ((block_attr & _attr) != 0); };
void BasicBlock::AppendBlkAttr(BlockAttr _attr) { block_attr |= _attr; }
void BasicBlock::ClearSpecAttr(BlockAttr _attr) { block_attr &= ~_attr; }
BlockAttr BasicBlock::GetBlockAttr() const { return block_attr; }

void BasicBlock::ResetBlkIdx() { blk_idx = 1; }
size_t BasicBlock::GetBlkIdx() { return blk_idx; }
void BasicBlock::SetBlkIdx(size_t _blk_idx) { blk_idx = _blk_idx; }
