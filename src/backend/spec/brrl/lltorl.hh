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

    void loadparam();
    void combine();
    void loadimm();
    
    void mvret();
    void ireidx();
    void rmx();

    void genstk();
    bool assign();

    void pinfo();
    void pir();

    void gencode();

   public:
    rl_lltorl_instance(NormalFunction *func, ABBProg *prog);

    void operator()();
};