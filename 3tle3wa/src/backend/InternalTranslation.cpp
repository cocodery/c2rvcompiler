#include "3tle3wa/backend/InternalTranslation.hh"

InternalTranslation::InternalTranslation(const NormalFuncPtr &fptr, const std::unordered_map<uint32_t, size_t> &lc_map,
                                         const std::unordered_map<size_t, AsmGlobalValue *> &gv_map)
    : fptr_(fptr), lc_map_(lc_map), gv_map_(gv_map) {}

void InternalTranslation::DoTranslation() {
    
}