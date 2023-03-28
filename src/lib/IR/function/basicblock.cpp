#include "basicblock.hh"

size_t BasicBlock::blk_idx = 1;

BasicBlock::BasicBlock() : idx(blk_idx++), inst_list(InstListType()) { 

}

size_t BasicBlock::getBlockIdx() {
    return this->idx;
}

InstListType &BasicBlock::getInstList() {
    return this->inst_list;
}

void BasicBlock::insertInst(InstPtr inst) {
    this->inst_list.push_back(inst);
}

BlockPtr BasicBlock::CreatePtr() {
    return std::make_shared<BasicBlock>();
}

void BasicBlock::resetBlkIdx() {
    blk_idx = 1;
}

std::string BasicBlock::tollvmIR() {
    std::stringstream ss;

    ss << "Block_" << this->idx << ":" << endl;
    for (auto &&inst : this->inst_list) {
        ss << '\t' << inst->tollvmIR() << endl;
    }

    return ss.str();
}
