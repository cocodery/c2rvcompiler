#pragma once

#include <cstdio>
#include <list>
#include <memory>
#include <unordered_map>

#include "3tle3wa/backend/Interface.hh"

class AsmBasicBlock;
class AsmGlobalValue;

class AsmProgress : public Serializable {
    char *label_;
    size_t label_len_;

    std::list<std::unique_ptr<AsmBasicBlock>> ablks_;

    AsmBasicBlock *ret_;

    void formatString(FILE *fp) final;

   public:
    AsmProgress(std::string &name);
    ~AsmProgress();

    void Push(std::unique_ptr<AsmBasicBlock> &ablk, bool is_ret = false);

    AsmBasicBlock *CreateEntryBlock();

    AsmBasicBlock *ReturnBlock();

    const char *Label() const;
};