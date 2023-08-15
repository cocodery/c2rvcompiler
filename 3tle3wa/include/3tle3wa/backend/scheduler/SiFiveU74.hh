#pragma once

#include <list>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/scheduler/Enums.hh"
#include "3tle3wa/backend/scheduler/SchedPolicy.hh"

struct SchedItem;

class SchedSiFiveU74 final : public SchedPolicy {
    struct AOVNode {
        SchedItem *self{nullptr};

        std::unordered_set<AOVNode *> post{};
        std::unordered_set<AOVNode *> prev{};

        bool issued{false};
    };

    struct Wrapper {
        AOVNode *node;

        bool operator<(const Wrapper &other) const;
    };

    struct RWStatus {
        AOVNode *writer{nullptr};
        std::unordered_set<AOVNode *> reader{};
    };
    struct AbstractMachine {
        std::unordered_map<SCHED_TYPE, bool> busy;
        std::unordered_map<size_t, bool> rdy;
    };

    struct Reservation {
        SCHED_TYPE type;
        size_t avail_time;
        size_t write;

        bool operator<(const Reservation &other) const;
    };

    AOVNode *fencer_;
    AOVNode *call_;
    std::unordered_map<size_t, RWStatus> status_;
    RWStatus memory_;

    AbstractMachine am_;

    std::list<std::unique_ptr<AOVNode>> storage_;

    std::vector<Wrapper> aovfree_;

   public:
    SchedSiFiveU74();

    void Push(SchedItem *item);
    void Sched();
};