#include "3tle3wa/backend/scheduler/SchedMachine.hh"

#include "3tle3wa/backend/scheduler/SchedItem.hh"
#include "3tle3wa/backend/scheduler/SchedPolicy.hh"

SchedMachine::SchedMachine(std::unique_ptr<SchedPolicy> policy) : policy_(std::move(policy)) {}

void SchedMachine::Push(std::unique_ptr<SchedItem> item) {
    policy_->Push(item.get());
    items_.push_back(std::move(item));
}

void SchedMachine::Sched() { policy_->Sched(); }

SchedPolicy *SchedMachine::Policy() { return policy_.get(); }