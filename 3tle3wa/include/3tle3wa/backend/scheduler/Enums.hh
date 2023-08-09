#pragma once

enum SCHED_TYPE {
    FENCE,

    IMISC,
    IMUL,
    IDIVREM,

    FMISC,
    FMUL,
    FDIVREM,

    LOAD,
    STORE,
    JMPBR,
    FUNCCALL,
};