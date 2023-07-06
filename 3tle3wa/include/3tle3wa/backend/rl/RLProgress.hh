#pragma once

#include <list>
#include <memory>
#include <vector>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/rl/Enums.hh"

class RLPlanner;
class RLBasicBlock;
class VirtualRegister;
class Variable;

class RLProgress : public Serializable {
    char *label_;
    size_t label_len_;

    std::vector<VirtualRegister *> params_;

    std::unique_ptr<RLPlanner> planner_;

    std::list<std::unique_ptr<RLBasicBlock>> rlbbs_;

    void formatString(FILE *fp) final;

   public:
    RLProgress(const std::string &name);

    void RegisterPlanner(std::unique_ptr<RLPlanner> &planner);

    void Push(std::unique_ptr<RLBasicBlock> &rlbb);

    void SetParam(Variable *var, VREG_TYPE type);

    void VirtualSchedule();

    void RealSchedule();
    
    void NoSchedule();
};