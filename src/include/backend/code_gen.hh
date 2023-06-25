#pragma once

#include <fstream>
#include <list>
#include <memory>

#include "asm/env.hh"
#include "debug/Logs.hh"
#include "ir/IR.hh"

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