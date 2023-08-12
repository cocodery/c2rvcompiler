#pragma once

#include <list>

#include "3tle3wa/backend/scheduler/Enums.hh"
#include "3tle3wa/backend/scheduler/SchedPolicy.hh"

struct SchedItem;

class SchedFastUse final : public SchedPolicy {
   public:
    SchedFastUse();

    void Push(SchedItem *item);
    void Sched();
};