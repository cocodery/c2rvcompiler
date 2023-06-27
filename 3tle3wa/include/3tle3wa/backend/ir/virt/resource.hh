#include <unordered_map>

#include "3tle3wa/backend/ir/virt/register.hh"

class virt_resource {
public:
    std::unordered_map<size_t, size_t> vrsc_map_;
    std::unordered_map<size_t, size_t> fvrsc_map_;
    std::unordered_map<size_t, size_t> hit_map_;

    std::unordered_map<size_t, size_t> idx_map_;

    std::unordered_map<size_t, size_t> rregs_;
    std::unordered_map<size_t, std::shared_ptr<stk_info>> stks_; 

    std::unordered_set<size_t> ifree_;
    std::unordered_set<size_t> ffree_;

    std::vector<size_t> ialloced_;
    std::vector<size_t> falloced_;

    size_t idx_ = 0;

public:
    void set(virt_reg *vr, rid_t vrid);
    void alc(virt_reg *vr);
    void rls(virt_reg *vr);
    void rlsall();
    void access(virt_reg *vr, size_t n = 1);

    void alcreal(rl_progress &rlp, size_t iuse, size_t fuse);
};