#include "3tle3wa/backend/scheduler/FastUse.hh"

SchedFastUse::SchedFastUse() {}

void SchedFastUse::Push(SchedItem *item) {
    
    view_.push_back(item); }

void SchedFastUse::Sched() {}

