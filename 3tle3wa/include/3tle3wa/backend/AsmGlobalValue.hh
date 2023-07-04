#pragma once

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "3tle3wa/backend/Interface.hh"

class AsmGlobalValue : public Serializable {
    char *name_{nullptr};
    size_t name_len_{0};

    std::vector<uint32_t> values_;

    bool on_heap_;

    size_t data_len_;

    bool uninit_;

    size_t align_;

    void formatString(FILE *fp) final;

   public:
    AsmGlobalValue(const std::string &name, size_t len, bool uninit, size_t reserve);

    ~AsmGlobalValue();

    void Push(uint32_t value);

    bool OnHeap() const;

    const char *Name() const;
};