#include "riscv-assigner.hh"
#include "riscv-asm.hh"

RISCVAssigner::RISCVAssigner() {
    for (size_t i = 0; i < total_reg_num; ++i) {
        rinfo[i] = (size_t)-1;
    }
}

size_t RISCVAssigner::gpr(size_t vidx) {
    static size_t allocable[] = {5,  6,  7,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18,
                                 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

    constexpr size_t aasize = 21;

    for (size_t i = 0; i < aasize; ++i) {
        if (rinfo[allocable[i]] == (size_t)-1) {
            rinfo[allocable[i]] = vidx;
            return allocable[i];
        }
    }

    return (size_t)-1;
}

size_t RISCVAssigner::fgpr(size_t vidx) {
    constexpr size_t aasize = 32;

    for (size_t i = aasize; i < (aasize * 2); ++i) {
        if (rinfo[i] == (size_t)-1) {
            rinfo[i] = vidx;
            return i;
        }
    }

    return (size_t)-1;
}

size_t RISCVAssigner::store(size_t vidx) {
    size_t empty = (size_t)-1;

    for (size_t i = 0; i < total_reg_num; ++i) {
        if (rinfo[i] == vidx) {
            rinfo[i] = (size_t)-1;
            empty = i;
            break;
        }
    }

    if (empty == (size_t)-1) {
        return empty;
    }

    return stkin(vidx);
}

size_t RISCVAssigner::load(size_t vidx) { return stkout(vidx); }

void RISCVAssigner::release(size_t vidx) {
    for (size_t i = 0; i < total_reg_num; ++i) {
        if (rinfo[i] == vidx) {
            rinfo[i] = (size_t)-1;
        }
    }
    stkout(vidx);
}

size_t RISCVAssigner::stkin(size_t vidx) {
    for (size_t i = 0; i < stk.size(); ++i) {
        if (stk[i] == (size_t)-1) {
            stk[i] = vidx;
            return i;
        }
    }
    stk.push_back(vidx);
    return stk.size() - 1;
}

size_t RISCVAssigner::stkout(size_t vidx) {
    for (size_t i = 0; i < stk.size(); ++i) {
        if (stk[i] == vidx) {
            stk[i] = (size_t)-1;
            return i;
        }
    }
    return (size_t)-1;
}

static size_t argreg[] __attribute_maybe_unused__ = {
    10, 11, 12, 13, 14, 15, 16, 17
};

size_t RISCVAssigner::param_req(size_t vidx, size_t ridx) {
    Assert(ridx < 8, "arg reg out of range (%lu)", ridx);

    auto stkoff = (size_t)-1;

    if (rinfo[ridx] == (size_t)-1) {
        rinfo[ridx] = vidx;
    } else {
        stkoff = store(rinfo[ridx]);
        rinfo[ridx] = vidx;
    }
    return stkoff;
}

// size_t RISCVAssigner::param_preset(size_t vidx, size_t ridx) {
//     if (rinfo[ridx] == (size_t)-1) {

//     }
// }