#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

#include "SysYBaseVisitor.h"
#include "SupportFile.hh"
#include "IR.hh"

using std::cout;
using std::endl;

class AstVisitor : public SysYBaseVisitor {
// private variable for parsing AST and building llvm-ir
private:
    CompilationUnit &comp_unit;

    bool have_main_func;

    bool in_function;
    bool in_loop;

    TypeID cur_type;
public:
    AstVisitor(CompilationUnit &);
    
    virtual antlrcpp::Any visitCompilationUnit(SysYParser::CompilationUnitContext *ctx) override;
// override anltr parsing function
private:
    virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree *ctx) override;
    
    virtual antlrcpp::Any visitTranslationUnit(SysYParser::TranslationUnitContext *ctx) override;
    
    virtual antlrcpp::Any visitGlobalDecl(SysYParser::GlobalDeclContext *ctx) override;

    virtual antlrcpp::Any visitGlobalFunc(SysYParser::GlobalFuncContext *ctx) override;
    
    virtual antlrcpp::Any visitStray(SysYParser::StrayContext *ctx) override;

    virtual antlrcpp::Any visitDecl(SysYParser::DeclContext *ctx) override;

    virtual antlrcpp::Any visitBType(SysYParser::BTypeContext *ctx) override;

    virtual antlrcpp::Any visitConstDecl(SysYParser::ConstDeclContext *ctx) override;

    virtual antlrcpp::Any visitConstDef(SysYParser::ConstDefContext *ctx) override;

    virtual antlrcpp::Any visitScalarConstInitVal(SysYParser::ScalarConstInitValContext *ctx) override;

    virtual antlrcpp::Any visitListConstInitVal(SysYParser::ListConstInitValContext *ctx) override;

    virtual antlrcpp::Any visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) override;

    virtual antlrcpp::Any visitNumber1(SysYParser::Number1Context *ctx) override;

    virtual antlrcpp::Any visitNumber2(SysYParser::Number2Context *ctx) override;

    virtual antlrcpp::Any visitUnary1(SysYParser::Unary1Context *ctx) override;

    virtual antlrcpp::Any visitMulExp(SysYParser::MulExpContext *ctx) override;

    virtual antlrcpp::Any visitMulOp(SysYParser::MulOpContext *ctx) override;

    virtual antlrcpp::Any visitAddExp(SysYParser::AddExpContext *ctx) override;

    virtual antlrcpp::Any visitAddOp(SysYParser::AddOpContext *ctx) override;

    virtual antlrcpp::Any visitShiftExp(SysYParser::ShiftExpContext *ctx) override;

    virtual antlrcpp::Any visitShiftOp(SysYParser::ShiftOpContext *ctx) override;

    virtual antlrcpp::Any visitRelExp(SysYParser::RelExpContext *ctx) override;

    virtual antlrcpp::Any visitRelOp(SysYParser::RelOpContext *ctx) override;

    virtual antlrcpp::Any visitEqExp(SysYParser::EqExpContext *ctx) override;

    virtual antlrcpp::Any visitEqOp(SysYParser::EqOpContext *ctx) override;

    virtual antlrcpp::Any visitAndExp(SysYParser::AndExpContext *ctx) override;

    virtual antlrcpp::Any visitExOrExp(SysYParser::ExOrExpContext *ctx) override;

    virtual antlrcpp::Any visitInOrExp(SysYParser::InOrExpContext *ctx) override;

    virtual antlrcpp::Any visitLAndExp(SysYParser::LAndExpContext *ctx) override;

    virtual antlrcpp::Any visitLOrExp(SysYParser::LOrExpContext *ctx) override;

    virtual antlrcpp::Any visitCondExp(SysYParser::CondExpContext *ctx) override;

    virtual antlrcpp::Any visitConstExp(SysYParser::ConstExpContext *ctx) override;
    
// support function to parse
    ArrDims getArrDims(std::vector<SysYParser::ConstExpContext *> &);
};
