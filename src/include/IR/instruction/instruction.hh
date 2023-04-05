#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <sstream>

#include "../value/typeHeader.hh"
#include "../value/baseValue.hh"
#include "../value/variable.hh"

using OpCode = uint64_t;
constexpr OpCode OP_ADD     = 0x0001; // binary add
constexpr OpCode OP_SUB     = 0x0002; // binary sub
constexpr OpCode OP_MUL     = 0x0003; // binary mul
constexpr OpCode OP_DIV     = 0x0004; // binary div
constexpr OpCode OP_REM     = 0x0005; // binary rem
constexpr OpCode OP_NOT     = 0x0006; // unary not
constexpr OpCode OP_MINUS   = 0x0007; // unary minus
constexpr OpCode OP_LSHIFT  = 0x0008; // left shift
constexpr OpCode OP_RSHIFT  = 0x0009; // right shift
constexpr OpCode OP_LTH     = 0x000A; // less than
constexpr OpCode OP_LEQ     = 0x000B; // less or equal
constexpr OpCode OP_GTH     = 0x000C; // greater
constexpr OpCode OP_GEQ     = 0x000D; // greater or equal
constexpr OpCode OP_EQU     = 0x000E; // equal
constexpr OpCode OP_NEQ     = 0x000F; // unequal

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

class Instruction {
public:
    Instruction() = default;
    ~Instruction() = default;

    virtual std::string tollvmIR() = 0;
};