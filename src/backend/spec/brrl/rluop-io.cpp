#include <sstream>

#include "../../debug/Logs.hh"
#include "rluop.hh"

std::string uop_ret::to_string() {
    std::stringstream ss;
    ss << "\tret" << std::endl;
    return ss.str();
}

std::string uop_set_param::to_string() {
    std::stringstream ss;
    ss << "\t";
    if (onflt) {
        ss << "fmv fa";
    } else {
        ss << "mv a";
    }
    ss << idx << ", " << src->to_string() << std::endl;
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
    if (src->kind == VREG_KIND::STK) {
        auto stkinfo = (virt_stkinf *)src->value;
        ss << "\taddi " << dst->to_string() << ", fp, " << stkinfo->off << std::endl;
    } else {
        ss << "\tmv " << dst->to_string() << ", " << src->to_string() << std::endl;
    }
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
    ss << "\tbeqz " << cond->to_string() << ", " << pfx << lbid << std::endl;
    return ss.str();
}

std::string uop_cmp_b::to_string() {
    std::stringstream ss;
    ss << "\tb";
    if (kind == COMP_KIND::EQU) {
        ss << "eq ";
    } else if (kind == COMP_KIND::NEQ) {
        ss << "ne ";
    } else if (kind == COMP_KIND::GEQ) {
        ss << "ge ";
    } else if (kind == COMP_KIND::LTH) {
        ss << "lt ";
    } else {
        panic("unexpected");
    }
    ss << lhs->to_string() << ", " << rhs->to_string() << ", " << pfx << lbid << std::endl;
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
        if (onflt) {
            ss << "\tflw.s ";
        } else {
            ss << "\tlw ";
        }
        ss << dst->to_string() << ", " << off << "(" << base->to_string() << ")" << std::endl;
    } else if (dst->len == 8 || dst->len == 0) {
        ss << "\tld " << dst->to_string() << ", " << off << "(" << base->to_string() << ")" << std::endl;
    } else {
        panic("unexpected");
    }
    return ss.str();
}

std::string uop_load_tag::to_string() {
    std::stringstream ss;
    if (onflt) {
        ss << "\tflw ";
    } else {
        ss << "\tlw ";
    }
    ss << dst->to_string() << ", " << src << std::endl;
    return ss.str();
}

std::string uop_store::to_string() {
    std::stringstream ss;
    if (src->len == 4) {
        if (onflt) {
            ss << "\tfsw.s ";
        } else {
            ss << "\tsw ";
        }
        ss << src->to_string() << ", " << off << "(" << base->to_string() << ")" << std::endl;
    } else if (src->len == 8 || src->len == 0) {
        ss << "\tsd " << src->to_string() << ", " << off << "(" << base->to_string() << ")" << std::endl;
    } else {
        panic("unexpected");
    }
    return ss.str();
}

std::string uop_store_tag::to_string() {
    std::stringstream ss;
    if (onflt) {
        ss << "\tfsw.s ";
    } else {
        ss << "\tsw ";
    }
    ss << src->to_string() << ", " << dst << std::endl;
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
    } else {
        panic("unexpected");
    }
    ss << dst->to_string() << ", " << lhs->to_string() << ", " << rhs->to_string() << std::endl;
    return ss.str();
}

std::string uop_bin::to_string() {
    std::stringstream ss;
    ss << "\t";
    if (kind == IBIN_KIND::ADD) {
        ss << "add";
    } else if (kind == IBIN_KIND::SUB) {
        ss << "sub";
    } else if (kind == IBIN_KIND::MUL) {
        ss << "mul";
    } else if (kind == IBIN_KIND::DIV) {
        ss << "div";
    } else if (kind == IBIN_KIND::REM) {
        ss << "rem";
    } else if (kind == IBIN_KIND::SLL) {
        ss << "sll";
    } else if (kind == IBIN_KIND::SRA) {
        ss << "sra";
    } else {
        panic("unexpected");
    }
    if (rhs->kind == VREG_KIND::IMM) {
        ss << "i";

        if (dst->len == 4) {
            ss << "w";
        }

        ss << " " << dst->to_string() << ", " << lhs->to_string() << ", " << (int64_t)rhs->value << std::endl;
    } else {
        if (dst->len == 4) {
            ss << "w";
        }
        ss << " " << dst->to_string() << ", " << lhs->to_string() << ", " << rhs->to_string() << std::endl;
    }
    return ss.str();
}

std::string uop_fbin::to_string() {
    std::stringstream ss;
    ss << "\tf";
    if (kind == FBIN_KIND::ADD) {
        ss << "add";
    } else if (kind == FBIN_KIND::SUB) {
        ss << "sub";
    } else if (kind == FBIN_KIND::MUL) {
        ss << "mul";
    } else if (kind == FBIN_KIND::DIV) {
        ss << "div";
    } else {
        panic("unexpected");
    }
    ss << ".s " << dst->to_string() << ", " << lhs->to_string() << ", " << rhs->to_string() << std::endl;
    return ss.str();
}

std::string uop_ftri::to_string() {
    std::stringstream ss;
    ss << "\tf";
    if (kind == FTRI_KIND::MADD) {
        ss << "madd";
    } else if (kind == FTRI_KIND::MSUB) {
        ss << "msub";
    } else if (kind == FTRI_KIND::NMADD) {
        ss << "nmadd";
    } else if (kind == FTRI_KIND::NMSUB) {
        ss << "nmsub";
    } else {
        panic("unexpected");
    }
    ss << ".s " << dst->to_string() << ", " << op1->to_string() << ", " << op2->to_string() << ", " << op3->to_string()
       << std::endl;
    return ss.str();
}