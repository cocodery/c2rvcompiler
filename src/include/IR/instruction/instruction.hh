#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <sstream>

#include "../value/typeHeader.hh"
#include "../value/baseValue.hh"
#include "../value/variable.hh"

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

class Instruction {
public:
    Instruction() = default;
    ~Instruction() = default;

    virtual std::string tollvmIR() = 0;
};