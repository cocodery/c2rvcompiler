#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <unordered_map>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/rl/Enums.hh"

class VirtualRegister;
class StackInfo;

class RLPlanner : public Serializable {
    std::unordered_map<uint64_t, StackInfo *> sinfo_map_;
    std::unordered_map<uint64_t, VirtualRegister *> vr_map_;

    std::list<std::unique_ptr<VirtualRegister>> vr_storage_;
    std::list<std::unique_ptr<StackInfo>> stk_storage_;

    size_t stkidx_;
    size_t regidx_;

    void formatString(FILE *fp) final;

   public:
    RLPlanner(size_t regidx);

    virtual ~RLPlanner() = default;

    StackInfo *Alloca(size_t len);

    VirtualRegister *Alloca(uint64_t vridx, size_t len);

    VirtualRegister *AllocVReg(VREG_TYPE type, uint64_t vridx);

    VirtualRegister *NewVReg(VREG_TYPE type);

    VirtualRegister *GetVReg(uint64_t vridx);

    void Link(uint64_t income, uint64_t old);

    void PlanRegisters();

    void PlanStackSpace();
};