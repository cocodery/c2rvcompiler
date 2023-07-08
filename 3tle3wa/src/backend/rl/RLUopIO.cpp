#include <cinttypes>

#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/utils.hh"

void UopRet::formatString(FILE *fp) {
    if (retval_ != nullptr) {
        fprintf(fp, "\tret\t%s\n", retval_->CString());
        return;
    }
    fprintf(fp, "\tret\n");
}

void UopCall::formatString(FILE *fp) {
    fprintf(fp, "\t");
    if (retval_ != nullptr) {
        fprintf(fp, "%s = ", retval_->CString());
    }

    if (libcall_) {
        fprintf(fp, "call %s(", callee_.c_str());
    } else if (tailcall_) {
        fprintf(fp, "j %s(", callee_.c_str());
    } else {
        fprintf(fp, "jal %s(", callee_.c_str());
    }

    if (not params_.empty()) {
        fprintf(fp, "%s", params_.front()->CString());
        for (size_t i = 1; i < params_.size(); ++i) {
            fprintf(fp, ", %s", params_.at(i)->CString());
        }
    }

    fprintf(fp, ")\n");
}

void UopLui::formatString(FILE *fp) {
    // avoid format
    fprintf(fp, "\tlui\t%s, 0x%" PRIx32 "\n", dst_->CString(), imm_up20_);
}

void UopMv::formatString(FILE *fp) {
    // avoid format
    if (src_ == nullptr and dst_->GetType() == VREG_TYPE::FLT) {
        fprintf(fp, "\tfmv.w.x\t%s, zero\n", dst_->CString());
        return;
    }
    fprintf(fp, "\tmv\t%s, %s\n", dst_->CString(), src_->CString());
}

void UopCvtS2W::formatString(FILE *fp) {
    // avoid format
    fprintf(fp, "\tfcvt.w.s\t%s, %s\n", dst_->CString(), src_->CString());
}

void UopCvtW2S::formatString(FILE *fp) {
    // avoid format
    fprintf(fp, "\tfcvt.s.w\t%s, %s\n", dst_->CString(), src_->CString());
}

void UopBranch::formatString(FILE *fp) {
    if (on_true_) {
        fprintf(fp, "\tbnez\t%s, .b%" PRIu64 "\n", cond_->CString(), dst_idx_);
    } else {
        fprintf(fp, "\tbeqz\t%s, .b%" PRIu64 "\n", cond_->CString(), dst_idx_);
    }
}

void UopICmpBranch::formatString(FILE *fp) {
    fprintf(fp, "\tb");
    switch (kind_) {
        case COMP_KIND::EQU:
            fprintf(fp, "eq");
            break;
        case COMP_KIND::NEQ:
            fprintf(fp, "ne");
            break;
        case COMP_KIND::LTH:
            fprintf(fp, "lt");
            break;
        case COMP_KIND::GTH:
            fprintf(fp, "gt");
            break;
        case COMP_KIND::LEQ:
            fprintf(fp, "le");
            break;
        case COMP_KIND::GEQ:
            fprintf(fp, "ge");
            break;
    }
    fprintf(fp, "\t%s, %s, .b%" PRIu64 "\n", lhs_->CString(), rhs_->CString(), dst_idx_);
}

void UopJump::formatString(FILE *fp) {
    // avoid format
    fprintf(fp, "\tj\t.b%" PRIu64 "\n", dst_idx_);
}

void UopLla::formatString(FILE *fp) {
    if (src_.length() != 0) {
        fprintf(fp, "\tlla\t%s, %s\n", dst_->CString(), src_.c_str());
        return;
    }
    // need more care
    fprintf(fp, "\tadd\t%s, %s, %" PRId32 "\n", dst_->CString(), "fp", 0);
}

void UopLoad::formatString(FILE *fp) {
    // avoid format
    fprintf(fp, "\tload\t%s, %" PRId32 "(%s)\n", dst_->CString(), off_lo12_, base_->CString());
}

void UopStore::formatString(FILE *fp) {
    if (src_ == nullptr) {
        fprintf(fp, "\tstore\tzero, %" PRId32 "(%s)\n", off_lo12_, base_->CString());
        return;
    }
    fprintf(fp, "\tstore\t%s, %" PRId32 "(%s)\n", src_->CString(), off_lo12_, base_->CString());
}

void UopFLoad::formatString(FILE *fp) {
    // avoid format
    fprintf(fp, "\tfload\t%s, %" PRId32 "(%s)\n", dst_->CString(), off_lo12_, base_->CString());
}

void UopFStore::formatString(FILE *fp) {
    // avoid format
    fprintf(fp, "\tfstore\t%s, %" PRId32 "(%s)\n", src_->CString(), off_lo12_, base_->CString());
}

