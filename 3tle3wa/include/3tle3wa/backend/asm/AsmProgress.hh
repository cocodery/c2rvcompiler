#pragma once

#include <cstdio>
#include <list>
#include <memory>
#include <unordered_map>

class AsmBasicBlock;
class AsmGlobalValue;

class AsmProgress {
    char *label_;
    size_t label_len_;

    std::list<std::unique_ptr<AsmBasicBlock>> ablks_;

    AsmBasicBlock *ret_;

   public:
    AsmProgress(std::string &name);
    ~AsmProgress();

    void Push(std::unique_ptr<AsmBasicBlock> &ablk, bool isRet = false);

    AsmBasicBlock *CreateEntryBlock();

    AsmBasicBlock *ReturnBlock();
};