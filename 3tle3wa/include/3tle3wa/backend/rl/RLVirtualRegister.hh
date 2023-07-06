#pragma once

#include <cstdint>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/rl/Enums.hh"

class StackInfo;

class VirtualRegister : public Serializable, public Weightable {
    VREG_TYPE type_;

    uint64_t vridx_;

    StackInfo *sinfo_;

    bool onstack_;

    size_t real_regidx_;
    bool assigned_;

    void formatString(FILE *fp) final;

   public:
    VirtualRegister(VREG_TYPE type, uint64_t vridx);
    
    void SetOnStack(bool on);
    
    void SetStackInfo(StackInfo *sinfo);

    void calculateWeight();

    uint64_t GetVRIdx() const;
};