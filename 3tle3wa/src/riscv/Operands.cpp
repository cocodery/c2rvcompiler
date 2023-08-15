#include "3tle3wa/riscv/asm.hh"

const std::vector<size_t> riscv::Instruction::GetOperands() const {
    std::vector<size_t> vec;
    if (rs_ != riscv::zero) {
        vec.push_back(rs_);
    }
    if (rt_ != riscv::zero) {
        vec.push_back(rt_);
    }
    return vec;
}

size_t riscv::Instruction::GetResult() const {
    return rd_;
}
