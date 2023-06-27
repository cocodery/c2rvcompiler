#pragma once

#include <fstream>
#include <list>
#include <memory>

#include "3tle3wa/backend/asm/env.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/utils/Logs.hh"

class code_gen {
    std::unique_ptr<asm_env> env_;

    std::fstream fs_;

    CompilationUnit &comp_unit_;

   public:
    code_gen(CompilationUnit &comp_unit);
    ~code_gen();

    /**
     * @brief 生成 env
     *
     */
    void gen_env();

    std::unique_ptr<asm_env> &exports();
};