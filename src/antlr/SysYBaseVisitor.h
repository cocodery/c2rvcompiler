
// Generated from SysY.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "SysYVisitor.h"


/**
 * This class provides an empty implementation of SysYVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  SysYBaseVisitor : public SysYVisitor {
public:

  virtual antlrcpp::Any visitCompilationUnit(SysYParser::CompilationUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTranslationUnit(SysYParser::TranslationUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExternalDeclaration(SysYParser::ExternalDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDecl(SysYParser::DeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstDecl(SysYParser::ConstDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBType(SysYParser::BTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstDef(SysYParser::ConstDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitScalarConstInitVal(SysYParser::ScalarConstInitValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitListConstInitVal(SysYParser::ListConstInitValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarDecl(SysYParser::VarDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUninitVarDef(SysYParser::UninitVarDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitInitVarDef(SysYParser::InitVarDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitScalarInitVal(SysYParser::ScalarInitValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitListInitval(SysYParser::ListInitvalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncDef(SysYParser::FuncDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncType(SysYParser::FuncTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncFParam(SysYParser::FuncFParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlock(SysYParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlockItemList(SysYParser::BlockItemListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlockItem(SysYParser::BlockItemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmt(SysYParser::StmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAssignStmt(SysYParser::AssignStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAssignOp(SysYParser::AssignOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExpStmt(SysYParser::ExpStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIfStmt(SysYParser::IfStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitWhileLoop(SysYParser::WhileLoopContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDoWhileLoop(SysYParser::DoWhileLoopContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitContinueStmt(SysYParser::ContinueStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBreakStmt(SysYParser::BreakStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExp(SysYParser::ExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstExp(SysYParser::ConstExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLVal(SysYParser::LValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryExp1(SysYParser::PrimaryExp1Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryExp2(SysYParser::PrimaryExp2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNumber1(SysYParser::Number1Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNumber2(SysYParser::Number2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncRParam(SysYParser::FuncRParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnary1(SysYParser::Unary1Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnary2(SysYParser::Unary2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnary3(SysYParser::Unary3Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnaryOp(SysYParser::UnaryOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMulExp(SysYParser::MulExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMulOp(SysYParser::MulOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAddExp(SysYParser::AddExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAddOp(SysYParser::AddOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitShiftExp(SysYParser::ShiftExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitShiftOp(SysYParser::ShiftOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRelExp(SysYParser::RelExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRelOp(SysYParser::RelOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEqExp(SysYParser::EqExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEqOp(SysYParser::EqOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAndExp(SysYParser::AndExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExOrExp(SysYParser::ExOrExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitInOrExp(SysYParser::InOrExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLAndExp(SysYParser::LAndExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLOrExp(SysYParser::LOrExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCondExp(SysYParser::CondExpContext *ctx) override {
    return visitChildren(ctx);
  }


};

