#pragma once

#include <string>
#include <memory>

#include "../value/typeHeader.hh"
#include "../value/baseValue.hh"
#include "../value/variable.hh"

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

class Instruction {
public:
    Instruction() = default;
    ~Instruction() = default;

    virtual std::string toString() = 0;
    virtual std::string tollvmIR() = 0;
};