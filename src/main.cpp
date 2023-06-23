#include <fcntl.h>
#include <unistd.h>

#include <Logs.hh>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "AstVisitor.hh"
#include "Pass.hh"
#include "SysYLexer.h"
#include "SysYParser.h"
#include "code_gen.hh"

using namespace antlr4;
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    [[maybe_unused]] char opt = 0;
    [[maybe_unused]] bool print_usage = false;

    const char *input = nullptr;
    [[maybe_unused]] const char *output = nullptr;
    [[maybe_unused]] const char *irfile = nullptr;

    for (int ch; (ch = getopt(argc, argv, "Sl:o:O:h")) != -1;) {
        switch (ch) {
            case 'l':
                Log("output llvm-ir");
                irfile = optarg;
                break;
            case 'o':
                Log("output filename: %s", optarg);
                output = optarg;
                break;
            case 'O':
                Log("optimize level: %c", *optarg);
                opt = *optarg;
                break;
            case 'h':
                print_usage = true;
                break;
            default:
                break;
        }
    }

    if (print_usage) {
        /* TODO: fill it */
        return 0;
    }

    if (optind < argc) {
        input = argv[optind];
    } else {
        panic("no input files");
    }

    std::ifstream src(input);
    if (!src.is_open()) {
        std::cerr << "line " << __LINE__ << ": cannot open input file \"" << input << "\"" << endl;
        return EXIT_FAILURE;
    }

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

    // Optimization optimizer(comp_unit);
    // optimizer.DoOptimization();

    if (irfile) {
        comp_unit.generatellvmIR(irfile);
    }

    if (output) {
        code_gen cg(output, comp_unit);
        cg.gen_env();
        cg.gen_asm();
    }

    return 0;
}