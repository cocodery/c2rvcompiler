#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/IntervalTree.hh"
#include "3tle3wa/backend/rl/Enums.hh"

class VirtualRegister;
class StackInfo;
class AsmBasicBlock;
class RLProgress;

class RLPlanner final : public Serializable {
    std::unordered_map<uint64_t, StackInfo *> sinfo_map_;
    std::unordered_map<uint64_t, VirtualRegister *> vr_map_;

    std::list<std::unique_ptr<VirtualRegister>> vr_storage_;
    std::list<std::unique_ptr<StackInfo>> stk_storage_;

    size_t stkidx_;
    size_t regidx_;

    size_t param_stack_;
    size_t total_stack_size_;

    RLProgress *belong_to_;

    std::unordered_map<size_t, std::unique_ptr<IntervalManager>> real_reg_inval_;

    std::vector<std::unique_ptr<IntervalManager>> real_stk_inval_;

    std::vector<StackInfo *> real_stkinfo_;

    std::unordered_map<size_t, int64_t> place_to_save_;

    std::queue<VirtualRegister *> savings_;

    void formatString(FILE *fp) final;

    bool tryUse(VirtualRegister *vr, size_t rridx);

    void spillOn(VirtualRegister *vr);

   public:
    RLPlanner(size_t regidx);

    StackInfo *Alloca(size_t len);

    VirtualRegister *AllocParam(VREG_TYPE type, uint64_t vridx, size_t len, bool onstk, size_t pos);

    VirtualRegister *Alloca(uint64_t vridx, size_t len);

    VirtualRegister *AllocVReg(VREG_TYPE type, uint64_t vridx);

    VirtualRegister *NewVReg(VREG_TYPE type);

    VirtualRegister *GetVReg(uint64_t vridx);

    void Link(uint64_t income, uint64_t old);

    void PlanRegistersGreedy(size_t igpr[], size_t igprlen, size_t fgpr[], size_t fgprlen);

    void PlanRegistersBlockBased(size_t igpr[], size_t igprlen, size_t fgpr[], size_t fgprlen);

    void PlanStackSpace();

    void SetPstkSiz(size_t sps);

    void Init(AsmBasicBlock *abb);

    void Recover(AsmBasicBlock *abb);

    void BeforeCall(AsmBasicBlock *abb, std::unordered_set<VirtualRegister *> &living_regs);

    void RecoverCall(AsmBasicBlock *abb, const std::vector<VirtualRegister *> &after_this);

    void RegisterOwner(RLProgress *rlp);

    size_t TotalStackSize();
};