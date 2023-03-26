#pragma once

#include <vector>
#include <memory>

#include "../instHeader.hh"

class BasicBlock;

using InstListType = std::vector<InstPtr>;
using BlockPtr = std::shared_ptr<BasicBlock>;

class BasicBlock {
private:
    size_t idx;
    InstListType inst_list;

    static size_t blk_idx;
public:
    BasicBlock();
    ~BasicBlock() = default;

    size_t getBlockIdx();

    InstListType &getInstList();

    void insertInst(InstPtr);

    static BlockPtr CreateBlock();
    static void resetBlkIdx();
};
