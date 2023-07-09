#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLGen.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/instruction/opCode.hh"

// scheduler need restrict pointer

struct AOVNode {
    std::unordered_set<AOVNode *> prev;
    std::unordered_set<AOVNode *> post;
    UopGeneral *self;
    bool issued;
};

struct RWStatus {
    std::unordered_set<AOVNode *> reader;
    AOVNode *writer;

    enum { READING, WRITING } status;
};

void InternalTranslation::DoVSchedule() { rlps_->VirtualSchedule(); }

void InternalTranslation::DoRSchedule() { apg_->ArchSchedule(); }

void InternalTranslation::NoSchedule() { rlps_->NoSchedule(); }

void RLProgress::VirtualSchedule() {
    for (auto &&bb : rlbbs_) {
        bb->VirtualSchedule();
    }
}

void RLProgress::NoSchedule() {
    for (auto &&bb : rlbbs_) {
        bb->NoSchedule();
    }
}

void RLBasicBlock::NoSchedule() {
    for (auto &&uop : ops_) {
        op_view_.push_back(uop.get());
    }
}

void RLBasicBlock::VirtualSchedule() {
    std::list<std::unique_ptr<AOVNode>> aov_nodes;
    std::unordered_map<size_t, RWStatus> stat_map;

    std::unordered_set<AOVNode *> free_nodes;

    for (auto &&uop : ops_) {
        auto node = std::make_unique<AOVNode>();
        node->issued = false;
        node->self = uop.get();
        auto &&nself = node.get();

        auto operands = uop->GetOperands();
        if (not operands.empty()) {
            for (auto &&operand : operands) {
                auto vridx = operand->GetVRIdx();

                if (auto fnd = stat_map.find(vridx); fnd != stat_map.end()) {
                    auto &&stat = fnd->second;

                    if (stat.status == RWStatus::WRITING) {
                        // RAW
                        node->prev.insert(stat.writer);
                        stat.writer->post.insert(nself);
                    } else {
                        // RAR
                        stat.reader.insert(nself);
                    }
                } else {
                    RWStatus stat;
                    stat.status = RWStatus::READING;
                    stat.reader.insert(nself);

                    CRVC_UNUSE auto result = stat_map.emplace(vridx, stat);
                    Assert(result.second, "fail");
                }
            }
        }

        auto dst = uop->GetResult();
        if (dst != nullptr) {
            auto vridx = dst->GetVRIdx();
            if (auto fnd = stat_map.find(vridx); fnd != stat_map.end()) {
                auto &&stat = fnd->second;

                if (stat.status == RWStatus::WRITING) {
                    // WAW
                    node->prev.insert(stat.writer);
                    node->prev.insert(stat.reader.begin(), stat.reader.end());
                    node->prev.erase(nself);

                    for (auto &&prev : node->prev) {
                        prev->post.insert(nself);
                    }

                    stat.writer = nself;
                    stat.reader.clear();
                } else {
                    // WAR
                    node->prev.insert(stat.reader.begin(), stat.reader.end());
                    node->prev.erase(nself);

                    for (auto &&prev : node->prev) {
                        prev->post.insert(nself);
                    }

                    stat.status = RWStatus::WRITING;
                    stat.writer = nself;
                    stat.reader.clear();
                }
            } else {
                RWStatus stat;
                stat.status = RWStatus::WRITING;
                stat.writer = nself;

                CRVC_UNUSE auto result = stat_map.emplace(vridx, stat);
                Assert(result.second, "fail");
            }
        }

        if (node->prev.empty()) {
            free_nodes.insert(nself);
        }

        aov_nodes.push_back(std::move(node));
    }

    UopGeneral *prev_issued = nullptr;

    while (not free_nodes.empty()) {
        if (free_nodes.size() == 1) {
            auto one = (*free_nodes.begin());
            free_nodes.erase(free_nodes.begin());

            if (one->self->GetOpKind() == OPERATION_KIND::BRAJMP) {
                op_view_.push_back(one->self);
                break;
            }

            op_view_.push_back(one->self);
            prev_issued = one->self;
            one->issued = true;
            for (auto &&post : one->post) {
                post->prev.erase(one);

                if (post->prev.empty()) {
                    free_nodes.insert(post);
                }
            }

            continue;
        }

        auto it = free_nodes.begin();
        OPERATION_KIND last_kind = OPERATION_KIND::BRAJMP;
        if (prev_issued != nullptr) {
            last_kind = prev_issued->GetOpKind();
        }

        auto curhigh_level = OPERATION_KIND::BRAJMP;
        AOVNode *curhigh = nullptr;

        auto choice_level = OPERATION_KIND::BRAJMP;
        AOVNode *choice = nullptr;

        while (it != free_nodes.end()) {
            auto cur = *it;
            auto cur_level = cur->self->GetOpKind();

            if (curhigh == nullptr) {
                curhigh_level = cur_level;
                curhigh = cur;
            } else if (cur_level > curhigh_level) {
                curhigh_level = cur_level;
                curhigh = cur;
            } else if (cur_level == curhigh_level) {
                Assert(curhigh != nullptr, "ill formed");

                if (curhigh->post.size() < cur->post.size()) {
                    curhigh = cur;
                }
            }

            if (last_kind == OPERATION_KIND::INTOPT or last_kind == OPERATION_KIND::CALLOP or cur_level != last_kind) {
                if (choice == nullptr) {
                    choice_level = cur_level;
                    choice = cur;
                } else if (cur_level > choice_level) {
                    choice_level = cur_level;
                    choice = cur;
                } else if (cur_level == choice_level) {
                    Assert(choice != nullptr, "ill formed");

                    if (choice->post.size() < cur->post.size()) {
                        choice = cur;
                    }
                }
            }

            it++;
        }

        if (choice == nullptr) {
            choice = curhigh;
        }

        free_nodes.erase(choice);

        prev_issued = choice->self;
        choice->issued = true;
        for (auto &&post : choice->post) {
            post->prev.erase(choice);

            if (post->prev.empty()) {
                free_nodes.insert(post);
            }
        }

        op_view_.push_back(choice->self);
    }
}