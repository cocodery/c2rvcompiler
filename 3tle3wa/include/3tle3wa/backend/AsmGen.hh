#pragma once

#include <list>
#include <memory>
#include <unordered_map>

#include "3tle3wa/backend/Interface.hh"

// asm class declaration
class AsmGlobalValue;
class AsmProgress;

class AsmGen : public Serializable {

    // for storage
    std::list<std::unique_ptr<AsmGlobalValue>> gv_storage_;
    std::list<std::unique_ptr<AsmProgress>> pg_storage_;

    void formatString(FILE *fp) final;

   public:
    void PushAsmGlobalValue(std::unique_ptr<AsmGlobalValue> &agv);

    void PushAsmProgress(std::unique_ptr<AsmProgress> &agv);
};