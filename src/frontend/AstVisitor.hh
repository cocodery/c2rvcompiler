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
    BlockPtr cur_block;
    FunctionPtr cur_func;
    
    SymbolTable *cur_table;
    SymTableList table_list;
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

    virtual antlrcpp::Any visitVarDecl(SysYParser::VarDeclContext *ctx) override;

    virtual antlrcpp::Any visitUninitVarDef(SysYParser::UninitVarDefContext *ctx) override;

    virtual antlrcpp::Any visitInitVarDef(SysYParser::InitVarDefContext *ctx) override;

    virtual antlrcpp::Any visitScalarInitVal(SysYParser::ScalarInitValContext *ctx) override;

    virtual antlrcpp::Any visitListInitval(SysYParser::ListInitvalContext *ctx) override;
    
    virtual antlrcpp::Any visitFuncDef(SysYParser::FuncDefContext *ctx) override;
    
    virtual antlrcpp::Any visitFuncType(SysYParser::FuncTypeContext *ctx) override;
    
    virtual antlrcpp::Any visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override;
    
    virtual antlrcpp::Any visitFuncFParam(SysYParser::FuncFParamContext *ctx) override;

    virtual antlrcpp::Any visitBlock(SysYParser::BlockContext *ctx) override;

    virtual antlrcpp::Any visitBlockItemList(SysYParser::BlockItemListContext *ctx) override;

    virtual antlrcpp::Any visitMemoryDecl(SysYParser::MemoryDeclContext *ctx) override;

    virtual antlrcpp::Any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override;

    virtual antlrcpp::Any visitExp(SysYParser::ExpContext *ctx) override;

    virtual antlrcpp::Any visitLVal(SysYParser::LValContext *ctx) override;

    virtual antlrcpp::Any visitPrimaryExp1(SysYParser::PrimaryExp1Context *ctx) override;

    virtual antlrcpp::Any visitPrimaryExp2(SysYParser::PrimaryExp2Context *ctx) override;

    virtual antlrcpp::Any visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) override;

    virtual antlrcpp::Any visitNumber1(SysYParser::Number1Context *ctx) override;

    virtual antlrcpp::Any visitNumber2(SysYParser::Number2Context *ctx) override;

    virtual antlrcpp::Any visitUnary1(SysYParser::Unary1Context *ctx) override;

    virtual antlrcpp::Any visitUnary3(SysYParser::Unary3Context *ctx) override;

    virtual antlrcpp::Any visitUnaryOp(SysYParser::UnaryOpContext *ctx) override;

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

    [[ deprecated ]] BaseValuePtr parseConstListInit(SysYParser::ListConstInitValContext *, ArrDims &);
    [[ deprecated ]] BaseValuePtr parseGlbVarListInit(SysYParser::ListInitvalContext *, ArrDims &);

    SymbolTable *newLocalTable(SymbolTable *);
    void clearTableList();

    BaseValuePtr resolveTable(std::string &name);
};
