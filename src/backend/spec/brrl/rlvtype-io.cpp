#include <vector>

#include "../../../debug/Logs.hh"
#include "../../utils.hh"
#include "../riscv/riscv-asm.hh"
#include "rlvtype.hh"

std::string virt_reg::to_string() {
    std::stringstream ss;
    if (kind == VREG_KIND::LOC) {
        ss << "LC." << std::hex << value;
    } else if (kind == VREG_KIND::IMM) {
        ss << value;
    } else if (kind == VREG_KIND::REG || kind == VREG_KIND::PRM) {
        if (inrreg != 0) {
            ss << rgnm[inrreg];
        } else {
            if (isflt) {
                ss << "f" << value;
            } else {
                ss << "r" << value;
            }
        }
    } else if (kind == VREG_KIND::PRM) {
        ss << "p" << value;
    } else if (kind == VREG_KIND::STK) {
        ss << "sp:" << std::hex << value;
    } else if (kind == VREG_KIND::ZERO) {
        ss << "zero";
    } else if (kind == VREG_KIND::FP) {
        ss << "fp";
    } else if (kind == VREG_KIND::SP) {
        ss << "sp";
    } else if (kind == VREG_KIND::RA) {
        ss << "ra";
    }
    return ss.str();
}