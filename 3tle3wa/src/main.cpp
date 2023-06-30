#include <fcntl.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "3tle3wa/backend/code_gen.hh"
#include "3tle3wa/frontend/AstVisitor.hh"
#include "3tle3wa/pass/Pass.hh"
#include "3tle3wa/utils/Logs.hh"
#include "SysYLexer.h"
#include "SysYParser.h"

using namespace antlr4;
using std::cout;
using std::endl;

char *dbgfile = nullptr;
char optlvl = 0;

int main(int argc, char *argv[]) {
    bool pusage = false;

    const char *input = nullptr;
    const char *output = nullptr;
    const char *irfile = nullptr;

    for (int ch; (ch = getopt(argc, argv, "SO:l:o:d:h")) != -1;) {
        switch (ch) {
            case 'l':
                Log("llvm ir filename: %s", optarg);
                irfile = optarg;
                break;
            case 'o':
                Log("rv asm filename: %s", optarg);
                output = optarg;
                break;
            case 'h':
                pusage = true;
                break;
            case 'd':
                Log("debug filename: %s", optarg);
                dbgfile = optarg;
                break;
            case 'O':
                Log("optimize level: %c", *optarg);
                optlvl = *optarg;
                break;
            case 'S':
                Log("output asm");
                break;
            default:
                break;
        }
    }

    if (pusage) {
        std::cout << "Usage: compiler [options] <file>" << std::endl;
        std::cout << "only receive one file as input" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -d <file>   output dbg file" << std::endl;
        std::cout << "  -l <file>   output llvm ir" << std::endl;
        std::cout << "  -o <file>   output rv asm" << std::endl;
        std::cout << "  -h          this help" << std::endl;
        return 0;
    }

    if (optind < argc) {
        input = argv[optind];
    } else {
        panic("no input file");
    }

    std::ifstream src(input);

    Assert(src.is_open(), "cannot open input file %s", input);

    if (dbgfile) {
        std::fstream dbgf(dbgfile, std::ios::out);
        Assert(dbgf.is_open(), "cannot open input file %s", dbgfile);

        dbgf << "Debug File" << std::endl;
        dbgf.close();
    }

    std::unique_ptr<asm_env> asmgen = nullptr;

    do {
        ANTLRInputStream source(src);
        SysYLexer lexer(&source);
        CommonTokenStream tokens(&lexer);
        SysYParser parser(&tokens);
        parser.setErrorHandler(std::make_shared<BailErrorStrategy>());

        SysYParser::CompilationUnitContext *root = parser.compilationUnit();

        CompilationUnit comp_unit;

        std::unique_ptr<AstVisitor> visitor = std::make_unique<AstVisitor>(comp_unit);
        visitor->visitCompilationUnit(root);
        visitor = nullptr;

        Optimization optimizer(comp_unit);
        optimizer.DoOptimization();

        if (irfile) {
            comp_unit.generatellvmIR(irfile);
        }

        if (output) {
            code_gen cg(comp_unit);
            cg.gen_env();
            asmgen = std::move(cg.exports());
        }
    } while (0);

    extern const char *skip;

    if (output and asmgen != nullptr) {
        std::fstream fs(output, std::ios::out);
        asmgen->do_optimize();
        asmgen->gen_asm(fs);
    }

    return 0;
}