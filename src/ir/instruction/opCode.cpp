#include "ir/instruction/opCode.hh"

bool IsCommutative(OpCode opcode) {
    switch (opcode) {
        case OP_ADD:
        case OP_MUL:
            return true;
        default:
            return false;
    }
}