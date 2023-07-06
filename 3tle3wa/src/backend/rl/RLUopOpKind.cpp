#include "3tle3wa/backend/rl/RLUop.hh"

OPERATION_KIND UopRet::GetOpKind() const { return OPERATION_KIND::BRAJMP; }

OPERATION_KIND UopCall::GetOpKind() const { return OPERATION_KIND::CALLOP; }

OPERATION_KIND UopLui::GetOpKind() const { return OPERATION_KIND::INTOPT; }

OPERATION_KIND UopMv::GetOpKind() const { return OPERATION_KIND::INTOPT; }

OPERATION_KIND UopCvtS2W::GetOpKind() const { return OPERATION_KIND::FLTOPT; }

OPERATION_KIND UopCvtW2S::GetOpKind() const { return OPERATION_KIND::FLTOPT; }

OPERATION_KIND UopBranch::GetOpKind() const { return OPERATION_KIND::BRAJMP; }

OPERATION_KIND UopJump::GetOpKind() const { return OPERATION_KIND::BRAJMP; }

OPERATION_KIND UopLla::GetOpKind() const { return OPERATION_KIND::INTOPT; }

OPERATION_KIND UopLoad::GetOpKind() const { return OPERATION_KIND::ACCMEM; }

OPERATION_KIND UopStore::GetOpKind() const { return OPERATION_KIND::ACCMEM; }

OPERATION_KIND UopFLoad::GetOpKind() const { return OPERATION_KIND::ACCMEM; }

OPERATION_KIND UopFStore::GetOpKind() const { return OPERATION_KIND::ACCMEM; }

OPERATION_KIND UopICmp::GetOpKind() const { return OPERATION_KIND::INTOPT; }

OPERATION_KIND UopFCmp::GetOpKind() const { return OPERATION_KIND::FLTOPT; }

OPERATION_KIND UopIBin::GetOpKind() const {
    switch (kind_) {
        case IBIN_KIND::MUL:
        case IBIN_KIND::DIV:
        case IBIN_KIND::REM:
            return OPERATION_KIND::MDROPT;
            break;
        default:
            break;
    }
    return OPERATION_KIND::INTOPT;
}

OPERATION_KIND UopIBinImm::GetOpKind() const {
    switch (kind_) {
        case IBIN_KIND::MUL:
        case IBIN_KIND::DIV:
        case IBIN_KIND::REM:
            return OPERATION_KIND::MDROPT;
            break;
        default:
            break;
    }
    return OPERATION_KIND::INTOPT;
}

OPERATION_KIND UopFBin::GetOpKind() const { return OPERATION_KIND::FLTOPT; }

OPERATION_KIND UopICmpBranch::GetOpKind() const { return OPERATION_KIND::BRAJMP; }