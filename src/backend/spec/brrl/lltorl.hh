#pragma once

#include "../../genabb/genabb.hh"
#include "../../utils.hh"
#include "rlbbtype.hh"

class rl_lltorl_instance {
    NormalFunction *func_;
    ABBProg *prog_;
    
    std::unique_ptr<rl_progress> rlp;

    std::string tpfx;

    size_t mxpsiz = 0;

    void torl();

    void ireidx();
    void rmx();

    void loadparam();

    void pinfo();
    void pir();

   public:
    rl_lltorl_instance(NormalFunction *func, ABBProg *prog);

    void operator()();
};