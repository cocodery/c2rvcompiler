#pragma once

#include <list>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "3tle3wa/backend/Interface.hh"

class AsmGen;
class AsmGlobalValue;
class CompilationUnit;
class InternalTranslation;
class NormalFuncList;

class RLGen : public Serializable {
    // look up table
    std::unordered_map<uint32_t, size_t> lc_map_;
    std::unordered_map<size_t, AsmGlobalValue *> gv_map_;

    std::vector<std::unique_ptr<InternalTranslation>> translation_tasks_;

    std::unique_ptr<AsmGen> asm_gen_;

    void registerGlobalValue(GlobalValue *, const std::string &);

    void registerNormalFunction(NormalFuncList &);

   public:
    RLGen(CompilationUnit &comp_unit_);

    ~RLGen();

    std::unique_ptr<AsmGen> &ExportAsmGen();

    void Register(CompilationUnit &comp_unit_);

    void SerialGenerate();

    void ParallelGenerate();
};