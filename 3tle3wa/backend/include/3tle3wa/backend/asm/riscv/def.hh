#pragma once

#include "3tle3wa/backend/asm/inst.hh"

extern cstr gpr[];

namespace riscv {

constexpr rid_t zero = 0;
constexpr rid_t ra = 1;
constexpr rid_t sp = 2;
constexpr rid_t gp = 3;
constexpr rid_t tp = 4;
constexpr rid_t t0 = 5;
constexpr rid_t t1 = 6;
constexpr rid_t t2 = 7;

constexpr rid_t fp = 8;
constexpr rid_t s0 = 8;

constexpr rid_t s1 = 9;

constexpr rid_t a0 = 10 + 0;
constexpr rid_t a1 = 10 + 1;
constexpr rid_t a2 = 10 + 2;
constexpr rid_t a3 = 10 + 3;
constexpr rid_t a4 = 10 + 4;
constexpr rid_t a5 = 10 + 5;
constexpr rid_t a6 = 10 + 6;
constexpr rid_t a7 = 10 + 7;

constexpr rid_t s2 = 18;
constexpr rid_t s3 = 19;
constexpr rid_t s4 = 20;
constexpr rid_t s5 = 21;
constexpr rid_t s6 = 22;
constexpr rid_t s7 = 23;
constexpr rid_t s8 = 24;
constexpr rid_t s9 = 25;
constexpr rid_t s10 = 26;
constexpr rid_t s11 = 27;
constexpr rid_t t3 = 28;
constexpr rid_t t4 = 29;
constexpr rid_t t5 = 30;
constexpr rid_t t6 = 31;

constexpr rid_t ft0 = 32;
constexpr rid_t ft1 = 33;
constexpr rid_t ft2 = 34;
constexpr rid_t ft3 = 35;
constexpr rid_t ft4 = 36;
constexpr rid_t ft5 = 37;
constexpr rid_t ft6 = 38;
constexpr rid_t ft7 = 39;
constexpr rid_t fs0 = 40;
constexpr rid_t fs1 = 41;

constexpr rid_t fa0 = 42 + 0;
constexpr rid_t fa1 = 42 + 1;
constexpr rid_t fa2 = 42 + 2;
constexpr rid_t fa3 = 42 + 3;
constexpr rid_t fa4 = 42 + 4;
constexpr rid_t fa5 = 42 + 5;
constexpr rid_t fa6 = 42 + 6;
constexpr rid_t fa7 = 42 + 7;

constexpr rid_t fs2 = 50;
constexpr rid_t fs3 = 51;
constexpr rid_t fs4 = 52;
constexpr rid_t fs5 = 53;
constexpr rid_t fs6 = 54;
constexpr rid_t fs7 = 55;
constexpr rid_t fs8 = 56;
constexpr rid_t fs9 = 57;
constexpr rid_t fs10 = 58;
constexpr rid_t fs11 = 59;
constexpr rid_t ft8 = 60;
constexpr rid_t ft9 = 61;
constexpr rid_t ft10 = 62;
constexpr rid_t ft11 = 63;

}  // namespace riscv