#pragma once

#include <cstddef>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "3tle3wa/backend/utils.hh"

class glb_value {
    std::string label_;

    const size_t len_;

    std::vector<uint32_t> values_;

   public:
    glb_value(const std::string &name, size_t len, bool uninit, size_t reserve);

    // 未来再改！
    
    bool onheap_{false};

    bool uninit_{false};

    void push(uint32_t value);

    bool onheap();

    std::string &name();

    void gen_asm(std::fstream &fs);
};