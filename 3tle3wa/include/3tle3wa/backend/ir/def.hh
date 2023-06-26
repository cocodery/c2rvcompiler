#pragma once

#include "3tle3wa/ir/instruction/opCode.hh"

enum class VREG_KIND { ZERO, IMM, REG, STK, LOC, PRM, SPEC };

enum class VREG_TYPE { FLT, PTR, INT, ARR };

enum class COMP_KIND {
    // 比较枚举对应 opCode

    LTH = OP_LTH,
    GTH = OP_GTH,
    LEQ = OP_LEQ,
    GEQ = OP_GEQ,
    EQU = OP_EQU,
    NEQ = OP_NEQ,
};

enum class IBIN_KIND {
    // 运算枚举对应 opCode

    ADD = OP_ADD,
    SUB = OP_SUB,
    MUL = OP_MUL,
    DIV = OP_DIV,
    REM = OP_REM,
    SLL = OP_LSHIFT,
    SRA = OP_RSHIFT
};

enum class FBIN_KIND {
    // 运算枚举对应 opCode

    ADD = OP_ADD,
    SUB = OP_SUB,
    MUL = OP_MUL,
    DIV = OP_DIV
};

enum class FTRI_KIND { MADD, MSUB, NMADD, NMSUB };

using xlen_t = int64_t;
using uxlen_t = uint64_t;