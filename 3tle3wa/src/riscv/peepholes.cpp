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

void AsmBasicBlock::rmNeedlessStore() {
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

        Transaction *load_trx = nullptr;
        Transaction *store_trx = nullptr;

        auto &&ctrx = rv_cur->ToTrx();
        auto &&ntrx = rv_cur->ToTrx();

        if (ctrx.optype_ == riscv::OpType::LoadData) {
            load_trx = &ctrx;
        } else if (ctrx.optype_ == riscv::OpType::StoreData) {
            store_trx = &ctrx;
        }

        if (ntrx.optype_ == riscv::OpType::LoadData) {
            load_trx = &ntrx;
        } else if (ntrx.optype_ == riscv::OpType::StoreData) {
            store_trx = &ntrx;
        }

        if (load_trx == nullptr or store_trx == nullptr) {
            cur_it = nxt_it++;
            continue;
        }

        auto ls_redundant = [](Transaction &load, Transaction &store) -> bool {
            if (load.resource_occupied_ == store.resource_required_.at(0)) {
                if (load.other_info_.at(0) == store.other_info_.at(0)) {
                    if (load.resource_required_.at(0) == store.resource_required_.at(1)) {
                        return true;
                    }
                }
            }
            return false;
        };

        auto cond = (dynamic_cast<riscv::LD *>(rv_cur) != nullptr and dynamic_cast<riscv::SD *>(rv_nxt) != nullptr) or
                    (dynamic_cast<riscv::SD *>(rv_cur) != nullptr and dynamic_cast<riscv::LD *>(rv_nxt) != nullptr) or
                    (dynamic_cast<riscv::LW *>(rv_cur) != nullptr and dynamic_cast<riscv::SW *>(rv_nxt) != nullptr) or
                    (dynamic_cast<riscv::SW *>(rv_cur) != nullptr and dynamic_cast<riscv::LW *>(rv_nxt) != nullptr) or
                    (dynamic_cast<riscv::FLW *>(rv_cur) != nullptr and dynamic_cast<riscv::FSW *>(rv_nxt) != nullptr) or
                    (dynamic_cast<riscv::FSW *>(rv_cur) != nullptr and dynamic_cast<riscv::FLW *>(rv_nxt) != nullptr);

        if (cond and ls_redundant(*load_trx, *store_trx)) {
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