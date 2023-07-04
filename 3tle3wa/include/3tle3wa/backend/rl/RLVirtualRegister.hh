#pragma once

#include <cstdint>

#include "3tle3wa/backend/Interface.hh"

enum class VREG_TYPE { FLT, PTR, INT, ARR };

class StackInfo;

class VirtualRegister : public Serializable, public Weightable {
    VREG_TYPE type_;

    uint64_t vridx_;

    StackInfo *sinfo_;

    bool onstack_;

    void formatString(FILE *fp) final;

    public:

    
};