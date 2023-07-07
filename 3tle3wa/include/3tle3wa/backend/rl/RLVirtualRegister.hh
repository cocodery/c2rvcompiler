#pragma once

#include <cstdint>
#include <cstddef>
#include <map>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/rl/Enums.hh"
#include "3tle3wa/backend/rl/Indicater.hh"
#include "3tle3wa/backend/IntervalTree.hh"

class StackInfo;
class AsmBasicBlock;

class VirtualRegister : public Serializable, public Weightable {
    VREG_TYPE type_;

    uint64_t vridx_;

    StackInfo *sinfo_;
    StackInfo *ainfo_;

    bool onstack_;

    size_t real_regidx_;
    size_t param_regidx_;

    bool assigned_;
    bool param_;

    IntervalManager imgr_;

    bool saving_;
    int64_t save_off_;

    void formatString(FILE *fp) final;

   public:
    VirtualRegister(VREG_TYPE type, uint64_t vridx);

    void SetOnStack(bool on);

    void SetParam(size_t pos);

    void SetRRidx(size_t rridx);

    void SetSaving(int64_t off);

    int64_t SavingInfo();

    bool IsSaving();

    bool IsAssigned();

    bool IsParam();

    bool OnStk();

    bool PtrOnStk();

    bool FGPR() const;

    void SetStackInfo(StackInfo *sinfo);

    StackInfo *GetStackInfo();

    StackInfo *GetAllocaInfo();

    VREG_TYPE GetType();

    void NewInterval(size_t idx, size_t begin, size_t end, size_t len);

    void UpdateIntervalBegin(size_t idx, size_t begin, size_t len);

    IntervalManager &Imgr();

    void UpdateIntervalEnd(size_t idx, size_t begin, size_t end, size_t len);

    void CalcuWeight();

    double CalcuBlkWeight(size_t lbidx);

    double NowWeight();

    void CallAt(size_t lbidx, size_t pos);

    void UseAt(size_t lbidx);

    uint64_t GetVRIdx() const;

    size_t GetPPos();

    size_t GetRRid();

    size_t GetSize();

    size_t GetRRidWithSaving(AsmBasicBlock *abb);

    void LoadTo(size_t to, AsmBasicBlock *abb, size_t to_tmp = 0);

    void StoreFrom(size_t from, AsmBasicBlock *abb, size_t to_tmp = 0);
};