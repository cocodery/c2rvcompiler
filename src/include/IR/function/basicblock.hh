#pragma once

#include <memory>

class BasicBlock {
public:
    BasicBlock() = default;
    ~BasicBlock() = default;
};

using BlockPtr = std::shared_ptr<BasicBlock>;