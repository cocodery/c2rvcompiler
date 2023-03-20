
// Generated from SysY.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"




class  SysYParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    Int = 21, Void = 22, Const = 23, Return = 24, If = 25, Else = 26, While = 27, 
    For = 28, Do = 29, Break = 30, Continue = 31, Lparen = 32, Rparen = 33, 
    Lbrkt = 34, Rbrkt = 35, Lbrace = 36, Rbrace = 37, Comma = 38, Semicolon = 39, 
    Minus = 40, Addition = 41, Exclamation = 42, Multiplication = 43, Division = 44, 
    Modulo = 45, LAND = 46, LOR = 47, EQ = 48, NEQ = 49, LT = 50, LE = 51, 
    GT = 52, GE = 53, IntLiteral = 54, FloatLiteral = 55, Identifier = 56, 
    Whitespace = 57, Newline = 58, BlockComment = 59, LineComment = 60
  };

  enum {
    RuleCompilationUnit = 0, RuleTranslationUnit = 1, RuleExternalDeclaration = 2, 
    RuleDecl = 3, RuleConstDecl = 4, RuleBType = 5, RuleConstDef = 6, RuleConstInitVal = 7, 
    RuleVarDecl = 8, RuleVarDef = 9, RuleInitVal = 10, RuleFuncDef = 11, 
    RuleFuncType = 12, RuleFuncFParams = 13, RuleFuncFParam = 14, RuleBlock = 15, 
    RuleBlockItemList = 16, RuleBlockItem = 17, RuleStmt = 18, RuleAssignOp = 19, 
    RuleExpStmt = 20, RuleSeleStmt = 21, RuleLoopStmt = 22, RuleJumpStmt = 23, 
    RuleExp = 24, RuleConstExp = 25, RuleLVal = 26, RulePrimaryExp = 27, 
    RuleNumber = 28, RuleFuncRParams = 29, RuleFuncRParam = 30, RuleUnaryExp = 31, 
    RuleUnaryOp = 32, RuleMulExp = 33, RuleMulOp = 34, RuleAddExp = 35, 
    RuleAddOp = 36, RuleShiftExp = 37, RuleShiftOp = 38, RuleRelExp = 39, 
    RuleRelOp = 40, RuleEqExp = 41, RuleEqOp = 42, RuleAndExp = 43, RuleExOrExp = 44, 
    RuleInOrExp = 45, RuleLAndExp = 46, RuleLOrExp = 47, RuleCondExp = 48
  };

  SysYParser(antlr4::TokenStream *input);
  ~SysYParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class CompilationUnitContext;
  class TranslationUnitContext;
  class ExternalDeclarationContext;
  class DeclContext;
  class ConstDeclContext;
  class BTypeContext;
  class ConstDefContext;
  class ConstInitValContext;
  class VarDeclContext;
  class VarDefContext;
  class InitValContext;
  class FuncDefContext;
  class FuncTypeContext;
  class FuncFParamsContext;
  class FuncFParamContext;
  class BlockContext;
  class BlockItemListContext;
  class BlockItemContext;
  class StmtContext;
  class AssignOpContext;
  class ExpStmtContext;
  class SeleStmtContext;
  class LoopStmtContext;
  class JumpStmtContext;
  class ExpContext;
  class ConstExpContext;
  class LValContext;
  class PrimaryExpContext;
  class NumberContext;
  class FuncRParamsContext;
  class FuncRParamContext;
  class UnaryExpContext;
  class UnaryOpContext;
  class MulExpContext;
  class MulOpContext;
  class AddExpContext;
  class AddOpContext;
  class ShiftExpContext;
  class ShiftOpContext;
  class RelExpContext;
  class RelOpContext;
  class EqExpContext;
  class EqOpContext;
  class AndExpContext;
  class ExOrExpContext;
  class InOrExpContext;
  class LAndExpContext;
  class LOrExpContext;
  class CondExpContext; 

  class  CompilationUnitContext : public antlr4::ParserRuleContext {
  public:
    CompilationUnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    TranslationUnitContext *translationUnit();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CompilationUnitContext* compilationUnit();

  class  TranslationUnitContext : public antlr4::ParserRuleContext {
  public:
    TranslationUnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExternalDeclarationContext *> externalDeclaration();
    ExternalDeclarationContext* externalDeclaration(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TranslationUnitContext* translationUnit();

  class  ExternalDeclarationContext : public antlr4::ParserRuleContext {
  public:
    ExternalDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ExternalDeclarationContext() = default;
    void copyFrom(ExternalDeclarationContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  StrayContext : public ExternalDeclarationContext {
  public:
    StrayContext(ExternalDeclarationContext *ctx);

    antlr4::tree::TerminalNode *Semicolon();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  GlobalDeclContext : public ExternalDeclarationContext {
  public:
    GlobalDeclContext(ExternalDeclarationContext *ctx);

    DeclContext *decl();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  GlobalFuncContext : public ExternalDeclarationContext {
  public:
    GlobalFuncContext(ExternalDeclarationContext *ctx);

    FuncDefContext *funcDef();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ExternalDeclarationContext* externalDeclaration();

  class  DeclContext : public antlr4::ParserRuleContext {
  public:
    DeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ConstDeclContext *constDecl();
    VarDeclContext *varDecl();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DeclContext* decl();

  class  ConstDeclContext : public antlr4::ParserRuleContext {
  public:
    ConstDeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Const();
    BTypeContext *bType();
    std::vector<ConstDefContext *> constDef();
    ConstDefContext* constDef(size_t i);
    antlr4::tree::TerminalNode *Semicolon();
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstDeclContext* constDecl();

  class  BTypeContext : public antlr4::ParserRuleContext {
  public:
    BTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Int();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BTypeContext* bType();

  class  ConstDefContext : public antlr4::ParserRuleContext {
  public:
    ConstDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Identifier();
    ConstInitValContext *constInitVal();
    std::vector<antlr4::tree::TerminalNode *> Lbrkt();
    antlr4::tree::TerminalNode* Lbrkt(size_t i);
    std::vector<ConstExpContext *> constExp();
    ConstExpContext* constExp(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Rbrkt();
    antlr4::tree::TerminalNode* Rbrkt(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstDefContext* constDef();

  class  ConstInitValContext : public antlr4::ParserRuleContext {
  public:
    ConstInitValContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ConstInitValContext() = default;
    void copyFrom(ConstInitValContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ListConstInitValContext : public ConstInitValContext {
  public:
    ListConstInitValContext(ConstInitValContext *ctx);

    antlr4::tree::TerminalNode *Lbrace();
    antlr4::tree::TerminalNode *Rbrace();
    std::vector<ConstInitValContext *> constInitVal();
    ConstInitValContext* constInitVal(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ScalarConstInitValContext : public ConstInitValContext {
  public:
    ScalarConstInitValContext(ConstInitValContext *ctx);

    ConstExpContext *constExp();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ConstInitValContext* constInitVal();

  class  VarDeclContext : public antlr4::ParserRuleContext {
  public:
    VarDeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BTypeContext *bType();
    std::vector<VarDefContext *> varDef();
    VarDefContext* varDef(size_t i);
    antlr4::tree::TerminalNode *Semicolon();
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VarDeclContext* varDecl();

  class  VarDefContext : public antlr4::ParserRuleContext {
  public:
    VarDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    VarDefContext() = default;
    void copyFrom(VarDefContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  UninitVarDefContext : public VarDefContext {
  public:
    UninitVarDefContext(VarDefContext *ctx);

    antlr4::tree::TerminalNode *Identifier();
    std::vector<antlr4::tree::TerminalNode *> Lbrkt();
    antlr4::tree::TerminalNode* Lbrkt(size_t i);
    std::vector<ConstExpContext *> constExp();
    ConstExpContext* constExp(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Rbrkt();
    antlr4::tree::TerminalNode* Rbrkt(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  InitVarDefContext : public VarDefContext {
  public:
    InitVarDefContext(VarDefContext *ctx);

    antlr4::tree::TerminalNode *Identifier();
    InitValContext *initVal();
    std::vector<antlr4::tree::TerminalNode *> Lbrkt();
    antlr4::tree::TerminalNode* Lbrkt(size_t i);
    std::vector<ConstExpContext *> constExp();
    ConstExpContext* constExp(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Rbrkt();
    antlr4::tree::TerminalNode* Rbrkt(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  VarDefContext* varDef();

  class  InitValContext : public antlr4::ParserRuleContext {
  public:
    InitValContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    InitValContext() = default;
    void copyFrom(InitValContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ScalarInitValContext : public InitValContext {
  public:
    ScalarInitValContext(InitValContext *ctx);

    ExpContext *exp();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ListInitvalContext : public InitValContext {
  public:
    ListInitvalContext(InitValContext *ctx);

    antlr4::tree::TerminalNode *Lbrace();
    antlr4::tree::TerminalNode *Rbrace();
    std::vector<InitValContext *> initVal();
    InitValContext* initVal(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  InitValContext* initVal();

  class  FuncDefContext : public antlr4::ParserRuleContext {
  public:
    FuncDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FuncTypeContext *funcType();
    antlr4::tree::TerminalNode *Identifier();
    antlr4::tree::TerminalNode *Lparen();
    antlr4::tree::TerminalNode *Rparen();
    BlockContext *block();
    FuncFParamsContext *funcFParams();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncDefContext* funcDef();

  class  FuncTypeContext : public antlr4::ParserRuleContext {
  public:
    FuncTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Void();
    antlr4::tree::TerminalNode *Int();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncTypeContext* funcType();

  class  FuncFParamsContext : public antlr4::ParserRuleContext {
  public:
    FuncFParamsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<FuncFParamContext *> funcFParam();
    FuncFParamContext* funcFParam(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncFParamsContext* funcFParams();

  class  FuncFParamContext : public antlr4::ParserRuleContext {
  public:
    FuncFParamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BTypeContext *bType();
    antlr4::tree::TerminalNode *Identifier();
    std::vector<antlr4::tree::TerminalNode *> Lbrkt();
    antlr4::tree::TerminalNode* Lbrkt(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Rbrkt();
    antlr4::tree::TerminalNode* Rbrkt(size_t i);
    std::vector<ConstExpContext *> constExp();
    ConstExpContext* constExp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncFParamContext* funcFParam();

  class  BlockContext : public antlr4::ParserRuleContext {
  public:
    BlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Lbrace();
    antlr4::tree::TerminalNode *Rbrace();
    BlockItemListContext *blockItemList();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockContext* block();

  class  BlockItemListContext : public antlr4::ParserRuleContext {
  public:
    BlockItemListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<BlockItemContext *> blockItem();
    BlockItemContext* blockItem(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockItemListContext* blockItemList();

  class  BlockItemContext : public antlr4::ParserRuleContext {
  public:
    BlockItemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    BlockItemContext() = default;
    void copyFrom(BlockItemContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  MemoryDeclContext : public BlockItemContext {
  public:
    MemoryDeclContext(BlockItemContext *ctx);

    DeclContext *decl();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  BlockStmtContext : public BlockItemContext {
  public:
    BlockStmtContext(BlockItemContext *ctx);

    StmtContext *stmt();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  BlockItemContext* blockItem();

  class  StmtContext : public antlr4::ParserRuleContext {
  public:
    StmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LValContext *lVal();
    AssignOpContext *assignOp();
    ExpContext *exp();
    antlr4::tree::TerminalNode *Semicolon();
    ExpStmtContext *expStmt();
    BlockContext *block();
    SeleStmtContext *seleStmt();
    LoopStmtContext *loopStmt();
    JumpStmtContext *jumpStmt();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StmtContext* stmt();

  class  AssignOpContext : public antlr4::ParserRuleContext {
  public:
    AssignOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AssignOpContext* assignOp();

  class  ExpStmtContext : public antlr4::ParserRuleContext {
  public:
    ExpStmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Semicolon();
    ExpContext *exp();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpStmtContext* expStmt();

  class  SeleStmtContext : public antlr4::ParserRuleContext {
  public:
    SeleStmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    SeleStmtContext() = default;
    void copyFrom(SeleStmtContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  IfStmt1Context : public SeleStmtContext {
  public:
    IfStmt1Context(SeleStmtContext *ctx);

    antlr4::tree::TerminalNode *If();
    antlr4::tree::TerminalNode *Lparen();
    CondExpContext *condExp();
    antlr4::tree::TerminalNode *Rparen();
    StmtContext *stmt();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  IfStmt2Context : public SeleStmtContext {
  public:
    IfStmt2Context(SeleStmtContext *ctx);

    antlr4::tree::TerminalNode *If();
    antlr4::tree::TerminalNode *Lparen();
    CondExpContext *condExp();
    antlr4::tree::TerminalNode *Rparen();
    std::vector<StmtContext *> stmt();
    StmtContext* stmt(size_t i);
    antlr4::tree::TerminalNode *Else();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  SeleStmtContext* seleStmt();

  class  LoopStmtContext : public antlr4::ParserRuleContext {
  public:
    LoopStmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    LoopStmtContext() = default;
    void copyFrom(LoopStmtContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  WhileLoopContext : public LoopStmtContext {
  public:
    WhileLoopContext(LoopStmtContext *ctx);

    antlr4::tree::TerminalNode *While();
    antlr4::tree::TerminalNode *Lparen();
    CondExpContext *condExp();
    antlr4::tree::TerminalNode *Rparen();
    StmtContext *stmt();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  DoWhileLoopContext : public LoopStmtContext {
  public:
    DoWhileLoopContext(LoopStmtContext *ctx);

    antlr4::tree::TerminalNode *Do();
    StmtContext *stmt();
    antlr4::tree::TerminalNode *While();
    antlr4::tree::TerminalNode *Lparen();
    CondExpContext *condExp();
    antlr4::tree::TerminalNode *Rparen();
    antlr4::tree::TerminalNode *Semicolon();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  LoopStmtContext* loopStmt();

  class  JumpStmtContext : public antlr4::ParserRuleContext {
  public:
    JumpStmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    JumpStmtContext() = default;
    void copyFrom(JumpStmtContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  BreakStmtContext : public JumpStmtContext {
  public:
    BreakStmtContext(JumpStmtContext *ctx);

    antlr4::tree::TerminalNode *Break();
    antlr4::tree::TerminalNode *Semicolon();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ReturnStmtContext : public JumpStmtContext {
  public:
    ReturnStmtContext(JumpStmtContext *ctx);

    antlr4::tree::TerminalNode *Return();
    antlr4::tree::TerminalNode *Semicolon();
    ExpContext *exp();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ContinueStmtContext : public JumpStmtContext {
  public:
    ContinueStmtContext(JumpStmtContext *ctx);

    antlr4::tree::TerminalNode *Continue();
    antlr4::tree::TerminalNode *Semicolon();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  JumpStmtContext* jumpStmt();

  class  ExpContext : public antlr4::ParserRuleContext {
  public:
    ExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CondExpContext *condExp();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpContext* exp();

  class  ConstExpContext : public antlr4::ParserRuleContext {
  public:
    ConstExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CondExpContext *condExp();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstExpContext* constExp();

  class  LValContext : public antlr4::ParserRuleContext {
  public:
    LValContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Identifier();
    std::vector<antlr4::tree::TerminalNode *> Lbrkt();
    antlr4::tree::TerminalNode* Lbrkt(size_t i);
    std::vector<ExpContext *> exp();
    ExpContext* exp(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Rbrkt();
    antlr4::tree::TerminalNode* Rbrkt(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LValContext* lVal();

  class  PrimaryExpContext : public antlr4::ParserRuleContext {
  public:
    PrimaryExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    PrimaryExpContext() = default;
    void copyFrom(PrimaryExpContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PrimaryExp2Context : public PrimaryExpContext {
  public:
    PrimaryExp2Context(PrimaryExpContext *ctx);

    LValContext *lVal();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryExp1Context : public PrimaryExpContext {
  public:
    PrimaryExp1Context(PrimaryExpContext *ctx);

    antlr4::tree::TerminalNode *Lparen();
    ExpContext *exp();
    antlr4::tree::TerminalNode *Rparen();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PrimaryExp3Context : public PrimaryExpContext {
  public:
    PrimaryExp3Context(PrimaryExpContext *ctx);

    NumberContext *number();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  PrimaryExpContext* primaryExp();

  class  NumberContext : public antlr4::ParserRuleContext {
  public:
    NumberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    NumberContext() = default;
    void copyFrom(NumberContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  Number1Context : public NumberContext {
  public:
    Number1Context(NumberContext *ctx);

    antlr4::tree::TerminalNode *IntLiteral();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  Number2Context : public NumberContext {
  public:
    Number2Context(NumberContext *ctx);

    antlr4::tree::TerminalNode *FloatLiteral();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  NumberContext* number();

  class  FuncRParamsContext : public antlr4::ParserRuleContext {
  public:
    FuncRParamsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<FuncRParamContext *> funcRParam();
    FuncRParamContext* funcRParam(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncRParamsContext* funcRParams();

  class  FuncRParamContext : public antlr4::ParserRuleContext {
  public:
    FuncRParamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpContext *exp();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncRParamContext* funcRParam();

  class  UnaryExpContext : public antlr4::ParserRuleContext {
  public:
    UnaryExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    UnaryExpContext() = default;
    void copyFrom(UnaryExpContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  Unary1Context : public UnaryExpContext {
  public:
    Unary1Context(UnaryExpContext *ctx);

    PrimaryExpContext *primaryExp();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  Unary2Context : public UnaryExpContext {
  public:
    Unary2Context(UnaryExpContext *ctx);

    antlr4::tree::TerminalNode *Identifier();
    antlr4::tree::TerminalNode *Lparen();
    antlr4::tree::TerminalNode *Rparen();
    FuncRParamsContext *funcRParams();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  Unary3Context : public UnaryExpContext {
  public:
    Unary3Context(UnaryExpContext *ctx);

    UnaryOpContext *unaryOp();
    UnaryExpContext *unaryExp();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  UnaryExpContext* unaryExp();

  class  UnaryOpContext : public antlr4::ParserRuleContext {
  public:
    UnaryOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Multiplication();
    antlr4::tree::TerminalNode *Addition();
    antlr4::tree::TerminalNode *Minus();
    antlr4::tree::TerminalNode *Exclamation();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UnaryOpContext* unaryOp();

  class  MulExpContext : public antlr4::ParserRuleContext {
  public:
    MulExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<UnaryExpContext *> unaryExp();
    UnaryExpContext* unaryExp(size_t i);
    std::vector<MulOpContext *> mulOp();
    MulOpContext* mulOp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MulExpContext* mulExp();

  class  MulOpContext : public antlr4::ParserRuleContext {
  public:
    MulOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Multiplication();
    antlr4::tree::TerminalNode *Division();
    antlr4::tree::TerminalNode *Modulo();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MulOpContext* mulOp();

  class  AddExpContext : public antlr4::ParserRuleContext {
  public:
    AddExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<MulExpContext *> mulExp();
    MulExpContext* mulExp(size_t i);
    std::vector<AddOpContext *> addOp();
    AddOpContext* addOp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AddExpContext* addExp();

  class  AddOpContext : public antlr4::ParserRuleContext {
  public:
    AddOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Addition();
    antlr4::tree::TerminalNode *Minus();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AddOpContext* addOp();

  class  ShiftExpContext : public antlr4::ParserRuleContext {
  public:
    ShiftExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AddExpContext *> addExp();
    AddExpContext* addExp(size_t i);
    std::vector<ShiftOpContext *> shiftOp();
    ShiftOpContext* shiftOp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ShiftExpContext* shiftExp();

  class  ShiftOpContext : public antlr4::ParserRuleContext {
  public:
    ShiftOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ShiftOpContext* shiftOp();

  class  RelExpContext : public antlr4::ParserRuleContext {
  public:
    RelExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ShiftExpContext *> shiftExp();
    ShiftExpContext* shiftExp(size_t i);
    std::vector<RelOpContext *> relOp();
    RelOpContext* relOp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RelExpContext* relExp();

  class  RelOpContext : public antlr4::ParserRuleContext {
  public:
    RelOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LT();
    antlr4::tree::TerminalNode *GT();
    antlr4::tree::TerminalNode *LE();
    antlr4::tree::TerminalNode *GE();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RelOpContext* relOp();

  class  EqExpContext : public antlr4::ParserRuleContext {
  public:
    EqExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<RelExpContext *> relExp();
    RelExpContext* relExp(size_t i);
    std::vector<EqOpContext *> eqOp();
    EqOpContext* eqOp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EqExpContext* eqExp();

  class  EqOpContext : public antlr4::ParserRuleContext {
  public:
    EqOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQ();
    antlr4::tree::TerminalNode *NEQ();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EqOpContext* eqOp();

  class  AndExpContext : public antlr4::ParserRuleContext {
  public:
    AndExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<EqExpContext *> eqExp();
    EqExpContext* eqExp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AndExpContext* andExp();

  class  ExOrExpContext : public antlr4::ParserRuleContext {
  public:
    ExOrExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AndExpContext *> andExp();
    AndExpContext* andExp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExOrExpContext* exOrExp();

  class  InOrExpContext : public antlr4::ParserRuleContext {
  public:
    InOrExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExOrExpContext *> exOrExp();
    ExOrExpContext* exOrExp(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InOrExpContext* inOrExp();

  class  LAndExpContext : public antlr4::ParserRuleContext {
  public:
    LAndExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<InOrExpContext *> inOrExp();
    InOrExpContext* inOrExp(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LAND();
    antlr4::tree::TerminalNode* LAND(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LAndExpContext* lAndExp();

  class  LOrExpContext : public antlr4::ParserRuleContext {
  public:
    LOrExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<LAndExpContext *> lAndExp();
    LAndExpContext* lAndExp(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LOR();
    antlr4::tree::TerminalNode* LOR(size_t i);


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LOrExpContext* lOrExp();

  class  CondExpContext : public antlr4::ParserRuleContext {
  public:
    CondExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LOrExpContext *lOrExp();
    ExpContext *exp();
    CondExpContext *condExp();


    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CondExpContext* condExp();


private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

