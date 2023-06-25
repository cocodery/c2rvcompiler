#pragma once

#include <fstream>
#include <list>
#include <memory>

#include "3tle3wa/backend/asm/env.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/utils/Logs.hh"

class code_gen {
    asm_env env_;

    std::fstream fs_;

    CompilationUnit &comp_unit_;

   public:
    code_gen(const char *path, CompilationUnit &comp_unit);
    ~code_gen();

    /**
     * @brief 生成 env
     *
     */
    void gen_env();

    /**
     * @brief 生成 asm 文件
     *
     */
    void gen_asm();
};