#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "baseValue.hh"
#include "variable.hh"

using OpCode = uint64_t;
constexpr OpCode OP_ADD = (1ul << 0x0000);     // binary add
constexpr OpCode OP_SUB = (1ul << 0x0001);     // binary sub
constexpr OpCode OP_MUL = (1ul << 0x0002);     // binary mul
constexpr OpCode OP_DIV = (1ul << 0x0003);     // binary div
constexpr OpCode OP_REM = (1ul << 0x0004);     // binary rem
constexpr OpCode OP_NOT = (1ul << 0x0005);     // unary not
constexpr OpCode OP_MINUS = (1ul << 0x0006);   // unary minus
constexpr OpCode OP_LSHIFT = (1ul << 0x0007);  // left shift
constexpr OpCode OP_RSHIFT = (1ul << 0x0008);  // right shift
constexpr OpCode OP_LTH = (1ul << 0x0009);     // less than
constexpr OpCode OP_LEQ = (1ul << 0x000A);     // less or equal
constexpr OpCode OP_GTH = (1ul << 0x000B);     // greater
constexpr OpCode OP_GEQ = (1ul << 0x000C);     // greater or equal
constexpr OpCode OP_EQU = (1ul << 0x000D);     // equal
constexpr OpCode OP_NEQ = (1ul << 0x000E);     // not equal

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

class Instruction {
   public:
    Instruction() = default;
    ~Instruction() = default;

    virtual bool IsAllocaInst() const;
    virtual bool IsStoreInst() const;
    virtual bool IsLoadInst() const;

    virtual std::string tollvmIR() = 0;
};