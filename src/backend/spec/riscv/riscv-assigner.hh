#pragma once

#include <cstddef>
#include <vector>

constexpr size_t total_reg_num = 64;

class RISCVAssigner {
    size_t rinfo[total_reg_num] = {0};
    // size_t roccupy[total_reg_num] = {0};
    std::vector<size_t> stk;

    size_t stkin(size_t vidx);

    size_t stkout(size_t vidx);

   public:
    RISCVAssigner();

    size_t gpr(size_t vidx);
    size_t fgpr(size_t vidx);

    size_t param_req(size_t vidx, size_t ridx);
    // size_t param_preset(size_t vidx, size_t ridx);

    size_t store(size_t vidx);
    size_t load(size_t vidx);

    void release(size_t vidx);
};