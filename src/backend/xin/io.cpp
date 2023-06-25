#include "backend/ir/uop/uop.hh"
#include "backend/xin/xin.hh"

extern char *dbgfile;

void cross_internal_manager::pir() {
    
    if (dbgfile == nullptr) {
        return;
    }

    std::fstream fs(dbgfile, std::ios::app);

    fs << rl_pgrs_.label_ << ":" << std::endl;
    for (auto &&rlbb : rl_pgrs_.bbs_) {
        fs << gen_pblk_label(rlbb->get_lbid()) << ":" << std::endl;

        fs << "\t # ";
        for (auto &&succ : rlbb->successer) {
            fs << succ << " ";
        }
        fs << "|" << std::endl;

        fs << "\t # ";
        for (auto &&domi : rlbb->dominator) {
            fs << domi << " ";
        }
        fs << "|" << std::endl;

        fs << "\t # ";
        for (auto &&lv : rlbb->dli.live_out) {
            fs << lv << " ";
        }
        fs << "|" << std::endl;

        for (auto &&uop : rlbb->ops_) {
            fs << "\t" << uop->c_str() << std::endl;
        }
    }

    rl_pgrs_.valc_.prinfo(fs);
}