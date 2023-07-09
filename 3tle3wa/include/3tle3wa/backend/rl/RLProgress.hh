#pragma once

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "3tle3wa/backend/Interface.hh"
#include "3tle3wa/backend/rl/Enums.hh"

class RLPlanner;
class RLBasicBlock;
class VirtualRegister;
class Variable;
class AsmProgress;

class RLProgress : public Serializable {
    char *label_;
    size_t label_len_;

    bool has_lib_call_;
    bool has_call_other_;

    std::vector<VirtualRegister *> params_;

    std::unique_ptr<RLPlanner> planner_;

    std::list<std::unique_ptr<RLBasicBlock>> rlbbs_;

    std::unordered_map<size_t, RLBasicBlock *> lbmap_;

    size_t ips_ = 0;
    size_t fps_ = 0;
    size_t sps_ = 0;

    void formatString(FILE *fp) final;

    void computeLivenessInfomation();

    void computeLiveInterval();

    void rvAssigner();

   public:
    RLProgress(const std::string &name);

    void RegisterPlanner(std::unique_ptr<RLPlanner> &planner);

    void Push(std::unique_ptr<RLBasicBlock> &rlbb);

    void SetParam(Variable *var, VREG_TYPE type);

    void MeetLibCall();

    void MeetCallOther();

    void VirtualSchedule();

    void NoSchedule();

    void DoAssignment();

    void DoToAsm(AsmProgress *apg);

    RLBasicBlock *FindBlkById(size_t lbidx);
};