#pragma once

#include <cstddef>
#include <list>
#include <memory>

#include "3tle3wa/backend/Interface.hh"

class AsmInstruction;
class AsmProgress;

class AsmBasicBlock : public Serializable {
    size_t lbidx_;

    std::list<std::unique_ptr<AsmInstruction>> insts_;

    const AsmProgress *father_;

    void formatString(FILE *fp) final;

   public:
    AsmBasicBlock(size_t lbidx, AsmProgress *father);

    virtual ~AsmBasicBlock() = default;

    void Push(AsmInstruction *inst);
};