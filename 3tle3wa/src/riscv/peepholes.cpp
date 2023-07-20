#include <algorithm>
#include <deque>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/riscv/asm.hh"
#include "3tle3wa/riscv/cpu/SifiveU74.hh"
#include "3tle3wa/riscv/spec.hh"

void AsmBasicBlock::rmNeedlessLS() {
    // for now only focus on inst pair

    auto nxt_it = insts_.begin();
    auto cur_it = nxt_it++;

    while (nxt_it != insts_.end()) {
        auto cur = cur_it->get();
        auto nxt = nxt_it->get();

        auto rv_cur = dynamic_cast<riscv::Instruction *>(cur);
        auto rv_nxt = dynamic_cast<riscv::Instruction *>(nxt);

        if (rv_cur == nullptr or rv_nxt == nullptr) {
            cur_it = nxt_it++;
            continue;
        }

        auto &&ctrx = rv_cur->ToTrx();
        auto &&ntrx = rv_nxt->ToTrx();

        bool redundant = false;

        if (auto c = dynamic_cast<riscv::LD *>(rv_cur); c != nullptr) {
            if (auto n = dynamic_cast<riscv::LD *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_occupied_ == ntrx.resource_occupied_ and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(0);
            } else if (auto n = dynamic_cast<riscv::SD *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_occupied_ == ntrx.resource_required_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(1);
            }
        } else if (auto c = dynamic_cast<riscv::SD *>(rv_cur); c != nullptr) {
            if (auto n = dynamic_cast<riscv::LD *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_occupied_ and
                            ctrx.resource_required_.at(1) == ntrx.resource_required_.at(0);
            } else if (auto n = dynamic_cast<riscv::SD *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(0) and
                            ctrx.resource_required_.at(1) == ntrx.resource_required_.at(1);
            }
        } else if (auto c = dynamic_cast<riscv::LW *>(rv_cur); c != nullptr) {
            if (auto n = dynamic_cast<riscv::LW *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_occupied_ == ntrx.resource_occupied_ and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(0);
            } else if (auto n = dynamic_cast<riscv::SW *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_occupied_ == ntrx.resource_required_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(1);
            }
        } else if (auto c = dynamic_cast<riscv::SW *>(rv_cur); c != nullptr) {
            if (auto n = dynamic_cast<riscv::LW *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_occupied_ and
                            ctrx.resource_required_.at(1) == ntrx.resource_required_.at(0);
            } else if (auto n = dynamic_cast<riscv::SW *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(0) and
                            ctrx.resource_required_.at(1) == ntrx.resource_required_.at(1);
            }
        } else if (auto c = dynamic_cast<riscv::FLW *>(rv_cur); c != nullptr) {
            if (auto n = dynamic_cast<riscv::FLW *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_occupied_ == ntrx.resource_occupied_ and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(0);
            } else if (auto n = dynamic_cast<riscv::FSW *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_occupied_ == ntrx.resource_required_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(1);
            }
        } else if (auto c = dynamic_cast<riscv::FSW *>(rv_cur); c != nullptr) {
            if (auto n = dynamic_cast<riscv::FLW *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_occupied_ and
                            ctrx.resource_required_.at(1) == ntrx.resource_required_.at(0);
            } else if (auto n = dynamic_cast<riscv::FSW *>(rv_nxt); n != nullptr) {
                redundant = ctrx.other_info_.at(0) == ntrx.other_info_.at(0) and
                            ctrx.resource_required_.at(0) == ntrx.resource_required_.at(0) and
                            ctrx.resource_required_.at(1) == ntrx.resource_required_.at(1);
            }
        }

        if (redundant) {
            nxt_it = insts_.erase(nxt_it);
            continue;
        }

        cur_it = nxt_it++;
    }
}

void AsmBasicBlock::combineAddLS() {
    // for now only focus on inst pair

    // auto nxt_it = insts_.begin();
    // auto cur_it = nxt_it++;

    // while (nxt_it != insts_.end()) {
    //     auto cur = cur_it->get();
    //     auto nxt = nxt_it->get();

    //     auto add = dynamic_cast<riscv::ADDI *>(cur);
    //     if (add == nullptr) {
    //         cur_it = nxt_it++;
    //         continue;
    //     }
    //     auto &&add_trx = add->ToTrx();

    //     auto base = add_trx.resource_required_.at(0);
    //     auto off = add_trx.other_info_.at(0);

    //     auto rv = dynamic_cast<riscv::Instruction *>(nxt);
    //     if (rv == nullptr) {
    //         cur_it = nxt_it++;
    //         continue;
    //     }
    //     auto &&rv_trx = rv->ToTrx();

    //     if (not rv_trx.resource_required_.empty() and not rv_trx.other_info_.empty()) {
    //         if (rv_trx.optype_ == riscv::OpType::LoadData) {
    //             auto nwoff = rv_trx.other_info_.at(0) + off;
    //             auto dst = rv_trx.resource_occupied_;

    //             if (rv_trx.resource_required_.at(0) == add_trx.resource_occupied_ and ImmWithin(12, nwoff)) {
    //                 if (dynamic_cast<riscv::LD *>(rv) != nullptr) {
    //                     auto load = new riscv::LD(dst, base, nwoff);

    //                     cur_it = insts_.insert(nxt_it, std::unique_ptr<riscv::Instruction>(load));
    //                     nxt_it = insts_.erase(nxt_it);
    //                 } else if (dynamic_cast<riscv::LW *>(rv) != nullptr) {
    //                     auto load = new riscv::LW(dst, base, nwoff);

    //                     cur_it = insts_.insert(nxt_it, std::unique_ptr<riscv::Instruction>(load));
    //                     nxt_it = insts_.erase(nxt_it);
    //                 } else if (dynamic_cast<riscv::FLW *>(rv) != nullptr) {
    //                     auto load = new riscv::FLW(dst, base, nwoff);

    //                     cur_it = insts_.insert(nxt_it, std::unique_ptr<riscv::Instruction>(load));
    //                     nxt_it = insts_.erase(nxt_it);
    //                 }

    //                 continue;
    //             }
    //         }

    //         if (rv_trx.optype_ == riscv::OpType::StoreData) {
    //             auto nwoff = rv_trx.other_info_.at(0) + off;
    //             auto src = rv_trx.resource_occupied_;

    //             if (rv_trx.resource_required_.at(0) == add_trx.resource_occupied_ and ImmWithin(12, nwoff)) {
    //                 if (dynamic_cast<riscv::SD *>(rv) != nullptr) {
    //                     auto load = new riscv::SD(src, base, nwoff);

    //                     cur_it = insts_.insert(nxt_it, std::unique_ptr<riscv::Instruction>(load));
    //                     nxt_it = insts_.erase(nxt_it);
    //                 } else if (dynamic_cast<riscv::SW *>(rv) != nullptr) {
    //                     auto load = new riscv::SW(src, base, nwoff);

    //                     cur_it = insts_.insert(nxt_it, std::unique_ptr<riscv::Instruction>(load));
    //                     nxt_it = insts_.erase(nxt_it);
    //                 } else if (dynamic_cast<riscv::FSW *>(rv) != nullptr) {
    //                     auto load = new riscv::FSW(src, base, nwoff);

    //                     cur_it = insts_.insert(nxt_it, std::unique_ptr<riscv::Instruction>(load));
    //                     nxt_it = insts_.erase(nxt_it);
    //                 }

    //                 continue;
    //             }
    //         }
    //     }

    //     cur_it = nxt_it++;
    // }
}