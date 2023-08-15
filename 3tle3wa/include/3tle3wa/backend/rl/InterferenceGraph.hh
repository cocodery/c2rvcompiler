#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <set>

class VirtualRegister;
class RLPlanner;

class InterferenceGraph {
    class IGNode {
        size_t idx_;
        size_t color_;
        VirtualRegister *ref_;
        std::unordered_set<size_t> another_;

       public:
        IGNode(size_t idx, VirtualRegister *ref);
        void ConnectTo(size_t another);

        void SetColor(size_t color);

        size_t GetDegree() const;
        size_t GetColor() const;
        VirtualRegister *GetRef() const;

        size_t PreferWhichArg();
        bool PreferCallerSave();
        bool PreferCalleeSave();
        bool CanSpill();

        std::set<size_t> InterferWith(std::unordered_map<size_t, std::unique_ptr<IGNode>> &nodes_map);

        bool operator<(const IGNode &other) const;
        bool operator>(const IGNode &other) const;
    };

    std::unordered_map<size_t, std::unique_ptr<IGNode>> nodes_;

    RLPlanner *planner_;

    bool allocaColor(IGNode *node, std::set<size_t> &all, std::set<size_t> &args, std::set<size_t> &callers, std::set<size_t> &callees);

   public:
    InterferenceGraph(RLPlanner *planner);

    void RegisterIGNode(VirtualRegister *ref);
    void Connect(size_t first, size_t second);

    bool KempeOptimistic(std::set<size_t> &args, std::set<size_t> &callers, std::set<size_t> &callees);
};