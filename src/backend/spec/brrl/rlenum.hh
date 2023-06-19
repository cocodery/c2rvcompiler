#pragma once

#include "opCode.hh"

enum class VREG_KIND { ZERO, IMM, REG, STK, LOC, PRM, FP, RA, SP };

enum class VSTK_KIND { ARR, PTR, VAL };

enum class COMP_KIND { LTH = OP_LTH, GTH = OP_GTH, LEQ = OP_LEQ, GEQ = OP_GEQ, EQU = OP_EQU, NEQ = OP_NEQ, NONE };

enum class IBIN_KIND {
    ADD = OP_ADD,
    SUB = OP_SUB,
    MUL = OP_MUL,
    DIV = OP_DIV,
    REM = OP_REM,
    SLL = OP_LSHIFT,
    SRA = OP_RSHIFT
};

enum class FBIN_KIND { ADD = OP_ADD, SUB = OP_SUB, MUL = OP_MUL, DIV = OP_DIV };

enum class FTRI_KIND { MADD, MSUB, NMADD, NMSUB };
