#pragma once

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

#include "backend/utils.hh"

class glb_value {
    const size_t glb_idx_;

    const size_t len_;

    const bool uninit_;

    std::vector<uint32_t> values_;

   public:
    glb_value(size_t glb_idx, size_t len, bool uninit, size_t reserve);

    void push(uint32_t value);

    void gen_asm(std::fstream &fs);
};