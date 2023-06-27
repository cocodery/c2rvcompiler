#pragma once

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>
#include <string_view>

#include "3tle3wa/backend/utils.hh"

class glb_value {
    std::string label_;

    const size_t len_;

    const bool uninit_;

    std::vector<uint32_t> values_;

    bool onheap_{false};

   public:
    glb_value(const std::string &name, size_t len, bool uninit, size_t reserve);

    void push(uint32_t value);

    bool onheap();

    std::string &name();

    void gen_asm(std::fstream &fs);
};