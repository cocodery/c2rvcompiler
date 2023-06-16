#include <sstream>

#include "rluop.hh"

std::string uop_ret::to_string() {
    std::stringstream ss;
    ss << "\tret" << std::endl;
    return ss.str();
}

std::string uop_set_param::to_string() {
    std::stringstream ss;
    ss << "\tmv a" << idx << ", " << src->to_string() << std::endl;
    return ss.str();
}

std::string uop_call::to_string() {
    std::stringstream ss;
    ss << "\tcall " << callee;

    if (retval) {
        ss << " => " << retval->to_string();
    }

    ss << std::endl;
    return ss.str();
}

std::string uop_li::to_string() {
    std::stringstream ss;
    ss << "\tli " << dst->to_string() << ", " << imm << std::endl;
    return ss.str();
}

std::string uop_mv::to_string() {
    std::stringstream ss;
    ss << "\tmv " << dst->to_string() << ", " << src->to_string() << std::endl;
    return ss.str();
}

std::string uop_cvts2w::to_string() {
    std::stringstream ss;
    ss << "\tfcvt.w.s " << dst->to_string() << ", " << src->to_string() << std::endl;
    return ss.str();
}

std::string uop_cvtw2s::to_string() {
    std::stringstream ss;
    ss << "\tfcvt.s.w " << dst->to_string() << ", " << src->to_string() << std::endl;
    return ss.str();
}

std::string uop_b::to_string() {
    std::stringstream ss;
    ss << "\tbnez " << cond->to_string() << ", " << pfx << lbid << std::endl;
    return ss.str();
}

std::string uop_j::to_string() {
    std::stringstream ss;
    ss << "\tj " << pfx << lbid << std::endl;
    return ss.str();
}

std::string uop_la::to_string() {
    std::stringstream ss;
    ss << "\tla " << dst->to_string() << ", " << src << std::endl;
    return ss.str();
}

std::string uop_load::to_string() {
    std::stringstream ss;
    if (dst->len == 4) {
        ss << "\tlw " << dst->to_string() << ", " << off << "(" << base->to_string() << ")" << std::endl;
    } else if (dst->len == 8) {
        ss << "\tld " << dst->to_string() << ", " << off << "(" << base->to_string() << ")" << std::endl;
    }
    return ss.str();
}

std::string uop_load_tag::to_string() {
    std::stringstream ss;
    ss << "\tlw " << dst->to_string() << ", " << src << std::endl;
    return ss.str();
}

std::string uop_store::to_string() {
    std::stringstream ss;
    if (src->len == 4) {
        ss << "\tsw " << src->to_string() << ", " << off << "(" << base->to_string() << ")" << std::endl;
    } else if (src->len == 8) {
        ss << "\tsd " << src->to_string() << ", " << off << "(" << base->to_string() << ")" << std::endl;
    }
    return ss.str();
}

std::string uop_store_tag::to_string() {
    std::stringstream ss;
    ss << "\tsw " << src->to_string() << ", " << dst << std::endl;
    return ss.str();
}

std::string uop_cmp::to_string() {
    std::stringstream ss;
    ss << "\tcmp ";
    if (kind == COMP_KIND::EQU) {
        ss << "equ ";
    } else if (kind == COMP_KIND::NEQ) {
        ss << "neq ";
    } else if (kind == COMP_KIND::GEQ) {
        ss << "geq ";
    } else if (kind == COMP_KIND::LEQ) {
        ss << "leq ";
    } else if (kind == COMP_KIND::GTH) {
        ss << "gth ";
    } else if (kind == COMP_KIND::LTH) {
        ss << "lth ";
    }
    ss << dst->to_string() << ", " << lhs->to_string() << ", " << rhs->to_string() << std::endl;
    return ss.str();
}

std::string uop_bin::to_string() {
    std::stringstream ss;
    ss << "\t";
    if (kind == IBIN_KIND::ADD) {
        ss << "add ";
    } else if (kind == IBIN_KIND::SUB) {
        ss << "sub ";
    } else if (kind == IBIN_KIND::MUL) {
        ss << "mul ";
    } else if (kind == IBIN_KIND::DIV) {
        ss << "div ";
    } else if (kind == IBIN_KIND::REM) {
        ss << "rem ";
    } else if (kind == IBIN_KIND::SLL) {
        ss << "sll ";
    } else if (kind == IBIN_KIND::SRA) {
        ss << "sra ";
    }
    ss << dst->to_string() << ", " << lhs->to_string() << ", " << rhs->to_string() << std::endl;
    return ss.str();
}

std::string uop_fbin::to_string() {
    std::stringstream ss;
    ss << "\tf";
    if (kind == FBIN_KIND::ADD) {
        ss << "add ";
    } else if (kind == FBIN_KIND::SUB) {
        ss << "sub ";
    } else if (kind == FBIN_KIND::MUL) {
        ss << "mul ";
    } else if (kind == FBIN_KIND::DIV) {
        ss << "div ";
    }
    ss << "f" << lhs->to_string() << ", f" << rhs->to_string() << std::endl;
    return ss.str();
}