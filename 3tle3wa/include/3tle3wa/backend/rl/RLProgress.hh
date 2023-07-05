#pragma once

#include <list>
#include <memory>
#include <vector>

class RLPlanner;
class RLBasicBlock;
class VirtualRegister;
enum class VREG_TYPE;

class RLProgress {
    char *label_;
    size_t label_len_;

    std::vector<VirtualRegister *> params_;

    std::unique_ptr<RLPlanner> planner_;

    std::list<std::unique_ptr<RLBasicBlock>> rlbbs_;

   public:
    RLProgress(const std::string &name);

    void RegisterPlanner(std::unique_ptr<RLPlanner> &planner);

    void Push(std::unique_ptr<RLBasicBlock> &rlbb);

    void SetParam(Variable *var, VREG_TYPE type);
};