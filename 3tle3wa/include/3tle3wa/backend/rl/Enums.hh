#pragma once

#include "3tle3wa/ir/instruction/opCode.hh"

enum class VREG_TYPE { FLT, PTR, INT };

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
    SRA = OP_RSHIFT,
    MULHS,
    SRL,
    XOR,
    AND,
    OR,
};

enum class FBIN_KIND {
    // 运算枚举对应 opCode

    ADD = OP_ADD,
    SUB = OP_SUB,
    MUL = OP_MUL,
    DIV = OP_DIV,
    NEG,
    MAX,
    MIN,
    ABS,
};

enum class PHI_KIND { IMM, REG };

enum class OPERATION_KIND {
    BRAJMP = 0,  // branch and jump
    INTOPT = 1,  // common integer operation
    CALLOP = 2,  // call func
    MDROPT = 3,  // mul div rem operation
    FLTOPT = 4,  // float operation
    ACCMEM = 5,  // access memory
};
