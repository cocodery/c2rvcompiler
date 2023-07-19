#include "3tle3wa/ir/function/loop.hh"

#include <sstream>

void PrintLoop(Loop &loop) {
    auto &&PrintTab = [](loop_depth_t depth) {
        std::stringstream ss;
        for (int idx = 0; idx < depth; ++idx) {
            ss << "\t";
        }
        return ss.str();
    };

    if (loop.before_loop) {
        cout << PrintTab(loop.loop_depth) << "\b\bLoop_" << loop.loop_depth << endl;
        cout << PrintTab(loop.loop_depth) << "Block_" << loop.before_loop->GetBlockIdx() << endl;
        cout << PrintTab(loop.loop_depth) << "Block_" << loop.cond_begin->GetBlockIdx() << endl;
        cout << PrintTab(loop.loop_depth) << "Block_" << loop.cond_end->GetBlockIdx() << endl;
        cout << PrintTab(loop.loop_depth) << "Block_" << loop.loop_exit->GetBlockIdx() << endl;
    }
    for (auto &&sub_loop : loop.sub_loops) {
        PrintLoop(*sub_loop);
    }
}