void UopICmp::formatString(FILE *fp) {
    fprintf(fp, "\ticmp ");
    switch (kind_) {
        case COMP_KIND::EQU:
            fprintf(fp, "eq");
            break;
        case COMP_KIND::NEQ:
            fprintf(fp, "ne");
            break;
        case COMP_KIND::LTH:
            fprintf(fp, "lt");
            break;
        case COMP_KIND::GTH:
            fprintf(fp, "gt");
            break;
        case COMP_KIND::LEQ:
            fprintf(fp, "le");
            break;
        case COMP_KIND::GEQ:
            fprintf(fp, "ge");
            break;
    }
    if (lhs_ == nullptr) {
        fprintf(fp, "\t%s, zero, %s\n", dst_->CString(), rhs_->CString());
        return;
    } else if (rhs_ == nullptr) {
        fprintf(fp, "\t%s, %s, zero\n", dst_->CString(), lhs_->CString());
        return;
    }
    fprintf(fp, "\t%s, %s, %s\n", dst_->CString(), lhs_->CString(), rhs_->CString());
}

void UopFCmp::formatString(FILE *fp) {
    fprintf(fp, "\tfcmp ");
    switch (kind_) {
        case COMP_KIND::EQU:
            fprintf(fp, "eq");
            break;
        case COMP_KIND::NEQ:
            fprintf(fp, "ne");
            break;
        case COMP_KIND::LTH:
            fprintf(fp, "lt");
            break;
        case COMP_KIND::GTH:
            fprintf(fp, "gt");
            break;
        case COMP_KIND::LEQ:
            fprintf(fp, "le");
            break;
        case COMP_KIND::GEQ:
            fprintf(fp, "ge");
            break;
    }
    fprintf(fp, "\t%s, %s, %s\n", dst_->CString(), lhs_->CString(), rhs_->CString());
}

void UopIBin::formatString(FILE *fp) {
    fprintf(fp, "\t");
    switch (kind_) {
        case IBIN_KIND::ADD:
            fprintf(fp, "add");
            break;
        case IBIN_KIND::SUB:
            fprintf(fp, "sub");
            break;
        case IBIN_KIND::MUL:
            fprintf(fp, "mul");
            break;
        case IBIN_KIND::DIV:
            fprintf(fp, "div");
            break;
        case IBIN_KIND::REM:
            fprintf(fp, "rem");
            break;
        case IBIN_KIND::AND:
            fprintf(fp, "and");
            break;
        case IBIN_KIND::OR:
            fprintf(fp, "or");
            break;
        case IBIN_KIND::XOR:
            fprintf(fp, "xor");
            break;
        case IBIN_KIND::SLL:
            fprintf(fp, "sll");
            break;
        case IBIN_KIND::SRA:
            fprintf(fp, "sra");
            break;
    }
    fprintf(fp, "\t%s, %s, %s\n", dst_->CString(), lhs_->CString(), rhs_->CString());
}

void UopIBinImm::formatString(FILE *fp) {
    fprintf(fp, "\t");
    switch (kind_) {
        case IBIN_KIND::ADD:
            fprintf(fp, "add");
            break;
        case IBIN_KIND::SUB:
            fprintf(fp, "sub");
            break;
        case IBIN_KIND::AND:
            fprintf(fp, "and");
            break;
        case IBIN_KIND::OR:
            fprintf(fp, "or");
            break;
        case IBIN_KIND::XOR:
            fprintf(fp, "xor");
            break;
        case IBIN_KIND::SLL:
            fprintf(fp, "sll");
            break;
        case IBIN_KIND::SRA:
            fprintf(fp, "sra");
            break;
        default:
            panic("unexpected");
    }
    if (lhs_ == nullptr) {
        fprintf(fp, "\t%s, zero, %" PRId32 "\n", dst_->CString(), imm_lo12_);
        return;
    }
    fprintf(fp, "\t%s, %s, %" PRId32 "\n", dst_->CString(), lhs_->CString(), imm_lo12_);
}

void UopFBin::formatString(FILE *fp) {
    fprintf(fp, "\t");
    switch (kind_) {
        case FBIN_KIND::ADD:
            fprintf(fp, "add");
            break;
        case FBIN_KIND::SUB:
            fprintf(fp, "sub");
            break;
        case FBIN_KIND::MUL:
            fprintf(fp, "mul");
            break;
        case FBIN_KIND::DIV:
            fprintf(fp, "div");
            break;
    }
    fprintf(fp, "\t%s, %s, %s\n", dst_->CString(), lhs_->CString(), rhs_->CString());
}