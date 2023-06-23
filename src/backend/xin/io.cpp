#include "xin.hh"

#include "../utils.hh"

void cross_internal_manager::pir() {
    std::fstream fs("dbg.ir.s", std::ios::out);
    fs << rl_pgrs_.label_ << ":" << std::endl;
    for (auto &&rlbb: rl_pgrs_.bbs_) {
        fs << gen_pblk_label(rlbb->get_lbid()) << ":" << std::endl;
        for (auto &&uop: rlbb->ops_) {
            fs << "\t" << uop->c_str() << std::endl;
        }
    }

    rl_pgrs_.valc_.prinfo();
}