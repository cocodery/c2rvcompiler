#include "3tle3wa/backend/rl/RLUop.hh"

using VecVR = std::vector<VirtualRegister *>;

const VecVR UopRet::GetOperands() const { return VecVR{retval_}; }

VirtualRegister *UopRet::GetResult() const { return nullptr; }

const VecVR UopCall::GetOperands() const { return params_; }

VirtualRegister *UopCall::GetResult() const { return retval_; }

const VecVR UopLui::GetOperands() const { return VecVR{}; }

VirtualRegister *UopLui::GetResult() const { return dst_; }

const VecVR UopMv::GetOperands() const {
    if (src_ != nullptr) {
        return VecVR{src_};
    }
    return VecVR{};
}

VirtualRegister *UopMv::GetResult() const { return dst_; }

const VecVR UopCvtS2W::GetOperands() const { return VecVR{src_}; }

VirtualRegister *UopCvtS2W::GetResult() const { return dst_; }

const VecVR UopCvtW2S::GetOperands() const { return VecVR{src_}; }

VirtualRegister *UopCvtW2S::GetResult() const { return dst_; }

const VecVR UopBranch::GetOperands() const { return VecVR{cond_}; }

VirtualRegister *UopBranch::GetResult() const { return nullptr; }

const VecVR UopICmpBranch::GetOperands() const { return VecVR{lhs_, rhs_}; }

VirtualRegister *UopICmpBranch::GetResult() const { return nullptr; }

const VecVR UopJump::GetOperands() const { return VecVR{}; }

VirtualRegister *UopJump::GetResult() const { return nullptr; }

const VecVR UopLla::GetOperands() const { return VecVR{}; }

VirtualRegister *UopLla::GetResult() const { return dst_; }

const VecVR UopLoad::GetOperands() const { return VecVR{base_}; }

VirtualRegister *UopLoad::GetResult() const { return dst_; }

const VecVR UopStore::GetOperands() const {
    if (src_ != nullptr) {
        return VecVR{src_, base_};
    }
    return VecVR{base_};
}

VirtualRegister *UopStore::GetResult() const { return nullptr; }

const VecVR UopFLoad::GetOperands() const { return VecVR{base_}; }

VirtualRegister *UopFLoad::GetResult() const { return dst_; }

const VecVR UopFStore::GetOperands() const { return VecVR{src_, base_}; }

VirtualRegister *UopFStore::GetResult() const { return nullptr; }

const VecVR UopICmp::GetOperands() const {
    VecVR vec;
    if (lhs_ != nullptr) {
        vec.push_back(lhs_);
    }
    if (rhs_ != nullptr) {
        vec.push_back(rhs_);
    }
    return vec;
}

VirtualRegister *UopICmp::GetResult() const { return dst_; }

const VecVR UopFCmp::GetOperands() const { return VecVR{lhs_, rhs_}; }

VirtualRegister *UopFCmp::GetResult() const { return dst_; }

const VecVR UopIBin::GetOperands() const {
    VecVR vec;
    if (lhs_ != nullptr) {
        vec.push_back(lhs_);
    }
    if (rhs_ != nullptr) {
        vec.push_back(rhs_);
    }
    return vec;
}

VirtualRegister *UopIBin::GetResult() const { return dst_; }

const VecVR UopIBinImm::GetOperands() const {
    if (lhs_ != nullptr) {
        return VecVR{lhs_};
    }
    return VecVR{};
}

VirtualRegister *UopIBinImm::GetResult() const { return dst_; }

const VecVR UopFBin::GetOperands() const { return VecVR{lhs_, rhs_}; }

VirtualRegister *UopFBin::GetResult() const { return dst_; }