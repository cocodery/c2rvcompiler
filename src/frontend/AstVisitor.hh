#pragma once

#include <iostream>
#include <list>
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

    bool in_loop;
    BlockPtr out_loop_block;

    VariablePtr ret_addr;
    BlockPtr ret_block;

    ATTR_TYPE cur_type;
    ATTR_POSITION cur_position;
    
    BlockPtr cur_block;

    NormalFuncPtr cur_func;
    BaseFuncPtr callee_func;

    using SymTableList = std::list<SymbolTable *>;
    SymbolTable *cur_table;
    SymTableList table_list;

    using RetInstList = std::list<JumpInstPtr>;
    RetInstList return_list;

    BlockPtr target_continue;

    using BreakInstList = std::list<JumpInstPtr>;
    BreakInstList break_list;

    using BranchInstList = std::list<BranchInstPtr>;
    BranchInstList lOr_list;
    BranchInstList lAnd_list;

    using AddrTypeMap = std::unordered_map<BaseValuePtr, ListTypePtr>;
    AddrTypeMap addrTypeTable;
public:
    AstVisitor(CompilationUnit &);
    ~AstVisitor() = default;
    
    virtual antlrcpp::Any visitCompilationUnit(SysYParser::CompilationUnitContext *ctx) override;
// override anltr parsing function
private:
    virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree *ctx) override;
    
    virtual antlrcpp::Any visitTranslationUnit(SysYParser::TranslationUnitContext *ctx) override;

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

    virtual antlrcpp::Any visitBlockItem(SysYParser::BlockItemContext *ctx) override;

    virtual antlrcpp::Any visitStmt(SysYParser::StmtContext *ctx) override;

    virtual antlrcpp::Any visitAssignStmt(SysYParser::AssignStmtContext *ctx) override;

    virtual antlrcpp::Any visitAssignOp(SysYParser::AssignOpContext *ctx) override;

    virtual antlrcpp::Any visitExpStmt(SysYParser::ExpStmtContext *ctx) override;

    virtual antlrcpp::Any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override;

    virtual antlrcpp::Any visitIfStmt(SysYParser::IfStmtContext *ctx) override;

    virtual antlrcpp::Any visitWhileLoop(SysYParser::WhileLoopContext *ctx) override;

    virtual antlrcpp::Any visitContinueStmt(SysYParser::ContinueStmtContext *ctx) override;

    virtual antlrcpp::Any visitBreakStmt(SysYParser::BreakStmtContext *ctx) override;

    virtual antlrcpp::Any visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override;

    virtual antlrcpp::Any visitExp(SysYParser::ExpContext *ctx) override;

    virtual antlrcpp::Any visitLVal(SysYParser::LValContext *ctx) override;

    virtual antlrcpp::Any visitPrimaryExp1(SysYParser::PrimaryExp1Context *ctx) override;

    virtual antlrcpp::Any visitPrimaryExp2(SysYParser::PrimaryExp2Context *ctx) override;

    virtual antlrcpp::Any visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) override;

    virtual antlrcpp::Any visitNumber1(SysYParser::Number1Context *ctx) override;

    virtual antlrcpp::Any visitNumber2(SysYParser::Number2Context *ctx) override;

    virtual antlrcpp::Any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override;

    virtual antlrcpp::Any visitFuncRParam(SysYParser::FuncRParamContext *ctx) override;

    virtual antlrcpp::Any visitUnary1(SysYParser::Unary1Context *ctx) override;

    virtual antlrcpp::Any visitUnary2(SysYParser::Unary2Context *ctx) override;

    virtual antlrcpp::Any visitUnary3(SysYParser::Unary3Context *ctx) override;

    virtual antlrcpp::Any visitUnaryOp(SysYParser::UnaryOpContext *ctx) override;

    virtual antlrcpp::Any visitMulExp(SysYParser::MulExpContext *ctx) override;

    virtual antlrcpp::Any visitMulOp(SysYParser::MulOpContext *ctx) override;

    virtual antlrcpp::Any visitAddExp(SysYParser::AddExpContext *ctx) override;

    virtual antlrcpp::Any visitAddOp(SysYParser::AddOpContext *ctx) override;

    virtual antlrcpp::Any visitRelExp(SysYParser::RelExpContext *ctx) override;

    virtual antlrcpp::Any visitRelOp(SysYParser::RelOpContext *ctx) override;

    virtual antlrcpp::Any visitEqExp(SysYParser::EqExpContext *ctx) override;

    virtual antlrcpp::Any visitEqOp(SysYParser::EqOpContext *ctx) override;

    virtual antlrcpp::Any visitLAnd1(SysYParser::LAnd1Context *ctx) override;

    virtual antlrcpp::Any visitLAnd2(SysYParser::LAnd2Context *ctx) override;

    virtual antlrcpp::Any visitLOr1(SysYParser::LOr1Context *ctx) override;

    virtual antlrcpp::Any visitLOr2(SysYParser::LOr2Context *ctx) override;

    virtual antlrcpp::Any visitCondExp(SysYParser::CondExpContext *ctx) override;

    virtual antlrcpp::Any visitConstExp(SysYParser::ConstExpContext *ctx) override;
    
// support function to parse
    ArrDims getArrDims(std::vector<SysYParser::ConstExpContext *> &);

    [[ deprecated ]] BaseValuePtr parseConstListInit(SysYParser::ListConstInitValContext *, ArrDims &);
    [[ deprecated ]] BaseValuePtr parseGlbVarListInit(SysYParser::ListInitvalContext *, ArrDims &);

    SymbolTable *newLocalTable(SymbolTable *);
    void clearTableList();

    BaseValuePtr resolveTable(std::string &name);

    SymbolTable *initParamList(BlockPtr, SymbolTable *, std::vector<std::string>);

    void parseLocalListInit(SysYParser::ListInitvalContext *, ListTypePtr, BaseValuePtr, BlockPtr);
};
