#pragma once

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/utils.hh"

class AsmInstruction : public Serializable {
   protected:
    virtual void formatString(FILE *fp) = 0;

   public:
    virtual ~AsmInstruction() = default;
};