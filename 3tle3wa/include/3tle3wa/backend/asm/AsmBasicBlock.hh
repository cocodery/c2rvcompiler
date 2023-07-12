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

    std::list<AsmInstruction *> view_insts_;

    const AsmProgress *father_;

    bool is_ret_;

    void formatString(FILE *fp) final;

    // ----- removes -----

    void rmNeedless();
    
    void rmNeedlessStore();

    // ----- combines -----

    void combineInstruction();

    void combineAddLS();

   public:
    AsmBasicBlock(size_t lbidx, AsmProgress *father);

    virtual ~AsmBasicBlock() = default;

    void Push(AsmInstruction *inst);

    void Pop();

    void SetIsRet(bool on);

    size_t Lbidx();

    bool IsRet();

    const char *FatherLabel() const;

    size_t SearchFirst();

    size_t GetBlockIdx();

    void ArchSchedule();

    void Peepholes();
};