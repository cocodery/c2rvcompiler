#pragma once

#include <vector>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/scheduler/Enums.hh"
#include "3tle3wa/backend/utils.hh"

class AsmInstruction : public Serializable {
   protected:
    virtual void formatString(FILE *fp) = 0;

   public:
    virtual ~AsmInstruction() = default;

    virtual const std::vector<size_t> GetOperands() const = 0;
    virtual size_t GetResult() const = 0;
    virtual SCHED_TYPE GetSchedType() const = 0;
};