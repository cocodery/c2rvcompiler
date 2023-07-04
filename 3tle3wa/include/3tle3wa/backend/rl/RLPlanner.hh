#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <unordered_map>

class VirtualRegister;
class StackInfo;
enum VREG_TYPE;

class StackPlanner {
    std::unordered_map<uint64_t, StackInfo *> sinfo_map_;

    std::list<std::unique_ptr<StackInfo>> storage_;

   public:
    StackInfo *Alloca(VREG_TYPE type, size_t len);

    void Plan();
};

class VirtRegPlanner {
    std::unordered_map<uint64_t, VirtualRegister *> vr_map_;

    std::list<std::unique_ptr<VirtualRegister>> storage_;

   public:
    VirtualRegister *AllocVReg(VREG_TYPE type, uint64_t vridx);

    VirtualRegister *GetVReg(uint64_t vridx);

    void Link(uint64_t income, uint64_t old);

    void Plan();
};