#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/Interface.hh"

struct ConstValueInfo;

class NormalFunction;
class CtrlFlowGraphNode;

class AsmGlobalValue;
class RLProgress;
class RLBasicBlock;
class RLPlanner;
class VirtualRegister;
class AsmProgress;

// control flow inst
class ReturnInst;
class JumpInst;
class BranchInst;

// compare inst
class ICmpInst;
class FCmpInst;

// binary inst
class IBinaryInst;
class FBinaryInst;
class FNegInst;

// memory inst
class AllocaInst;
class StoreInst;
class LoadInst;
class GetElementPtrInst;

// type convert
class SitoFpInst;
class FptoSiInst;
class ZextInst;

// other inst
class CallInst;
class BitCastInst;
class PhiInst;

using NormalFuncPtr = std::shared_ptr<NormalFunction>;

class InternalTranslation final : public Serializable {
    const NormalFuncPtr &fptr_;
    const std::unordered_map<uint32_t, size_t> &lc_map_;
    const std::unordered_map<size_t, AsmGlobalValue *> &gv_map_;

    std::unique_ptr<RLProgress> rlps_;

    std::unique_ptr<AsmProgress> apg_;

    std::unordered_set<ICmpInst *> icmp_map;

    struct IT_STAT {
        CtrlFlowGraphNode *cur_cfg;
        CtrlFlowGraphNode *nxt_cfg;

        RLBasicBlock *cur_blk;
        RLPlanner *planner;

        bool meetcall;
        bool meettail;
    } curstat_;

    void li(VirtualRegister *dst, ConstValueInfo &cinfo);

    void formatString(FILE *fp);

   public:
    InternalTranslation(const NormalFuncPtr &fptr, const std::unordered_map<uint32_t, size_t> &lc_map,
                        const std::unordered_map<size_t, AsmGlobalValue *> &gv_map);

    void DoTranslation();

    void DoVSchedule();

    void NoSchedule();

    void DoAssignment();

    void DoRSchedule();

    void DoTranslateToAsm();

    std::unique_ptr<AsmProgress> &ExportAPG();

    void Translate(ReturnInst *);
    void Translate(JumpInst *);
    void Translate(BranchInst *);

    void Translate(ICmpInst *);
    void Translate(FCmpInst *);

    void Translate(IBinaryInst *);
    void Translate(FBinaryInst *);
    void Translate(FNegInst *);

    void Translate(AllocaInst *);
    void Translate(StoreInst *);
    void Translate(LoadInst *);
    void Translate(GetElementPtrInst *);

    void Translate(SitoFpInst *);
    void Translate(FptoSiInst *);
    void Translate(ZextInst *);

    void Translate(CallInst *);
    void Translate(BitCastInst *);
    void Translate(PhiInst *);
};