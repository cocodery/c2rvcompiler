#include <iostream>
#include <string>
#include <fstream>

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include "SysYLexer.h"
#include "SysYParser.h"
#include "AstVisitor.hh"

using namespace antlr4;
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    int opt = 0;
    bool print_usage = false;
    std::string input, output, irfile;

    for (int ch; (ch = getopt(argc, argv, "Sl:o:O:h")) != -1;) {
        switch (ch) {
            case 'l':
                irfile = strdup(optarg);
                break;
            case 'o':
                output = strdup(optarg);
                break;
            case 'O':
                opt = atoi(optarg);
                break;
            case 'h':
                print_usage = true; 
                break;
            default:
                break;
        }
    }

    if (optind <= argc) {
        input = argv[optind];
    }

    std::ifstream src(input);
    if (!src.is_open()) {
        std::cerr << "line " <<  __LINE__ << ": cannot open input file \"" << input << "\"" << endl;
        return EXIT_FAILURE;
    }

    ANTLRInputStream source(src);
    SysYLexer lexer(&source);
    CommonTokenStream tokens(&lexer);
    SysYParser parser(&tokens);
    parser.setErrorHandler(std::make_shared<BailErrorStrategy>());

    SysYParser::CompilationUnitContext *root = parser.compilationUnit();

    CompilationUnit comp_unit;
    AstVisitor visitor(comp_unit);

    visitor.visitCompilationUnit(root);

    comp_unit.printGlbTable();
    cout << endl;
    comp_unit.printFuncTalbe();

    return 0;
}