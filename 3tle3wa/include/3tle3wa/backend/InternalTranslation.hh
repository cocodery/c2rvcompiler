#pragma once

#include <cstdint>
#include <unordered_map>

class NormalFuncPtr;
class AsmGlobalValue;

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

class InternalTranslation {
    const NormalFuncPtr &fptr_;
    const std::unordered_map<uint32_t, size_t> &lc_map_;
    const std::unordered_map<size_t, AsmGlobalValue *> &gv_map_;

   public:
    InternalTranslation(const NormalFuncPtr &fptr, const std::unordered_map<uint32_t, size_t> &lc_map,
                        const std::unordered_map<size_t, AsmGlobalValue *> &gv_map);
    
    void DoTranslation();

    void Translate(ReturnInst *);
    void Translate(JumpInst *);
    void Translate(BranchInst *);

    void Translate(ICmpInst *);
    void Translate(FCmpInst *);
    
    void Translate(IBinaryInst *);
    void Translate(FBinaryInst *);
    
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