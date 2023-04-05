
// Generated from SysY.g4 by ANTLR 4.8


#include "SysYVisitor.h"

#include "SysYParser.h"


using namespace antlrcpp;
using namespace antlr4;

SysYParser::SysYParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

SysYParser::~SysYParser() {
  delete _interpreter;
}

std::string SysYParser::getGrammarFileName() const {
  return "SysY.g4";
}

const std::vector<std::string>& SysYParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& SysYParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- CompilationUnitContext ------------------------------------------------------------------

SysYParser::CompilationUnitContext::CompilationUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::CompilationUnitContext::EOF() {
  return getToken(SysYParser::EOF, 0);
}

SysYParser::TranslationUnitContext* SysYParser::CompilationUnitContext::translationUnit() {
  return getRuleContext<SysYParser::TranslationUnitContext>(0);
}


size_t SysYParser::CompilationUnitContext::getRuleIndex() const {
  return SysYParser::RuleCompilationUnit;
}


antlrcpp::Any SysYParser::CompilationUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitCompilationUnit(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::CompilationUnitContext* SysYParser::compilationUnit() {
  CompilationUnitContext *_localctx = _tracker.createInstance<CompilationUnitContext>(_ctx, getState());
  enterRule(_localctx, 0, SysYParser::RuleCompilationUnit);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(103);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::T__0)
      | (1ULL << SysYParser::Int)
      | (1ULL << SysYParser::Void)
      | (1ULL << SysYParser::Const)
      | (1ULL << SysYParser::Semicolon))) != 0)) {
      setState(102);
      translationUnit();
    }
    setState(105);
    match(SysYParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TranslationUnitContext ------------------------------------------------------------------

SysYParser::TranslationUnitContext::TranslationUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::ExternalDeclarationContext *> SysYParser::TranslationUnitContext::externalDeclaration() {
  return getRuleContexts<SysYParser::ExternalDeclarationContext>();
}

SysYParser::ExternalDeclarationContext* SysYParser::TranslationUnitContext::externalDeclaration(size_t i) {
  return getRuleContext<SysYParser::ExternalDeclarationContext>(i);
}


size_t SysYParser::TranslationUnitContext::getRuleIndex() const {
  return SysYParser::RuleTranslationUnit;
}


antlrcpp::Any SysYParser::TranslationUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitTranslationUnit(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::TranslationUnitContext* SysYParser::translationUnit() {
  TranslationUnitContext *_localctx = _tracker.createInstance<TranslationUnitContext>(_ctx, getState());
  enterRule(_localctx, 2, SysYParser::RuleTranslationUnit);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(108); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(107);
      externalDeclaration();
      setState(110); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::T__0)
      | (1ULL << SysYParser::Int)
      | (1ULL << SysYParser::Void)
      | (1ULL << SysYParser::Const)
      | (1ULL << SysYParser::Semicolon))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExternalDeclarationContext ------------------------------------------------------------------

SysYParser::ExternalDeclarationContext::ExternalDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::DeclContext* SysYParser::ExternalDeclarationContext::decl() {
  return getRuleContext<SysYParser::DeclContext>(0);
}

SysYParser::FuncDefContext* SysYParser::ExternalDeclarationContext::funcDef() {
  return getRuleContext<SysYParser::FuncDefContext>(0);
}

tree::TerminalNode* SysYParser::ExternalDeclarationContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}


size_t SysYParser::ExternalDeclarationContext::getRuleIndex() const {
  return SysYParser::RuleExternalDeclaration;
}


antlrcpp::Any SysYParser::ExternalDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitExternalDeclaration(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ExternalDeclarationContext* SysYParser::externalDeclaration() {
  ExternalDeclarationContext *_localctx = _tracker.createInstance<ExternalDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 4, SysYParser::RuleExternalDeclaration);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(115);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(112);
      decl();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(113);
      funcDef();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(114);
      match(SysYParser::Semicolon);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclContext ------------------------------------------------------------------

SysYParser::DeclContext::DeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ConstDeclContext* SysYParser::DeclContext::constDecl() {
  return getRuleContext<SysYParser::ConstDeclContext>(0);
}

SysYParser::VarDeclContext* SysYParser::DeclContext::varDecl() {
  return getRuleContext<SysYParser::VarDeclContext>(0);
}


size_t SysYParser::DeclContext::getRuleIndex() const {
  return SysYParser::RuleDecl;
}


antlrcpp::Any SysYParser::DeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::DeclContext* SysYParser::decl() {
  DeclContext *_localctx = _tracker.createInstance<DeclContext>(_ctx, getState());
  enterRule(_localctx, 6, SysYParser::RuleDecl);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(119);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::Const: {
        enterOuterAlt(_localctx, 1);
        setState(117);
        constDecl();
        break;
      }

      case SysYParser::T__0:
      case SysYParser::Int: {
        enterOuterAlt(_localctx, 2);
        setState(118);
        varDecl();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstDeclContext ------------------------------------------------------------------

SysYParser::ConstDeclContext::ConstDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ConstDeclContext::Const() {
  return getToken(SysYParser::Const, 0);
}

SysYParser::BTypeContext* SysYParser::ConstDeclContext::bType() {
  return getRuleContext<SysYParser::BTypeContext>(0);
}

std::vector<SysYParser::ConstDefContext *> SysYParser::ConstDeclContext::constDef() {
  return getRuleContexts<SysYParser::ConstDefContext>();
}

SysYParser::ConstDefContext* SysYParser::ConstDeclContext::constDef(size_t i) {
  return getRuleContext<SysYParser::ConstDefContext>(i);
}

tree::TerminalNode* SysYParser::ConstDeclContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}

std::vector<tree::TerminalNode *> SysYParser::ConstDeclContext::Comma() {
  return getTokens(SysYParser::Comma);
}

tree::TerminalNode* SysYParser::ConstDeclContext::Comma(size_t i) {
  return getToken(SysYParser::Comma, i);
}


size_t SysYParser::ConstDeclContext::getRuleIndex() const {
  return SysYParser::RuleConstDecl;
}


antlrcpp::Any SysYParser::ConstDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitConstDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ConstDeclContext* SysYParser::constDecl() {
  ConstDeclContext *_localctx = _tracker.createInstance<ConstDeclContext>(_ctx, getState());
  enterRule(_localctx, 8, SysYParser::RuleConstDecl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(121);
    match(SysYParser::Const);
    setState(122);
    bType();
    setState(123);
    constDef();
    setState(128);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::Comma) {
      setState(124);
      match(SysYParser::Comma);
      setState(125);
      constDef();
      setState(130);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(131);
    match(SysYParser::Semicolon);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BTypeContext ------------------------------------------------------------------

SysYParser::BTypeContext::BTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BTypeContext::Int() {
  return getToken(SysYParser::Int, 0);
}


size_t SysYParser::BTypeContext::getRuleIndex() const {
  return SysYParser::RuleBType;
}


antlrcpp::Any SysYParser::BTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBType(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BTypeContext* SysYParser::bType() {
  BTypeContext *_localctx = _tracker.createInstance<BTypeContext>(_ctx, getState());
  enterRule(_localctx, 10, SysYParser::RuleBType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(133);
    _la = _input->LA(1);
    if (!(_la == SysYParser::T__0

    || _la == SysYParser::Int)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstDefContext ------------------------------------------------------------------

SysYParser::ConstDefContext::ConstDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ConstDefContext::Identifier() {
  return getToken(SysYParser::Identifier, 0);
}

SysYParser::ConstInitValContext* SysYParser::ConstDefContext::constInitVal() {
  return getRuleContext<SysYParser::ConstInitValContext>(0);
}

std::vector<tree::TerminalNode *> SysYParser::ConstDefContext::Lbrkt() {
  return getTokens(SysYParser::Lbrkt);
}

tree::TerminalNode* SysYParser::ConstDefContext::Lbrkt(size_t i) {
  return getToken(SysYParser::Lbrkt, i);
}

std::vector<SysYParser::ConstExpContext *> SysYParser::ConstDefContext::constExp() {
  return getRuleContexts<SysYParser::ConstExpContext>();
}

SysYParser::ConstExpContext* SysYParser::ConstDefContext::constExp(size_t i) {
  return getRuleContext<SysYParser::ConstExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::ConstDefContext::Rbrkt() {
  return getTokens(SysYParser::Rbrkt);
}

tree::TerminalNode* SysYParser::ConstDefContext::Rbrkt(size_t i) {
  return getToken(SysYParser::Rbrkt, i);
}


size_t SysYParser::ConstDefContext::getRuleIndex() const {
  return SysYParser::RuleConstDef;
}


antlrcpp::Any SysYParser::ConstDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitConstDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ConstDefContext* SysYParser::constDef() {
  ConstDefContext *_localctx = _tracker.createInstance<ConstDefContext>(_ctx, getState());
  enterRule(_localctx, 12, SysYParser::RuleConstDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(135);
    match(SysYParser::Identifier);
    setState(142);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::Lbrkt) {
      setState(136);
      match(SysYParser::Lbrkt);
      setState(137);
      constExp();
      setState(138);
      match(SysYParser::Rbrkt);
      setState(144);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(145);
    match(SysYParser::T__1);
    setState(146);
    constInitVal();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstInitValContext ------------------------------------------------------------------

SysYParser::ConstInitValContext::ConstInitValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::ConstInitValContext::getRuleIndex() const {
  return SysYParser::RuleConstInitVal;
}

void SysYParser::ConstInitValContext::copyFrom(ConstInitValContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ListConstInitValContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::ListConstInitValContext::Lbrace() {
  return getToken(SysYParser::Lbrace, 0);
}

tree::TerminalNode* SysYParser::ListConstInitValContext::Rbrace() {
  return getToken(SysYParser::Rbrace, 0);
}

std::vector<SysYParser::ConstInitValContext *> SysYParser::ListConstInitValContext::constInitVal() {
  return getRuleContexts<SysYParser::ConstInitValContext>();
}

SysYParser::ConstInitValContext* SysYParser::ListConstInitValContext::constInitVal(size_t i) {
  return getRuleContext<SysYParser::ConstInitValContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::ListConstInitValContext::Comma() {
  return getTokens(SysYParser::Comma);
}

tree::TerminalNode* SysYParser::ListConstInitValContext::Comma(size_t i) {
  return getToken(SysYParser::Comma, i);
}

SysYParser::ListConstInitValContext::ListConstInitValContext(ConstInitValContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::ListConstInitValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitListConstInitVal(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ScalarConstInitValContext ------------------------------------------------------------------

SysYParser::ConstExpContext* SysYParser::ScalarConstInitValContext::constExp() {
  return getRuleContext<SysYParser::ConstExpContext>(0);
}

SysYParser::ScalarConstInitValContext::ScalarConstInitValContext(ConstInitValContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::ScalarConstInitValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitScalarConstInitVal(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::ConstInitValContext* SysYParser::constInitVal() {
  ConstInitValContext *_localctx = _tracker.createInstance<ConstInitValContext>(_ctx, getState());
  enterRule(_localctx, 14, SysYParser::RuleConstInitVal);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(161);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::T__12:
      case SysYParser::T__13:
      case SysYParser::Lparen:
      case SysYParser::Minus:
      case SysYParser::Addition:
      case SysYParser::Exclamation:
      case SysYParser::Multiplication:
      case SysYParser::IntLiteral:
      case SysYParser::FloatLiteral:
      case SysYParser::Identifier: {
        _localctx = dynamic_cast<ConstInitValContext *>(_tracker.createInstance<SysYParser::ScalarConstInitValContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(148);
        constExp();
        break;
      }

      case SysYParser::Lbrace: {
        _localctx = dynamic_cast<ConstInitValContext *>(_tracker.createInstance<SysYParser::ListConstInitValContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(149);
        match(SysYParser::Lbrace);
        setState(158);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParser::T__12)
          | (1ULL << SysYParser::T__13)
          | (1ULL << SysYParser::Lparen)
          | (1ULL << SysYParser::Lbrace)
          | (1ULL << SysYParser::Minus)
          | (1ULL << SysYParser::Addition)
          | (1ULL << SysYParser::Exclamation)
          | (1ULL << SysYParser::Multiplication)
          | (1ULL << SysYParser::IntLiteral)
          | (1ULL << SysYParser::FloatLiteral)
          | (1ULL << SysYParser::Identifier))) != 0)) {
          setState(150);
          constInitVal();
          setState(155);
          _errHandler->sync(this);
          _la = _input->LA(1);
          while (_la == SysYParser::Comma) {
            setState(151);
            match(SysYParser::Comma);
            setState(152);
            constInitVal();
            setState(157);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
        }
        setState(160);
        match(SysYParser::Rbrace);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDeclContext ------------------------------------------------------------------

SysYParser::VarDeclContext::VarDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::BTypeContext* SysYParser::VarDeclContext::bType() {
  return getRuleContext<SysYParser::BTypeContext>(0);
}

std::vector<SysYParser::VarDefContext *> SysYParser::VarDeclContext::varDef() {
  return getRuleContexts<SysYParser::VarDefContext>();
}

SysYParser::VarDefContext* SysYParser::VarDeclContext::varDef(size_t i) {
  return getRuleContext<SysYParser::VarDefContext>(i);
}

tree::TerminalNode* SysYParser::VarDeclContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}

std::vector<tree::TerminalNode *> SysYParser::VarDeclContext::Comma() {
  return getTokens(SysYParser::Comma);
}

tree::TerminalNode* SysYParser::VarDeclContext::Comma(size_t i) {
  return getToken(SysYParser::Comma, i);
}


size_t SysYParser::VarDeclContext::getRuleIndex() const {
  return SysYParser::RuleVarDecl;
}


antlrcpp::Any SysYParser::VarDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitVarDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::VarDeclContext* SysYParser::varDecl() {
  VarDeclContext *_localctx = _tracker.createInstance<VarDeclContext>(_ctx, getState());
  enterRule(_localctx, 16, SysYParser::RuleVarDecl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(163);
    bType();
    setState(164);
    varDef();
    setState(169);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::Comma) {
      setState(165);
      match(SysYParser::Comma);
      setState(166);
      varDef();
      setState(171);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(172);
    match(SysYParser::Semicolon);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDefContext ------------------------------------------------------------------

SysYParser::VarDefContext::VarDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::VarDefContext::getRuleIndex() const {
  return SysYParser::RuleVarDef;
}

void SysYParser::VarDefContext::copyFrom(VarDefContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- UninitVarDefContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::UninitVarDefContext::Identifier() {
  return getToken(SysYParser::Identifier, 0);
}

std::vector<tree::TerminalNode *> SysYParser::UninitVarDefContext::Lbrkt() {
  return getTokens(SysYParser::Lbrkt);
}

tree::TerminalNode* SysYParser::UninitVarDefContext::Lbrkt(size_t i) {
  return getToken(SysYParser::Lbrkt, i);
}

std::vector<SysYParser::ConstExpContext *> SysYParser::UninitVarDefContext::constExp() {
  return getRuleContexts<SysYParser::ConstExpContext>();
}

SysYParser::ConstExpContext* SysYParser::UninitVarDefContext::constExp(size_t i) {
  return getRuleContext<SysYParser::ConstExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::UninitVarDefContext::Rbrkt() {
  return getTokens(SysYParser::Rbrkt);
}

tree::TerminalNode* SysYParser::UninitVarDefContext::Rbrkt(size_t i) {
  return getToken(SysYParser::Rbrkt, i);
}

SysYParser::UninitVarDefContext::UninitVarDefContext(VarDefContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::UninitVarDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitUninitVarDef(this);
  else
    return visitor->visitChildren(this);
}
//----------------- InitVarDefContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::InitVarDefContext::Identifier() {
  return getToken(SysYParser::Identifier, 0);
}

SysYParser::InitValContext* SysYParser::InitVarDefContext::initVal() {
  return getRuleContext<SysYParser::InitValContext>(0);
}

std::vector<tree::TerminalNode *> SysYParser::InitVarDefContext::Lbrkt() {
  return getTokens(SysYParser::Lbrkt);
}

tree::TerminalNode* SysYParser::InitVarDefContext::Lbrkt(size_t i) {
  return getToken(SysYParser::Lbrkt, i);
}

std::vector<SysYParser::ConstExpContext *> SysYParser::InitVarDefContext::constExp() {
  return getRuleContexts<SysYParser::ConstExpContext>();
}

SysYParser::ConstExpContext* SysYParser::InitVarDefContext::constExp(size_t i) {
  return getRuleContext<SysYParser::ConstExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::InitVarDefContext::Rbrkt() {
  return getTokens(SysYParser::Rbrkt);
}

tree::TerminalNode* SysYParser::InitVarDefContext::Rbrkt(size_t i) {
  return getToken(SysYParser::Rbrkt, i);
}

SysYParser::InitVarDefContext::InitVarDefContext(VarDefContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::InitVarDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitInitVarDef(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::VarDefContext* SysYParser::varDef() {
  VarDefContext *_localctx = _tracker.createInstance<VarDefContext>(_ctx, getState());
  enterRule(_localctx, 18, SysYParser::RuleVarDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(196);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      _localctx = dynamic_cast<VarDefContext *>(_tracker.createInstance<SysYParser::UninitVarDefContext>(_localctx));
      enterOuterAlt(_localctx, 1);
      setState(174);
      match(SysYParser::Identifier);
      setState(181);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::Lbrkt) {
        setState(175);
        match(SysYParser::Lbrkt);
        setState(176);
        constExp();
        setState(177);
        match(SysYParser::Rbrkt);
        setState(183);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      break;
    }

    case 2: {
      _localctx = dynamic_cast<VarDefContext *>(_tracker.createInstance<SysYParser::InitVarDefContext>(_localctx));
      enterOuterAlt(_localctx, 2);
      setState(184);
      match(SysYParser::Identifier);
      setState(191);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::Lbrkt) {
        setState(185);
        match(SysYParser::Lbrkt);
        setState(186);
        constExp();
        setState(187);
        match(SysYParser::Rbrkt);
        setState(193);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(194);
      match(SysYParser::T__1);
      setState(195);
      initVal();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InitValContext ------------------------------------------------------------------

SysYParser::InitValContext::InitValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::InitValContext::getRuleIndex() const {
  return SysYParser::RuleInitVal;
}

void SysYParser::InitValContext::copyFrom(InitValContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ScalarInitValContext ------------------------------------------------------------------

SysYParser::ExpContext* SysYParser::ScalarInitValContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

SysYParser::ScalarInitValContext::ScalarInitValContext(InitValContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::ScalarInitValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitScalarInitVal(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ListInitvalContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::ListInitvalContext::Lbrace() {
  return getToken(SysYParser::Lbrace, 0);
}

tree::TerminalNode* SysYParser::ListInitvalContext::Rbrace() {
  return getToken(SysYParser::Rbrace, 0);
}

std::vector<SysYParser::InitValContext *> SysYParser::ListInitvalContext::initVal() {
  return getRuleContexts<SysYParser::InitValContext>();
}

SysYParser::InitValContext* SysYParser::ListInitvalContext::initVal(size_t i) {
  return getRuleContext<SysYParser::InitValContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::ListInitvalContext::Comma() {
  return getTokens(SysYParser::Comma);
}

tree::TerminalNode* SysYParser::ListInitvalContext::Comma(size_t i) {
  return getToken(SysYParser::Comma, i);
}

SysYParser::ListInitvalContext::ListInitvalContext(InitValContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::ListInitvalContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitListInitval(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::InitValContext* SysYParser::initVal() {
  InitValContext *_localctx = _tracker.createInstance<InitValContext>(_ctx, getState());
  enterRule(_localctx, 20, SysYParser::RuleInitVal);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(211);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::T__12:
      case SysYParser::T__13:
      case SysYParser::Lparen:
      case SysYParser::Minus:
      case SysYParser::Addition:
      case SysYParser::Exclamation:
      case SysYParser::Multiplication:
      case SysYParser::IntLiteral:
      case SysYParser::FloatLiteral:
      case SysYParser::Identifier: {
        _localctx = dynamic_cast<InitValContext *>(_tracker.createInstance<SysYParser::ScalarInitValContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(198);
        exp();
        break;
      }

      case SysYParser::Lbrace: {
        _localctx = dynamic_cast<InitValContext *>(_tracker.createInstance<SysYParser::ListInitvalContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(199);
        match(SysYParser::Lbrace);
        setState(208);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParser::T__12)
          | (1ULL << SysYParser::T__13)
          | (1ULL << SysYParser::Lparen)
          | (1ULL << SysYParser::Lbrace)
          | (1ULL << SysYParser::Minus)
          | (1ULL << SysYParser::Addition)
          | (1ULL << SysYParser::Exclamation)
          | (1ULL << SysYParser::Multiplication)
          | (1ULL << SysYParser::IntLiteral)
          | (1ULL << SysYParser::FloatLiteral)
          | (1ULL << SysYParser::Identifier))) != 0)) {
          setState(200);
          initVal();
          setState(205);
          _errHandler->sync(this);
          _la = _input->LA(1);
          while (_la == SysYParser::Comma) {
            setState(201);
            match(SysYParser::Comma);
            setState(202);
            initVal();
            setState(207);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
        }
        setState(210);
        match(SysYParser::Rbrace);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncDefContext ------------------------------------------------------------------

SysYParser::FuncDefContext::FuncDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::FuncTypeContext* SysYParser::FuncDefContext::funcType() {
  return getRuleContext<SysYParser::FuncTypeContext>(0);
}

tree::TerminalNode* SysYParser::FuncDefContext::Identifier() {
  return getToken(SysYParser::Identifier, 0);
}

tree::TerminalNode* SysYParser::FuncDefContext::Lparen() {
  return getToken(SysYParser::Lparen, 0);
}

tree::TerminalNode* SysYParser::FuncDefContext::Rparen() {
  return getToken(SysYParser::Rparen, 0);
}

SysYParser::BlockContext* SysYParser::FuncDefContext::block() {
  return getRuleContext<SysYParser::BlockContext>(0);
}

SysYParser::FuncFParamsContext* SysYParser::FuncDefContext::funcFParams() {
  return getRuleContext<SysYParser::FuncFParamsContext>(0);
}


size_t SysYParser::FuncDefContext::getRuleIndex() const {
  return SysYParser::RuleFuncDef;
}


antlrcpp::Any SysYParser::FuncDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncDefContext* SysYParser::funcDef() {
  FuncDefContext *_localctx = _tracker.createInstance<FuncDefContext>(_ctx, getState());
  enterRule(_localctx, 22, SysYParser::RuleFuncDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(213);
    funcType();
    setState(214);
    match(SysYParser::Identifier);
    setState(215);
    match(SysYParser::Lparen);
    setState(217);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::T__0

    || _la == SysYParser::Int) {
      setState(216);
      funcFParams();
    }
    setState(219);
    match(SysYParser::Rparen);
    setState(220);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncTypeContext ------------------------------------------------------------------

SysYParser::FuncTypeContext::FuncTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::FuncTypeContext::Void() {
  return getToken(SysYParser::Void, 0);
}

tree::TerminalNode* SysYParser::FuncTypeContext::Int() {
  return getToken(SysYParser::Int, 0);
}


size_t SysYParser::FuncTypeContext::getRuleIndex() const {
  return SysYParser::RuleFuncType;
}


antlrcpp::Any SysYParser::FuncTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncType(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncTypeContext* SysYParser::funcType() {
  FuncTypeContext *_localctx = _tracker.createInstance<FuncTypeContext>(_ctx, getState());
  enterRule(_localctx, 24, SysYParser::RuleFuncType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(222);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::T__0)
      | (1ULL << SysYParser::Int)
      | (1ULL << SysYParser::Void))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncFParamsContext ------------------------------------------------------------------

SysYParser::FuncFParamsContext::FuncFParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::FuncFParamContext *> SysYParser::FuncFParamsContext::funcFParam() {
  return getRuleContexts<SysYParser::FuncFParamContext>();
}

SysYParser::FuncFParamContext* SysYParser::FuncFParamsContext::funcFParam(size_t i) {
  return getRuleContext<SysYParser::FuncFParamContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamsContext::Comma() {
  return getTokens(SysYParser::Comma);
}

tree::TerminalNode* SysYParser::FuncFParamsContext::Comma(size_t i) {
  return getToken(SysYParser::Comma, i);
}


size_t SysYParser::FuncFParamsContext::getRuleIndex() const {
  return SysYParser::RuleFuncFParams;
}


antlrcpp::Any SysYParser::FuncFParamsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncFParams(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncFParamsContext* SysYParser::funcFParams() {
  FuncFParamsContext *_localctx = _tracker.createInstance<FuncFParamsContext>(_ctx, getState());
  enterRule(_localctx, 26, SysYParser::RuleFuncFParams);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(224);
    funcFParam();
    setState(229);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::Comma) {
      setState(225);
      match(SysYParser::Comma);
      setState(226);
      funcFParam();
      setState(231);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncFParamContext ------------------------------------------------------------------

SysYParser::FuncFParamContext::FuncFParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::BTypeContext* SysYParser::FuncFParamContext::bType() {
  return getRuleContext<SysYParser::BTypeContext>(0);
}

tree::TerminalNode* SysYParser::FuncFParamContext::Identifier() {
  return getToken(SysYParser::Identifier, 0);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamContext::Lbrkt() {
  return getTokens(SysYParser::Lbrkt);
}

tree::TerminalNode* SysYParser::FuncFParamContext::Lbrkt(size_t i) {
  return getToken(SysYParser::Lbrkt, i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamContext::Rbrkt() {
  return getTokens(SysYParser::Rbrkt);
}

tree::TerminalNode* SysYParser::FuncFParamContext::Rbrkt(size_t i) {
  return getToken(SysYParser::Rbrkt, i);
}

std::vector<SysYParser::ConstExpContext *> SysYParser::FuncFParamContext::constExp() {
  return getRuleContexts<SysYParser::ConstExpContext>();
}

SysYParser::ConstExpContext* SysYParser::FuncFParamContext::constExp(size_t i) {
  return getRuleContext<SysYParser::ConstExpContext>(i);
}


size_t SysYParser::FuncFParamContext::getRuleIndex() const {
  return SysYParser::RuleFuncFParam;
}


antlrcpp::Any SysYParser::FuncFParamContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncFParam(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncFParamContext* SysYParser::funcFParam() {
  FuncFParamContext *_localctx = _tracker.createInstance<FuncFParamContext>(_ctx, getState());
  enterRule(_localctx, 28, SysYParser::RuleFuncFParam);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(232);
    bType();
    setState(233);
    match(SysYParser::Identifier);
    setState(245);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::Lbrkt) {
      setState(234);
      match(SysYParser::Lbrkt);
      setState(235);
      match(SysYParser::Rbrkt);
      setState(242);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::Lbrkt) {
        setState(236);
        match(SysYParser::Lbrkt);
        setState(237);
        constExp();
        setState(238);
        match(SysYParser::Rbrkt);
        setState(244);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

SysYParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BlockContext::Lbrace() {
  return getToken(SysYParser::Lbrace, 0);
}

tree::TerminalNode* SysYParser::BlockContext::Rbrace() {
  return getToken(SysYParser::Rbrace, 0);
}

SysYParser::BlockItemListContext* SysYParser::BlockContext::blockItemList() {
  return getRuleContext<SysYParser::BlockItemListContext>(0);
}


size_t SysYParser::BlockContext::getRuleIndex() const {
  return SysYParser::RuleBlock;
}


antlrcpp::Any SysYParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockContext* SysYParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 30, SysYParser::RuleBlock);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(247);
    match(SysYParser::Lbrace);
    setState(249);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::T__0)
      | (1ULL << SysYParser::T__12)
      | (1ULL << SysYParser::T__13)
      | (1ULL << SysYParser::Int)
      | (1ULL << SysYParser::Const)
      | (1ULL << SysYParser::Return)
      | (1ULL << SysYParser::If)
      | (1ULL << SysYParser::While)
      | (1ULL << SysYParser::Do)
      | (1ULL << SysYParser::Break)
      | (1ULL << SysYParser::Continue)
      | (1ULL << SysYParser::Lparen)
      | (1ULL << SysYParser::Lbrace)
      | (1ULL << SysYParser::Semicolon)
      | (1ULL << SysYParser::Minus)
      | (1ULL << SysYParser::Addition)
      | (1ULL << SysYParser::Exclamation)
      | (1ULL << SysYParser::Multiplication)
      | (1ULL << SysYParser::IntLiteral)
      | (1ULL << SysYParser::FloatLiteral)
      | (1ULL << SysYParser::Identifier))) != 0)) {
      setState(248);
      blockItemList();
    }
    setState(251);
    match(SysYParser::Rbrace);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockItemListContext ------------------------------------------------------------------

SysYParser::BlockItemListContext::BlockItemListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::BlockItemContext *> SysYParser::BlockItemListContext::blockItem() {
  return getRuleContexts<SysYParser::BlockItemContext>();
}

SysYParser::BlockItemContext* SysYParser::BlockItemListContext::blockItem(size_t i) {
  return getRuleContext<SysYParser::BlockItemContext>(i);
}


size_t SysYParser::BlockItemListContext::getRuleIndex() const {
  return SysYParser::RuleBlockItemList;
}


antlrcpp::Any SysYParser::BlockItemListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBlockItemList(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockItemListContext* SysYParser::blockItemList() {
  BlockItemListContext *_localctx = _tracker.createInstance<BlockItemListContext>(_ctx, getState());
  enterRule(_localctx, 32, SysYParser::RuleBlockItemList);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(254); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(253);
      blockItem();
      setState(256); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::T__0)
      | (1ULL << SysYParser::T__12)
      | (1ULL << SysYParser::T__13)
      | (1ULL << SysYParser::Int)
      | (1ULL << SysYParser::Const)
      | (1ULL << SysYParser::Return)
      | (1ULL << SysYParser::If)
      | (1ULL << SysYParser::While)
      | (1ULL << SysYParser::Do)
      | (1ULL << SysYParser::Break)
      | (1ULL << SysYParser::Continue)
      | (1ULL << SysYParser::Lparen)
      | (1ULL << SysYParser::Lbrace)
      | (1ULL << SysYParser::Semicolon)
      | (1ULL << SysYParser::Minus)
      | (1ULL << SysYParser::Addition)
      | (1ULL << SysYParser::Exclamation)
      | (1ULL << SysYParser::Multiplication)
      | (1ULL << SysYParser::IntLiteral)
      | (1ULL << SysYParser::FloatLiteral)
      | (1ULL << SysYParser::Identifier))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockItemContext ------------------------------------------------------------------

SysYParser::BlockItemContext::BlockItemContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::DeclContext* SysYParser::BlockItemContext::decl() {
  return getRuleContext<SysYParser::DeclContext>(0);
}

SysYParser::StmtContext* SysYParser::BlockItemContext::stmt() {
  return getRuleContext<SysYParser::StmtContext>(0);
}


size_t SysYParser::BlockItemContext::getRuleIndex() const {
  return SysYParser::RuleBlockItem;
}


antlrcpp::Any SysYParser::BlockItemContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBlockItem(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockItemContext* SysYParser::blockItem() {
  BlockItemContext *_localctx = _tracker.createInstance<BlockItemContext>(_ctx, getState());
  enterRule(_localctx, 34, SysYParser::RuleBlockItem);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(260);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::T__0:
      case SysYParser::Int:
      case SysYParser::Const: {
        enterOuterAlt(_localctx, 1);
        setState(258);
        decl();
        break;
      }

      case SysYParser::T__12:
      case SysYParser::T__13:
      case SysYParser::Return:
      case SysYParser::If:
      case SysYParser::While:
      case SysYParser::Do:
      case SysYParser::Break:
      case SysYParser::Continue:
      case SysYParser::Lparen:
      case SysYParser::Lbrace:
      case SysYParser::Semicolon:
      case SysYParser::Minus:
      case SysYParser::Addition:
      case SysYParser::Exclamation:
      case SysYParser::Multiplication:
      case SysYParser::IntLiteral:
      case SysYParser::FloatLiteral:
      case SysYParser::Identifier: {
        enterOuterAlt(_localctx, 2);
        setState(259);
        stmt();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

SysYParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::AssignStmtContext* SysYParser::StmtContext::assignStmt() {
  return getRuleContext<SysYParser::AssignStmtContext>(0);
}

SysYParser::ExpStmtContext* SysYParser::StmtContext::expStmt() {
  return getRuleContext<SysYParser::ExpStmtContext>(0);
}

SysYParser::BlockStmtContext* SysYParser::StmtContext::blockStmt() {
  return getRuleContext<SysYParser::BlockStmtContext>(0);
}

SysYParser::SeleStmtContext* SysYParser::StmtContext::seleStmt() {
  return getRuleContext<SysYParser::SeleStmtContext>(0);
}

SysYParser::LoopStmtContext* SysYParser::StmtContext::loopStmt() {
  return getRuleContext<SysYParser::LoopStmtContext>(0);
}

SysYParser::JumpStmtContext* SysYParser::StmtContext::jumpStmt() {
  return getRuleContext<SysYParser::JumpStmtContext>(0);
}


size_t SysYParser::StmtContext::getRuleIndex() const {
  return SysYParser::RuleStmt;
}


antlrcpp::Any SysYParser::StmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::StmtContext* SysYParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 36, SysYParser::RuleStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(268);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(262);
      assignStmt();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(263);
      expStmt();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(264);
      blockStmt();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(265);
      seleStmt();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(266);
      loopStmt();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(267);
      jumpStmt();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignStmtContext ------------------------------------------------------------------

SysYParser::AssignStmtContext::AssignStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LValContext* SysYParser::AssignStmtContext::lVal() {
  return getRuleContext<SysYParser::LValContext>(0);
}

SysYParser::AssignOpContext* SysYParser::AssignStmtContext::assignOp() {
  return getRuleContext<SysYParser::AssignOpContext>(0);
}

SysYParser::ExpContext* SysYParser::AssignStmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::AssignStmtContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}


size_t SysYParser::AssignStmtContext::getRuleIndex() const {
  return SysYParser::RuleAssignStmt;
}


antlrcpp::Any SysYParser::AssignStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAssignStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::AssignStmtContext* SysYParser::assignStmt() {
  AssignStmtContext *_localctx = _tracker.createInstance<AssignStmtContext>(_ctx, getState());
  enterRule(_localctx, 38, SysYParser::RuleAssignStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(270);
    lVal();
    setState(271);
    assignOp();
    setState(272);
    exp();
    setState(273);
    match(SysYParser::Semicolon);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignOpContext ------------------------------------------------------------------

SysYParser::AssignOpContext::AssignOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::AssignOpContext::getRuleIndex() const {
  return SysYParser::RuleAssignOp;
}


antlrcpp::Any SysYParser::AssignOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAssignOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::AssignOpContext* SysYParser::assignOp() {
  AssignOpContext *_localctx = _tracker.createInstance<AssignOpContext>(_ctx, getState());
  enterRule(_localctx, 40, SysYParser::RuleAssignOp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(275);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::T__1)
      | (1ULL << SysYParser::T__2)
      | (1ULL << SysYParser::T__3)
      | (1ULL << SysYParser::T__4)
      | (1ULL << SysYParser::T__5)
      | (1ULL << SysYParser::T__6)
      | (1ULL << SysYParser::T__7)
      | (1ULL << SysYParser::T__8)
      | (1ULL << SysYParser::T__9)
      | (1ULL << SysYParser::T__10)
      | (1ULL << SysYParser::T__11))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpStmtContext ------------------------------------------------------------------

SysYParser::ExpStmtContext::ExpStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ExpStmtContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}

SysYParser::ExpContext* SysYParser::ExpStmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}


size_t SysYParser::ExpStmtContext::getRuleIndex() const {
  return SysYParser::RuleExpStmt;
}


antlrcpp::Any SysYParser::ExpStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitExpStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ExpStmtContext* SysYParser::expStmt() {
  ExpStmtContext *_localctx = _tracker.createInstance<ExpStmtContext>(_ctx, getState());
  enterRule(_localctx, 42, SysYParser::RuleExpStmt);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(278);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::T__12)
      | (1ULL << SysYParser::T__13)
      | (1ULL << SysYParser::Lparen)
      | (1ULL << SysYParser::Minus)
      | (1ULL << SysYParser::Addition)
      | (1ULL << SysYParser::Exclamation)
      | (1ULL << SysYParser::Multiplication)
      | (1ULL << SysYParser::IntLiteral)
      | (1ULL << SysYParser::FloatLiteral)
      | (1ULL << SysYParser::Identifier))) != 0)) {
      setState(277);
      exp();
    }
    setState(280);
    match(SysYParser::Semicolon);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockStmtContext ------------------------------------------------------------------

SysYParser::BlockStmtContext::BlockStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::BlockContext* SysYParser::BlockStmtContext::block() {
  return getRuleContext<SysYParser::BlockContext>(0);
}


size_t SysYParser::BlockStmtContext::getRuleIndex() const {
  return SysYParser::RuleBlockStmt;
}


antlrcpp::Any SysYParser::BlockStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBlockStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockStmtContext* SysYParser::blockStmt() {
  BlockStmtContext *_localctx = _tracker.createInstance<BlockStmtContext>(_ctx, getState());
  enterRule(_localctx, 44, SysYParser::RuleBlockStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(282);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SeleStmtContext ------------------------------------------------------------------

SysYParser::SeleStmtContext::SeleStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::SeleStmtContext::getRuleIndex() const {
  return SysYParser::RuleSeleStmt;
}

void SysYParser::SeleStmtContext::copyFrom(SeleStmtContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- IfStmtContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::IfStmtContext::If() {
  return getToken(SysYParser::If, 0);
}

tree::TerminalNode* SysYParser::IfStmtContext::Lparen() {
  return getToken(SysYParser::Lparen, 0);
}

SysYParser::CondExpContext* SysYParser::IfStmtContext::condExp() {
  return getRuleContext<SysYParser::CondExpContext>(0);
}

tree::TerminalNode* SysYParser::IfStmtContext::Rparen() {
  return getToken(SysYParser::Rparen, 0);
}

std::vector<SysYParser::StmtContext *> SysYParser::IfStmtContext::stmt() {
  return getRuleContexts<SysYParser::StmtContext>();
}

SysYParser::StmtContext* SysYParser::IfStmtContext::stmt(size_t i) {
  return getRuleContext<SysYParser::StmtContext>(i);
}

tree::TerminalNode* SysYParser::IfStmtContext::Else() {
  return getToken(SysYParser::Else, 0);
}

SysYParser::IfStmtContext::IfStmtContext(SeleStmtContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::IfStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitIfStmt(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::SeleStmtContext* SysYParser::seleStmt() {
  SeleStmtContext *_localctx = _tracker.createInstance<SeleStmtContext>(_ctx, getState());
  enterRule(_localctx, 46, SysYParser::RuleSeleStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    _localctx = dynamic_cast<SeleStmtContext *>(_tracker.createInstance<SysYParser::IfStmtContext>(_localctx));
    enterOuterAlt(_localctx, 1);
    setState(284);
    match(SysYParser::If);
    setState(285);
    match(SysYParser::Lparen);
    setState(286);
    condExp();
    setState(287);
    match(SysYParser::Rparen);
    setState(288);
    stmt();
    setState(291);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      setState(289);
      match(SysYParser::Else);
      setState(290);
      stmt();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LoopStmtContext ------------------------------------------------------------------

SysYParser::LoopStmtContext::LoopStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::LoopStmtContext::getRuleIndex() const {
  return SysYParser::RuleLoopStmt;
}

void SysYParser::LoopStmtContext::copyFrom(LoopStmtContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- WhileLoopContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::WhileLoopContext::While() {
  return getToken(SysYParser::While, 0);
}

tree::TerminalNode* SysYParser::WhileLoopContext::Lparen() {
  return getToken(SysYParser::Lparen, 0);
}

SysYParser::CondExpContext* SysYParser::WhileLoopContext::condExp() {
  return getRuleContext<SysYParser::CondExpContext>(0);
}

tree::TerminalNode* SysYParser::WhileLoopContext::Rparen() {
  return getToken(SysYParser::Rparen, 0);
}

SysYParser::StmtContext* SysYParser::WhileLoopContext::stmt() {
  return getRuleContext<SysYParser::StmtContext>(0);
}

SysYParser::WhileLoopContext::WhileLoopContext(LoopStmtContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::WhileLoopContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitWhileLoop(this);
  else
    return visitor->visitChildren(this);
}
//----------------- DoWhileLoopContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::DoWhileLoopContext::Do() {
  return getToken(SysYParser::Do, 0);
}

SysYParser::StmtContext* SysYParser::DoWhileLoopContext::stmt() {
  return getRuleContext<SysYParser::StmtContext>(0);
}

tree::TerminalNode* SysYParser::DoWhileLoopContext::While() {
  return getToken(SysYParser::While, 0);
}

tree::TerminalNode* SysYParser::DoWhileLoopContext::Lparen() {
  return getToken(SysYParser::Lparen, 0);
}

SysYParser::CondExpContext* SysYParser::DoWhileLoopContext::condExp() {
  return getRuleContext<SysYParser::CondExpContext>(0);
}

tree::TerminalNode* SysYParser::DoWhileLoopContext::Rparen() {
  return getToken(SysYParser::Rparen, 0);
}

tree::TerminalNode* SysYParser::DoWhileLoopContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}

SysYParser::DoWhileLoopContext::DoWhileLoopContext(LoopStmtContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::DoWhileLoopContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitDoWhileLoop(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::LoopStmtContext* SysYParser::loopStmt() {
  LoopStmtContext *_localctx = _tracker.createInstance<LoopStmtContext>(_ctx, getState());
  enterRule(_localctx, 48, SysYParser::RuleLoopStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(307);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::While: {
        _localctx = dynamic_cast<LoopStmtContext *>(_tracker.createInstance<SysYParser::WhileLoopContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(293);
        match(SysYParser::While);
        setState(294);
        match(SysYParser::Lparen);
        setState(295);
        condExp();
        setState(296);
        match(SysYParser::Rparen);
        setState(297);
        stmt();
        break;
      }

      case SysYParser::Do: {
        _localctx = dynamic_cast<LoopStmtContext *>(_tracker.createInstance<SysYParser::DoWhileLoopContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(299);
        match(SysYParser::Do);
        setState(300);
        stmt();
        setState(301);
        match(SysYParser::While);
        setState(302);
        match(SysYParser::Lparen);
        setState(303);
        condExp();
        setState(304);
        match(SysYParser::Rparen);
        setState(305);
        match(SysYParser::Semicolon);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JumpStmtContext ------------------------------------------------------------------

SysYParser::JumpStmtContext::JumpStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::JumpStmtContext::getRuleIndex() const {
  return SysYParser::RuleJumpStmt;
}

void SysYParser::JumpStmtContext::copyFrom(JumpStmtContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- BreakStmtContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::BreakStmtContext::Break() {
  return getToken(SysYParser::Break, 0);
}

tree::TerminalNode* SysYParser::BreakStmtContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}

SysYParser::BreakStmtContext::BreakStmtContext(JumpStmtContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::BreakStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBreakStmt(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ReturnStmtContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::ReturnStmtContext::Return() {
  return getToken(SysYParser::Return, 0);
}

tree::TerminalNode* SysYParser::ReturnStmtContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}

SysYParser::ExpContext* SysYParser::ReturnStmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

SysYParser::ReturnStmtContext::ReturnStmtContext(JumpStmtContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::ReturnStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitReturnStmt(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ContinueStmtContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::ContinueStmtContext::Continue() {
  return getToken(SysYParser::Continue, 0);
}

tree::TerminalNode* SysYParser::ContinueStmtContext::Semicolon() {
  return getToken(SysYParser::Semicolon, 0);
}

SysYParser::ContinueStmtContext::ContinueStmtContext(JumpStmtContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::ContinueStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitContinueStmt(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::JumpStmtContext* SysYParser::jumpStmt() {
  JumpStmtContext *_localctx = _tracker.createInstance<JumpStmtContext>(_ctx, getState());
  enterRule(_localctx, 50, SysYParser::RuleJumpStmt);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(318);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::Continue: {
        _localctx = dynamic_cast<JumpStmtContext *>(_tracker.createInstance<SysYParser::ContinueStmtContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(309);
        match(SysYParser::Continue);
        setState(310);
        match(SysYParser::Semicolon);
        break;
      }

      case SysYParser::Break: {
        _localctx = dynamic_cast<JumpStmtContext *>(_tracker.createInstance<SysYParser::BreakStmtContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(311);
        match(SysYParser::Break);
        setState(312);
        match(SysYParser::Semicolon);
        break;
      }

      case SysYParser::Return: {
        _localctx = dynamic_cast<JumpStmtContext *>(_tracker.createInstance<SysYParser::ReturnStmtContext>(_localctx));
        enterOuterAlt(_localctx, 3);
        setState(313);
        match(SysYParser::Return);
        setState(315);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParser::T__12)
          | (1ULL << SysYParser::T__13)
          | (1ULL << SysYParser::Lparen)
          | (1ULL << SysYParser::Minus)
          | (1ULL << SysYParser::Addition)
          | (1ULL << SysYParser::Exclamation)
          | (1ULL << SysYParser::Multiplication)
          | (1ULL << SysYParser::IntLiteral)
          | (1ULL << SysYParser::FloatLiteral)
          | (1ULL << SysYParser::Identifier))) != 0)) {
          setState(314);
          exp();
        }
        setState(317);
        match(SysYParser::Semicolon);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpContext ------------------------------------------------------------------

SysYParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::CondExpContext* SysYParser::ExpContext::condExp() {
  return getRuleContext<SysYParser::CondExpContext>(0);
}


size_t SysYParser::ExpContext::getRuleIndex() const {
  return SysYParser::RuleExp;
}


antlrcpp::Any SysYParser::ExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ExpContext* SysYParser::exp() {
  ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, getState());
  enterRule(_localctx, 52, SysYParser::RuleExp);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(320);
    condExp();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstExpContext ------------------------------------------------------------------

SysYParser::ConstExpContext::ConstExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::CondExpContext* SysYParser::ConstExpContext::condExp() {
  return getRuleContext<SysYParser::CondExpContext>(0);
}


size_t SysYParser::ConstExpContext::getRuleIndex() const {
  return SysYParser::RuleConstExp;
}


antlrcpp::Any SysYParser::ConstExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitConstExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ConstExpContext* SysYParser::constExp() {
  ConstExpContext *_localctx = _tracker.createInstance<ConstExpContext>(_ctx, getState());
  enterRule(_localctx, 54, SysYParser::RuleConstExp);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(322);
    condExp();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LValContext ------------------------------------------------------------------

SysYParser::LValContext::LValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::LValContext::Identifier() {
  return getToken(SysYParser::Identifier, 0);
}

std::vector<tree::TerminalNode *> SysYParser::LValContext::Lbrkt() {
  return getTokens(SysYParser::Lbrkt);
}

tree::TerminalNode* SysYParser::LValContext::Lbrkt(size_t i) {
  return getToken(SysYParser::Lbrkt, i);
}

std::vector<SysYParser::ExpContext *> SysYParser::LValContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::LValContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::LValContext::Rbrkt() {
  return getTokens(SysYParser::Rbrkt);
}

tree::TerminalNode* SysYParser::LValContext::Rbrkt(size_t i) {
  return getToken(SysYParser::Rbrkt, i);
}


size_t SysYParser::LValContext::getRuleIndex() const {
  return SysYParser::RuleLVal;
}


antlrcpp::Any SysYParser::LValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitLVal(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::LValContext* SysYParser::lVal() {
  LValContext *_localctx = _tracker.createInstance<LValContext>(_ctx, getState());
  enterRule(_localctx, 56, SysYParser::RuleLVal);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(324);
    match(SysYParser::Identifier);
    setState(331);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::Lbrkt) {
      setState(325);
      match(SysYParser::Lbrkt);
      setState(326);
      exp();
      setState(327);
      match(SysYParser::Rbrkt);
      setState(333);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrimaryExpContext ------------------------------------------------------------------

SysYParser::PrimaryExpContext::PrimaryExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::PrimaryExpContext::getRuleIndex() const {
  return SysYParser::RulePrimaryExp;
}

void SysYParser::PrimaryExpContext::copyFrom(PrimaryExpContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- PrimaryExp2Context ------------------------------------------------------------------

SysYParser::LValContext* SysYParser::PrimaryExp2Context::lVal() {
  return getRuleContext<SysYParser::LValContext>(0);
}

SysYParser::PrimaryExp2Context::PrimaryExp2Context(PrimaryExpContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::PrimaryExp2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitPrimaryExp2(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryExp1Context ------------------------------------------------------------------

tree::TerminalNode* SysYParser::PrimaryExp1Context::Lparen() {
  return getToken(SysYParser::Lparen, 0);
}

SysYParser::ExpContext* SysYParser::PrimaryExp1Context::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::PrimaryExp1Context::Rparen() {
  return getToken(SysYParser::Rparen, 0);
}

SysYParser::PrimaryExp1Context::PrimaryExp1Context(PrimaryExpContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::PrimaryExp1Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitPrimaryExp1(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryExp3Context ------------------------------------------------------------------

SysYParser::NumberContext* SysYParser::PrimaryExp3Context::number() {
  return getRuleContext<SysYParser::NumberContext>(0);
}

SysYParser::PrimaryExp3Context::PrimaryExp3Context(PrimaryExpContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::PrimaryExp3Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitPrimaryExp3(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::PrimaryExpContext* SysYParser::primaryExp() {
  PrimaryExpContext *_localctx = _tracker.createInstance<PrimaryExpContext>(_ctx, getState());
  enterRule(_localctx, 58, SysYParser::RulePrimaryExp);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(340);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::Lparen: {
        _localctx = dynamic_cast<PrimaryExpContext *>(_tracker.createInstance<SysYParser::PrimaryExp1Context>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(334);
        match(SysYParser::Lparen);
        setState(335);
        exp();
        setState(336);
        match(SysYParser::Rparen);
        break;
      }

      case SysYParser::Identifier: {
        _localctx = dynamic_cast<PrimaryExpContext *>(_tracker.createInstance<SysYParser::PrimaryExp2Context>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(338);
        lVal();
        break;
      }

      case SysYParser::IntLiteral:
      case SysYParser::FloatLiteral: {
        _localctx = dynamic_cast<PrimaryExpContext *>(_tracker.createInstance<SysYParser::PrimaryExp3Context>(_localctx));
        enterOuterAlt(_localctx, 3);
        setState(339);
        number();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext ------------------------------------------------------------------

SysYParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::NumberContext::getRuleIndex() const {
  return SysYParser::RuleNumber;
}

void SysYParser::NumberContext::copyFrom(NumberContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- Number1Context ------------------------------------------------------------------

tree::TerminalNode* SysYParser::Number1Context::IntLiteral() {
  return getToken(SysYParser::IntLiteral, 0);
}

SysYParser::Number1Context::Number1Context(NumberContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::Number1Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitNumber1(this);
  else
    return visitor->visitChildren(this);
}
//----------------- Number2Context ------------------------------------------------------------------

tree::TerminalNode* SysYParser::Number2Context::FloatLiteral() {
  return getToken(SysYParser::FloatLiteral, 0);
}

SysYParser::Number2Context::Number2Context(NumberContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::Number2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitNumber2(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::NumberContext* SysYParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 60, SysYParser::RuleNumber);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(344);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::IntLiteral: {
        _localctx = dynamic_cast<NumberContext *>(_tracker.createInstance<SysYParser::Number1Context>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(342);
        match(SysYParser::IntLiteral);
        break;
      }

      case SysYParser::FloatLiteral: {
        _localctx = dynamic_cast<NumberContext *>(_tracker.createInstance<SysYParser::Number2Context>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(343);
        match(SysYParser::FloatLiteral);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncRParamsContext ------------------------------------------------------------------

SysYParser::FuncRParamsContext::FuncRParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::FuncRParamContext *> SysYParser::FuncRParamsContext::funcRParam() {
  return getRuleContexts<SysYParser::FuncRParamContext>();
}

SysYParser::FuncRParamContext* SysYParser::FuncRParamsContext::funcRParam(size_t i) {
  return getRuleContext<SysYParser::FuncRParamContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncRParamsContext::Comma() {
  return getTokens(SysYParser::Comma);
}

tree::TerminalNode* SysYParser::FuncRParamsContext::Comma(size_t i) {
  return getToken(SysYParser::Comma, i);
}


size_t SysYParser::FuncRParamsContext::getRuleIndex() const {
  return SysYParser::RuleFuncRParams;
}


antlrcpp::Any SysYParser::FuncRParamsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncRParams(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncRParamsContext* SysYParser::funcRParams() {
  FuncRParamsContext *_localctx = _tracker.createInstance<FuncRParamsContext>(_ctx, getState());
  enterRule(_localctx, 62, SysYParser::RuleFuncRParams);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(346);
    funcRParam();
    setState(351);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::Comma) {
      setState(347);
      match(SysYParser::Comma);
      setState(348);
      funcRParam();
      setState(353);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncRParamContext ------------------------------------------------------------------

SysYParser::FuncRParamContext::FuncRParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ExpContext* SysYParser::FuncRParamContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}


size_t SysYParser::FuncRParamContext::getRuleIndex() const {
  return SysYParser::RuleFuncRParam;
}


antlrcpp::Any SysYParser::FuncRParamContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncRParam(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncRParamContext* SysYParser::funcRParam() {
  FuncRParamContext *_localctx = _tracker.createInstance<FuncRParamContext>(_ctx, getState());
  enterRule(_localctx, 64, SysYParser::RuleFuncRParam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(354);
    exp();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryExpContext ------------------------------------------------------------------

SysYParser::UnaryExpContext::UnaryExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::UnaryExpContext::getRuleIndex() const {
  return SysYParser::RuleUnaryExp;
}

void SysYParser::UnaryExpContext::copyFrom(UnaryExpContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- Unary1Context ------------------------------------------------------------------

SysYParser::PrimaryExpContext* SysYParser::Unary1Context::primaryExp() {
  return getRuleContext<SysYParser::PrimaryExpContext>(0);
}

SysYParser::Unary1Context::Unary1Context(UnaryExpContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::Unary1Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitUnary1(this);
  else
    return visitor->visitChildren(this);
}
//----------------- Unary2Context ------------------------------------------------------------------

tree::TerminalNode* SysYParser::Unary2Context::Identifier() {
  return getToken(SysYParser::Identifier, 0);
}

tree::TerminalNode* SysYParser::Unary2Context::Lparen() {
  return getToken(SysYParser::Lparen, 0);
}

tree::TerminalNode* SysYParser::Unary2Context::Rparen() {
  return getToken(SysYParser::Rparen, 0);
}

SysYParser::FuncRParamsContext* SysYParser::Unary2Context::funcRParams() {
  return getRuleContext<SysYParser::FuncRParamsContext>(0);
}

SysYParser::Unary2Context::Unary2Context(UnaryExpContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::Unary2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitUnary2(this);
  else
    return visitor->visitChildren(this);
}
//----------------- Unary3Context ------------------------------------------------------------------

SysYParser::UnaryOpContext* SysYParser::Unary3Context::unaryOp() {
  return getRuleContext<SysYParser::UnaryOpContext>(0);
}

SysYParser::UnaryExpContext* SysYParser::Unary3Context::unaryExp() {
  return getRuleContext<SysYParser::UnaryExpContext>(0);
}

SysYParser::Unary3Context::Unary3Context(UnaryExpContext *ctx) { copyFrom(ctx); }


antlrcpp::Any SysYParser::Unary3Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitUnary3(this);
  else
    return visitor->visitChildren(this);
}
SysYParser::UnaryExpContext* SysYParser::unaryExp() {
  UnaryExpContext *_localctx = _tracker.createInstance<UnaryExpContext>(_ctx, getState());
  enterRule(_localctx, 66, SysYParser::RuleUnaryExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(366);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
    case 1: {
      _localctx = dynamic_cast<UnaryExpContext *>(_tracker.createInstance<SysYParser::Unary1Context>(_localctx));
      enterOuterAlt(_localctx, 1);
      setState(356);
      primaryExp();
      break;
    }

    case 2: {
      _localctx = dynamic_cast<UnaryExpContext *>(_tracker.createInstance<SysYParser::Unary2Context>(_localctx));
      enterOuterAlt(_localctx, 2);
      setState(357);
      match(SysYParser::Identifier);
      setState(358);
      match(SysYParser::Lparen);
      setState(360);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParser::T__12)
        | (1ULL << SysYParser::T__13)
        | (1ULL << SysYParser::Lparen)
        | (1ULL << SysYParser::Minus)
        | (1ULL << SysYParser::Addition)
        | (1ULL << SysYParser::Exclamation)
        | (1ULL << SysYParser::Multiplication)
        | (1ULL << SysYParser::IntLiteral)
        | (1ULL << SysYParser::FloatLiteral)
        | (1ULL << SysYParser::Identifier))) != 0)) {
        setState(359);
        funcRParams();
      }
      setState(362);
      match(SysYParser::Rparen);
      break;
    }

    case 3: {
      _localctx = dynamic_cast<UnaryExpContext *>(_tracker.createInstance<SysYParser::Unary3Context>(_localctx));
      enterOuterAlt(_localctx, 3);
      setState(363);
      unaryOp();
      setState(364);
      unaryExp();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryOpContext ------------------------------------------------------------------

SysYParser::UnaryOpContext::UnaryOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::UnaryOpContext::Multiplication() {
  return getToken(SysYParser::Multiplication, 0);
}

tree::TerminalNode* SysYParser::UnaryOpContext::Addition() {
  return getToken(SysYParser::Addition, 0);
}

tree::TerminalNode* SysYParser::UnaryOpContext::Minus() {
  return getToken(SysYParser::Minus, 0);
}

tree::TerminalNode* SysYParser::UnaryOpContext::Exclamation() {
  return getToken(SysYParser::Exclamation, 0);
}


size_t SysYParser::UnaryOpContext::getRuleIndex() const {
  return SysYParser::RuleUnaryOp;
}


antlrcpp::Any SysYParser::UnaryOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitUnaryOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::UnaryOpContext* SysYParser::unaryOp() {
  UnaryOpContext *_localctx = _tracker.createInstance<UnaryOpContext>(_ctx, getState());
  enterRule(_localctx, 68, SysYParser::RuleUnaryOp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(368);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::T__12)
      | (1ULL << SysYParser::T__13)
      | (1ULL << SysYParser::Minus)
      | (1ULL << SysYParser::Addition)
      | (1ULL << SysYParser::Exclamation)
      | (1ULL << SysYParser::Multiplication))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MulExpContext ------------------------------------------------------------------

SysYParser::MulExpContext::MulExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::UnaryExpContext *> SysYParser::MulExpContext::unaryExp() {
  return getRuleContexts<SysYParser::UnaryExpContext>();
}

SysYParser::UnaryExpContext* SysYParser::MulExpContext::unaryExp(size_t i) {
  return getRuleContext<SysYParser::UnaryExpContext>(i);
}

std::vector<SysYParser::MulOpContext *> SysYParser::MulExpContext::mulOp() {
  return getRuleContexts<SysYParser::MulOpContext>();
}

SysYParser::MulOpContext* SysYParser::MulExpContext::mulOp(size_t i) {
  return getRuleContext<SysYParser::MulOpContext>(i);
}


size_t SysYParser::MulExpContext::getRuleIndex() const {
  return SysYParser::RuleMulExp;
}


antlrcpp::Any SysYParser::MulExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitMulExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::MulExpContext* SysYParser::mulExp() {
  MulExpContext *_localctx = _tracker.createInstance<MulExpContext>(_ctx, getState());
  enterRule(_localctx, 70, SysYParser::RuleMulExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(370);
    unaryExp();
    setState(376);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::Multiplication)
      | (1ULL << SysYParser::Division)
      | (1ULL << SysYParser::Modulo))) != 0)) {
      setState(371);
      mulOp();
      setState(372);
      unaryExp();
      setState(378);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MulOpContext ------------------------------------------------------------------

SysYParser::MulOpContext::MulOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::MulOpContext::Multiplication() {
  return getToken(SysYParser::Multiplication, 0);
}

tree::TerminalNode* SysYParser::MulOpContext::Division() {
  return getToken(SysYParser::Division, 0);
}

tree::TerminalNode* SysYParser::MulOpContext::Modulo() {
  return getToken(SysYParser::Modulo, 0);
}


size_t SysYParser::MulOpContext::getRuleIndex() const {
  return SysYParser::RuleMulOp;
}


antlrcpp::Any SysYParser::MulOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitMulOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::MulOpContext* SysYParser::mulOp() {
  MulOpContext *_localctx = _tracker.createInstance<MulOpContext>(_ctx, getState());
  enterRule(_localctx, 72, SysYParser::RuleMulOp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(379);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::Multiplication)
      | (1ULL << SysYParser::Division)
      | (1ULL << SysYParser::Modulo))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AddExpContext ------------------------------------------------------------------

SysYParser::AddExpContext::AddExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::MulExpContext *> SysYParser::AddExpContext::mulExp() {
  return getRuleContexts<SysYParser::MulExpContext>();
}

SysYParser::MulExpContext* SysYParser::AddExpContext::mulExp(size_t i) {
  return getRuleContext<SysYParser::MulExpContext>(i);
}

std::vector<SysYParser::AddOpContext *> SysYParser::AddExpContext::addOp() {
  return getRuleContexts<SysYParser::AddOpContext>();
}

SysYParser::AddOpContext* SysYParser::AddExpContext::addOp(size_t i) {
  return getRuleContext<SysYParser::AddOpContext>(i);
}


size_t SysYParser::AddExpContext::getRuleIndex() const {
  return SysYParser::RuleAddExp;
}


antlrcpp::Any SysYParser::AddExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAddExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::AddExpContext* SysYParser::addExp() {
  AddExpContext *_localctx = _tracker.createInstance<AddExpContext>(_ctx, getState());
  enterRule(_localctx, 74, SysYParser::RuleAddExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(381);
    mulExp();
    setState(387);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::Minus

    || _la == SysYParser::Addition) {
      setState(382);
      addOp();
      setState(383);
      mulExp();
      setState(389);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AddOpContext ------------------------------------------------------------------

SysYParser::AddOpContext::AddOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::AddOpContext::Addition() {
  return getToken(SysYParser::Addition, 0);
}

tree::TerminalNode* SysYParser::AddOpContext::Minus() {
  return getToken(SysYParser::Minus, 0);
}


size_t SysYParser::AddOpContext::getRuleIndex() const {
  return SysYParser::RuleAddOp;
}


antlrcpp::Any SysYParser::AddOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAddOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::AddOpContext* SysYParser::addOp() {
  AddOpContext *_localctx = _tracker.createInstance<AddOpContext>(_ctx, getState());
  enterRule(_localctx, 76, SysYParser::RuleAddOp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(390);
    _la = _input->LA(1);
    if (!(_la == SysYParser::Minus

    || _la == SysYParser::Addition)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ShiftExpContext ------------------------------------------------------------------

SysYParser::ShiftExpContext::ShiftExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::AddExpContext *> SysYParser::ShiftExpContext::addExp() {
  return getRuleContexts<SysYParser::AddExpContext>();
}

SysYParser::AddExpContext* SysYParser::ShiftExpContext::addExp(size_t i) {
  return getRuleContext<SysYParser::AddExpContext>(i);
}

std::vector<SysYParser::ShiftOpContext *> SysYParser::ShiftExpContext::shiftOp() {
  return getRuleContexts<SysYParser::ShiftOpContext>();
}

SysYParser::ShiftOpContext* SysYParser::ShiftExpContext::shiftOp(size_t i) {
  return getRuleContext<SysYParser::ShiftOpContext>(i);
}


size_t SysYParser::ShiftExpContext::getRuleIndex() const {
  return SysYParser::RuleShiftExp;
}


antlrcpp::Any SysYParser::ShiftExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitShiftExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ShiftExpContext* SysYParser::shiftExp() {
  ShiftExpContext *_localctx = _tracker.createInstance<ShiftExpContext>(_ctx, getState());
  enterRule(_localctx, 78, SysYParser::RuleShiftExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(392);
    addExp();
    setState(398);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::T__14

    || _la == SysYParser::T__15) {
      setState(393);
      shiftOp();
      setState(394);
      addExp();
      setState(400);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ShiftOpContext ------------------------------------------------------------------

SysYParser::ShiftOpContext::ShiftOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::ShiftOpContext::getRuleIndex() const {
  return SysYParser::RuleShiftOp;
}


antlrcpp::Any SysYParser::ShiftOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitShiftOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ShiftOpContext* SysYParser::shiftOp() {
  ShiftOpContext *_localctx = _tracker.createInstance<ShiftOpContext>(_ctx, getState());
  enterRule(_localctx, 80, SysYParser::RuleShiftOp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(401);
    _la = _input->LA(1);
    if (!(_la == SysYParser::T__14

    || _la == SysYParser::T__15)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RelExpContext ------------------------------------------------------------------

SysYParser::RelExpContext::RelExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::ShiftExpContext *> SysYParser::RelExpContext::shiftExp() {
  return getRuleContexts<SysYParser::ShiftExpContext>();
}

SysYParser::ShiftExpContext* SysYParser::RelExpContext::shiftExp(size_t i) {
  return getRuleContext<SysYParser::ShiftExpContext>(i);
}

std::vector<SysYParser::RelOpContext *> SysYParser::RelExpContext::relOp() {
  return getRuleContexts<SysYParser::RelOpContext>();
}

SysYParser::RelOpContext* SysYParser::RelExpContext::relOp(size_t i) {
  return getRuleContext<SysYParser::RelOpContext>(i);
}


size_t SysYParser::RelExpContext::getRuleIndex() const {
  return SysYParser::RuleRelExp;
}


antlrcpp::Any SysYParser::RelExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitRelExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::RelExpContext* SysYParser::relExp() {
  RelExpContext *_localctx = _tracker.createInstance<RelExpContext>(_ctx, getState());
  enterRule(_localctx, 82, SysYParser::RuleRelExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(403);
    shiftExp();
    setState(409);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::LT)
      | (1ULL << SysYParser::LE)
      | (1ULL << SysYParser::GT)
      | (1ULL << SysYParser::GE))) != 0)) {
      setState(404);
      relOp();
      setState(405);
      shiftExp();
      setState(411);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RelOpContext ------------------------------------------------------------------

SysYParser::RelOpContext::RelOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::RelOpContext::LT() {
  return getToken(SysYParser::LT, 0);
}

tree::TerminalNode* SysYParser::RelOpContext::GT() {
  return getToken(SysYParser::GT, 0);
}

tree::TerminalNode* SysYParser::RelOpContext::LE() {
  return getToken(SysYParser::LE, 0);
}

tree::TerminalNode* SysYParser::RelOpContext::GE() {
  return getToken(SysYParser::GE, 0);
}


size_t SysYParser::RelOpContext::getRuleIndex() const {
  return SysYParser::RuleRelOp;
}


antlrcpp::Any SysYParser::RelOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitRelOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::RelOpContext* SysYParser::relOp() {
  RelOpContext *_localctx = _tracker.createInstance<RelOpContext>(_ctx, getState());
  enterRule(_localctx, 84, SysYParser::RuleRelOp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(412);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::LT)
      | (1ULL << SysYParser::LE)
      | (1ULL << SysYParser::GT)
      | (1ULL << SysYParser::GE))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EqExpContext ------------------------------------------------------------------

SysYParser::EqExpContext::EqExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::RelExpContext *> SysYParser::EqExpContext::relExp() {
  return getRuleContexts<SysYParser::RelExpContext>();
}

SysYParser::RelExpContext* SysYParser::EqExpContext::relExp(size_t i) {
  return getRuleContext<SysYParser::RelExpContext>(i);
}

std::vector<SysYParser::EqOpContext *> SysYParser::EqExpContext::eqOp() {
  return getRuleContexts<SysYParser::EqOpContext>();
}

SysYParser::EqOpContext* SysYParser::EqExpContext::eqOp(size_t i) {
  return getRuleContext<SysYParser::EqOpContext>(i);
}


size_t SysYParser::EqExpContext::getRuleIndex() const {
  return SysYParser::RuleEqExp;
}


antlrcpp::Any SysYParser::EqExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitEqExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::EqExpContext* SysYParser::eqExp() {
  EqExpContext *_localctx = _tracker.createInstance<EqExpContext>(_ctx, getState());
  enterRule(_localctx, 86, SysYParser::RuleEqExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(414);
    relExp();
    setState(420);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::EQ

    || _la == SysYParser::NEQ) {
      setState(415);
      eqOp();
      setState(416);
      relExp();
      setState(422);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EqOpContext ------------------------------------------------------------------

SysYParser::EqOpContext::EqOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::EqOpContext::EQ() {
  return getToken(SysYParser::EQ, 0);
}

tree::TerminalNode* SysYParser::EqOpContext::NEQ() {
  return getToken(SysYParser::NEQ, 0);
}


size_t SysYParser::EqOpContext::getRuleIndex() const {
  return SysYParser::RuleEqOp;
}


antlrcpp::Any SysYParser::EqOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitEqOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::EqOpContext* SysYParser::eqOp() {
  EqOpContext *_localctx = _tracker.createInstance<EqOpContext>(_ctx, getState());
  enterRule(_localctx, 88, SysYParser::RuleEqOp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(423);
    _la = _input->LA(1);
    if (!(_la == SysYParser::EQ

    || _la == SysYParser::NEQ)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AndExpContext ------------------------------------------------------------------

SysYParser::AndExpContext::AndExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::EqExpContext *> SysYParser::AndExpContext::eqExp() {
  return getRuleContexts<SysYParser::EqExpContext>();
}

SysYParser::EqExpContext* SysYParser::AndExpContext::eqExp(size_t i) {
  return getRuleContext<SysYParser::EqExpContext>(i);
}


size_t SysYParser::AndExpContext::getRuleIndex() const {
  return SysYParser::RuleAndExp;
}


antlrcpp::Any SysYParser::AndExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAndExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::AndExpContext* SysYParser::andExp() {
  AndExpContext *_localctx = _tracker.createInstance<AndExpContext>(_ctx, getState());
  enterRule(_localctx, 90, SysYParser::RuleAndExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(425);
    eqExp();
    setState(430);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::T__12) {
      setState(426);
      match(SysYParser::T__12);
      setState(427);
      eqExp();
      setState(432);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExOrExpContext ------------------------------------------------------------------

SysYParser::ExOrExpContext::ExOrExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::AndExpContext *> SysYParser::ExOrExpContext::andExp() {
  return getRuleContexts<SysYParser::AndExpContext>();
}

SysYParser::AndExpContext* SysYParser::ExOrExpContext::andExp(size_t i) {
  return getRuleContext<SysYParser::AndExpContext>(i);
}


size_t SysYParser::ExOrExpContext::getRuleIndex() const {
  return SysYParser::RuleExOrExp;
}


antlrcpp::Any SysYParser::ExOrExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitExOrExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ExOrExpContext* SysYParser::exOrExp() {
  ExOrExpContext *_localctx = _tracker.createInstance<ExOrExpContext>(_ctx, getState());
  enterRule(_localctx, 92, SysYParser::RuleExOrExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(433);
    andExp();
    setState(438);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::T__16) {
      setState(434);
      match(SysYParser::T__16);
      setState(435);
      andExp();
      setState(440);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InOrExpContext ------------------------------------------------------------------

SysYParser::InOrExpContext::InOrExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::ExOrExpContext *> SysYParser::InOrExpContext::exOrExp() {
  return getRuleContexts<SysYParser::ExOrExpContext>();
}

SysYParser::ExOrExpContext* SysYParser::InOrExpContext::exOrExp(size_t i) {
  return getRuleContext<SysYParser::ExOrExpContext>(i);
}


size_t SysYParser::InOrExpContext::getRuleIndex() const {
  return SysYParser::RuleInOrExp;
}


antlrcpp::Any SysYParser::InOrExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitInOrExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::InOrExpContext* SysYParser::inOrExp() {
  InOrExpContext *_localctx = _tracker.createInstance<InOrExpContext>(_ctx, getState());
  enterRule(_localctx, 94, SysYParser::RuleInOrExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(441);
    exOrExp();
    setState(446);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::T__17) {
      setState(442);
      match(SysYParser::T__17);
      setState(443);
      exOrExp();
      setState(448);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LAndExpContext ------------------------------------------------------------------

SysYParser::LAndExpContext::LAndExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::InOrExpContext *> SysYParser::LAndExpContext::inOrExp() {
  return getRuleContexts<SysYParser::InOrExpContext>();
}

SysYParser::InOrExpContext* SysYParser::LAndExpContext::inOrExp(size_t i) {
  return getRuleContext<SysYParser::InOrExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::LAndExpContext::LAND() {
  return getTokens(SysYParser::LAND);
}

tree::TerminalNode* SysYParser::LAndExpContext::LAND(size_t i) {
  return getToken(SysYParser::LAND, i);
}


size_t SysYParser::LAndExpContext::getRuleIndex() const {
  return SysYParser::RuleLAndExp;
}


antlrcpp::Any SysYParser::LAndExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitLAndExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::LAndExpContext* SysYParser::lAndExp() {
  LAndExpContext *_localctx = _tracker.createInstance<LAndExpContext>(_ctx, getState());
  enterRule(_localctx, 96, SysYParser::RuleLAndExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(449);
    inOrExp();
    setState(454);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::LAND) {
      setState(450);
      match(SysYParser::LAND);
      setState(451);
      inOrExp();
      setState(456);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LOrExpContext ------------------------------------------------------------------

SysYParser::LOrExpContext::LOrExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::LAndExpContext *> SysYParser::LOrExpContext::lAndExp() {
  return getRuleContexts<SysYParser::LAndExpContext>();
}

SysYParser::LAndExpContext* SysYParser::LOrExpContext::lAndExp(size_t i) {
  return getRuleContext<SysYParser::LAndExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::LOrExpContext::LOR() {
  return getTokens(SysYParser::LOR);
}

tree::TerminalNode* SysYParser::LOrExpContext::LOR(size_t i) {
  return getToken(SysYParser::LOR, i);
}


size_t SysYParser::LOrExpContext::getRuleIndex() const {
  return SysYParser::RuleLOrExp;
}


antlrcpp::Any SysYParser::LOrExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitLOrExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::LOrExpContext* SysYParser::lOrExp() {
  LOrExpContext *_localctx = _tracker.createInstance<LOrExpContext>(_ctx, getState());
  enterRule(_localctx, 98, SysYParser::RuleLOrExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(457);
    lAndExp();
    setState(462);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::LOR) {
      setState(458);
      match(SysYParser::LOR);
      setState(459);
      lAndExp();
      setState(464);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CondExpContext ------------------------------------------------------------------

SysYParser::CondExpContext::CondExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LOrExpContext* SysYParser::CondExpContext::lOrExp() {
  return getRuleContext<SysYParser::LOrExpContext>(0);
}

SysYParser::ExpContext* SysYParser::CondExpContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

SysYParser::CondExpContext* SysYParser::CondExpContext::condExp() {
  return getRuleContext<SysYParser::CondExpContext>(0);
}


size_t SysYParser::CondExpContext::getRuleIndex() const {
  return SysYParser::RuleCondExp;
}


antlrcpp::Any SysYParser::CondExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitCondExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::CondExpContext* SysYParser::condExp() {
  CondExpContext *_localctx = _tracker.createInstance<CondExpContext>(_ctx, getState());
  enterRule(_localctx, 100, SysYParser::RuleCondExp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(465);
    lOrExp();
    setState(471);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::T__18) {
      setState(466);
      match(SysYParser::T__18);
      setState(467);
      exp();
      setState(468);
      match(SysYParser::T__19);
      setState(469);
      condExp();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

// Static vars and initialization.
std::vector<dfa::DFA> SysYParser::_decisionToDFA;
atn::PredictionContextCache SysYParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN SysYParser::_atn;
std::vector<uint16_t> SysYParser::_serializedATN;

std::vector<std::string> SysYParser::_ruleNames = {
  "compilationUnit", "translationUnit", "externalDeclaration", "decl", "constDecl", 
  "bType", "constDef", "constInitVal", "varDecl", "varDef", "initVal", "funcDef", 
  "funcType", "funcFParams", "funcFParam", "block", "blockItemList", "blockItem", 
  "stmt", "assignStmt", "assignOp", "expStmt", "blockStmt", "seleStmt", 
  "loopStmt", "jumpStmt", "exp", "constExp", "lVal", "primaryExp", "number", 
  "funcRParams", "funcRParam", "unaryExp", "unaryOp", "mulExp", "mulOp", 
  "addExp", "addOp", "shiftExp", "shiftOp", "relExp", "relOp", "eqExp", 
  "eqOp", "andExp", "exOrExp", "inOrExp", "lAndExp", "lOrExp", "condExp"
};

std::vector<std::string> SysYParser::_literalNames = {
  "", "'float'", "'='", "'*='", "'/='", "'%='", "'+='", "'-='", "'<<='", 
  "'>>='", "'&='", "'^='", "'|='", "'&'", "'~'", "'<<'", "'>>'", "'^'", 
  "'|'", "'?'", "':'", "'int'", "'void'", "'const'", "'return'", "'if'", 
  "'else'", "'while'", "'for'", "'do'", "'break'", "'continue'", "'('", 
  "')'", "'['", "']'", "'{'", "'}'", "','", "';'", "'-'", "'+'", "'!'", 
  "'*'", "'/'", "'%'", "'&&'", "'||'", "'=='", "'!='", "'<'", "'<='", "'>'", 
  "'>='"
};

std::vector<std::string> SysYParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "Int", "Void", "Const", "Return", "If", "Else", "While", "For", 
  "Do", "Break", "Continue", "Lparen", "Rparen", "Lbrkt", "Rbrkt", "Lbrace", 
  "Rbrace", "Comma", "Semicolon", "Minus", "Addition", "Exclamation", "Multiplication", 
  "Division", "Modulo", "LAND", "LOR", "EQ", "NEQ", "LT", "LE", "GT", "GE", 
  "IntLiteral", "FloatLiteral", "Identifier", "Whitespace", "Newline", "BlockComment", 
  "LineComment"
};

dfa::Vocabulary SysYParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> SysYParser::_tokenNames;

SysYParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x3e, 0x1dc, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
    0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 
    0x4, 0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 
    0x9, 0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 
    0x4, 0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 
    0x12, 0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x4, 0x15, 
    0x9, 0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 0x9, 0x17, 0x4, 0x18, 0x9, 
    0x18, 0x4, 0x19, 0x9, 0x19, 0x4, 0x1a, 0x9, 0x1a, 0x4, 0x1b, 0x9, 0x1b, 
    0x4, 0x1c, 0x9, 0x1c, 0x4, 0x1d, 0x9, 0x1d, 0x4, 0x1e, 0x9, 0x1e, 0x4, 
    0x1f, 0x9, 0x1f, 0x4, 0x20, 0x9, 0x20, 0x4, 0x21, 0x9, 0x21, 0x4, 0x22, 
    0x9, 0x22, 0x4, 0x23, 0x9, 0x23, 0x4, 0x24, 0x9, 0x24, 0x4, 0x25, 0x9, 
    0x25, 0x4, 0x26, 0x9, 0x26, 0x4, 0x27, 0x9, 0x27, 0x4, 0x28, 0x9, 0x28, 
    0x4, 0x29, 0x9, 0x29, 0x4, 0x2a, 0x9, 0x2a, 0x4, 0x2b, 0x9, 0x2b, 0x4, 
    0x2c, 0x9, 0x2c, 0x4, 0x2d, 0x9, 0x2d, 0x4, 0x2e, 0x9, 0x2e, 0x4, 0x2f, 
    0x9, 0x2f, 0x4, 0x30, 0x9, 0x30, 0x4, 0x31, 0x9, 0x31, 0x4, 0x32, 0x9, 
    0x32, 0x4, 0x33, 0x9, 0x33, 0x4, 0x34, 0x9, 0x34, 0x3, 0x2, 0x5, 0x2, 
    0x6a, 0xa, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 0x6, 0x3, 0x6f, 0xa, 0x3, 
    0xd, 0x3, 0xe, 0x3, 0x70, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x76, 
    0xa, 0x4, 0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0x7a, 0xa, 0x5, 0x3, 0x6, 0x3, 
    0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x7, 0x6, 0x81, 0xa, 0x6, 0xc, 0x6, 
    0xe, 0x6, 0x84, 0xb, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 
    0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x7, 0x8, 0x8f, 0xa, 0x8, 
    0xc, 0x8, 0xe, 0x8, 0x92, 0xb, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 
    0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x7, 0x9, 0x9c, 0xa, 0x9, 
    0xc, 0x9, 0xe, 0x9, 0x9f, 0xb, 0x9, 0x5, 0x9, 0xa1, 0xa, 0x9, 0x3, 0x9, 
    0x5, 0x9, 0xa4, 0xa, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x7, 
    0xa, 0xaa, 0xa, 0xa, 0xc, 0xa, 0xe, 0xa, 0xad, 0xb, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x7, 0xb, 0xb6, 
    0xa, 0xb, 0xc, 0xb, 0xe, 0xb, 0xb9, 0xb, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xb, 0x7, 0xb, 0xc0, 0xa, 0xb, 0xc, 0xb, 0xe, 0xb, 
    0xc3, 0xb, 0xb, 0x3, 0xb, 0x3, 0xb, 0x5, 0xb, 0xc7, 0xa, 0xb, 0x3, 0xc, 
    0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x7, 0xc, 0xce, 0xa, 0xc, 0xc, 
    0xc, 0xe, 0xc, 0xd1, 0xb, 0xc, 0x5, 0xc, 0xd3, 0xa, 0xc, 0x3, 0xc, 0x5, 
    0xc, 0xd6, 0xa, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x5, 0xd, 
    0xdc, 0xa, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 
    0xf, 0x3, 0xf, 0x3, 0xf, 0x7, 0xf, 0xe6, 0xa, 0xf, 0xc, 0xf, 0xe, 0xf, 
    0xe9, 0xb, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 
    0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x7, 0x10, 0xf3, 0xa, 0x10, 0xc, 0x10, 
    0xe, 0x10, 0xf6, 0xb, 0x10, 0x5, 0x10, 0xf8, 0xa, 0x10, 0x3, 0x11, 0x3, 
    0x11, 0x5, 0x11, 0xfc, 0xa, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x6, 
    0x12, 0x101, 0xa, 0x12, 0xd, 0x12, 0xe, 0x12, 0x102, 0x3, 0x13, 0x3, 
    0x13, 0x5, 0x13, 0x107, 0xa, 0x13, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 
    0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x10f, 0xa, 0x14, 0x3, 0x15, 
    0x3, 0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 0x16, 0x3, 0x16, 0x3, 
    0x17, 0x5, 0x17, 0x119, 0xa, 0x17, 0x3, 0x17, 0x3, 0x17, 0x3, 0x18, 
    0x3, 0x18, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 
    0x19, 0x3, 0x19, 0x5, 0x19, 0x126, 0xa, 0x19, 0x3, 0x1a, 0x3, 0x1a, 
    0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 
    0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x5, 0x1a, 
    0x136, 0xa, 0x1a, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 
    0x3, 0x1b, 0x5, 0x1b, 0x13e, 0xa, 0x1b, 0x3, 0x1b, 0x5, 0x1b, 0x141, 
    0xa, 0x1b, 0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1e, 0x3, 
    0x1e, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 0x7, 0x1e, 0x14c, 0xa, 0x1e, 
    0xc, 0x1e, 0xe, 0x1e, 0x14f, 0xb, 0x1e, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 
    0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x5, 0x1f, 0x157, 0xa, 0x1f, 0x3, 0x20, 
    0x3, 0x20, 0x5, 0x20, 0x15b, 0xa, 0x20, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 
    0x7, 0x21, 0x160, 0xa, 0x21, 0xc, 0x21, 0xe, 0x21, 0x163, 0xb, 0x21, 
    0x3, 0x22, 0x3, 0x22, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x5, 
    0x23, 0x16b, 0xa, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 
    0x5, 0x23, 0x171, 0xa, 0x23, 0x3, 0x24, 0x3, 0x24, 0x3, 0x25, 0x3, 0x25, 
    0x3, 0x25, 0x3, 0x25, 0x7, 0x25, 0x179, 0xa, 0x25, 0xc, 0x25, 0xe, 0x25, 
    0x17c, 0xb, 0x25, 0x3, 0x26, 0x3, 0x26, 0x3, 0x27, 0x3, 0x27, 0x3, 0x27, 
    0x3, 0x27, 0x7, 0x27, 0x184, 0xa, 0x27, 0xc, 0x27, 0xe, 0x27, 0x187, 
    0xb, 0x27, 0x3, 0x28, 0x3, 0x28, 0x3, 0x29, 0x3, 0x29, 0x3, 0x29, 0x3, 
    0x29, 0x7, 0x29, 0x18f, 0xa, 0x29, 0xc, 0x29, 0xe, 0x29, 0x192, 0xb, 
    0x29, 0x3, 0x2a, 0x3, 0x2a, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 
    0x7, 0x2b, 0x19a, 0xa, 0x2b, 0xc, 0x2b, 0xe, 0x2b, 0x19d, 0xb, 0x2b, 
    0x3, 0x2c, 0x3, 0x2c, 0x3, 0x2d, 0x3, 0x2d, 0x3, 0x2d, 0x3, 0x2d, 0x7, 
    0x2d, 0x1a5, 0xa, 0x2d, 0xc, 0x2d, 0xe, 0x2d, 0x1a8, 0xb, 0x2d, 0x3, 
    0x2e, 0x3, 0x2e, 0x3, 0x2f, 0x3, 0x2f, 0x3, 0x2f, 0x7, 0x2f, 0x1af, 
    0xa, 0x2f, 0xc, 0x2f, 0xe, 0x2f, 0x1b2, 0xb, 0x2f, 0x3, 0x30, 0x3, 0x30, 
    0x3, 0x30, 0x7, 0x30, 0x1b7, 0xa, 0x30, 0xc, 0x30, 0xe, 0x30, 0x1ba, 
    0xb, 0x30, 0x3, 0x31, 0x3, 0x31, 0x3, 0x31, 0x7, 0x31, 0x1bf, 0xa, 0x31, 
    0xc, 0x31, 0xe, 0x31, 0x1c2, 0xb, 0x31, 0x3, 0x32, 0x3, 0x32, 0x3, 0x32, 
    0x7, 0x32, 0x1c7, 0xa, 0x32, 0xc, 0x32, 0xe, 0x32, 0x1ca, 0xb, 0x32, 
    0x3, 0x33, 0x3, 0x33, 0x3, 0x33, 0x7, 0x33, 0x1cf, 0xa, 0x33, 0xc, 0x33, 
    0xe, 0x33, 0x1d2, 0xb, 0x33, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 
    0x3, 0x34, 0x3, 0x34, 0x5, 0x34, 0x1da, 0xa, 0x34, 0x3, 0x34, 0x2, 0x2, 
    0x35, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 
    0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 
    0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x48, 
    0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e, 0x60, 
    0x62, 0x64, 0x66, 0x2, 0xb, 0x4, 0x2, 0x3, 0x3, 0x17, 0x17, 0x4, 0x2, 
    0x3, 0x3, 0x17, 0x18, 0x3, 0x2, 0x4, 0xe, 0x4, 0x2, 0xf, 0x10, 0x2a, 
    0x2d, 0x3, 0x2, 0x2d, 0x2f, 0x3, 0x2, 0x2a, 0x2b, 0x3, 0x2, 0x11, 0x12, 
    0x3, 0x2, 0x34, 0x37, 0x3, 0x2, 0x32, 0x33, 0x2, 0x1de, 0x2, 0x69, 0x3, 
    0x2, 0x2, 0x2, 0x4, 0x6e, 0x3, 0x2, 0x2, 0x2, 0x6, 0x75, 0x3, 0x2, 0x2, 
    0x2, 0x8, 0x79, 0x3, 0x2, 0x2, 0x2, 0xa, 0x7b, 0x3, 0x2, 0x2, 0x2, 0xc, 
    0x87, 0x3, 0x2, 0x2, 0x2, 0xe, 0x89, 0x3, 0x2, 0x2, 0x2, 0x10, 0xa3, 
    0x3, 0x2, 0x2, 0x2, 0x12, 0xa5, 0x3, 0x2, 0x2, 0x2, 0x14, 0xc6, 0x3, 
    0x2, 0x2, 0x2, 0x16, 0xd5, 0x3, 0x2, 0x2, 0x2, 0x18, 0xd7, 0x3, 0x2, 
    0x2, 0x2, 0x1a, 0xe0, 0x3, 0x2, 0x2, 0x2, 0x1c, 0xe2, 0x3, 0x2, 0x2, 
    0x2, 0x1e, 0xea, 0x3, 0x2, 0x2, 0x2, 0x20, 0xf9, 0x3, 0x2, 0x2, 0x2, 
    0x22, 0x100, 0x3, 0x2, 0x2, 0x2, 0x24, 0x106, 0x3, 0x2, 0x2, 0x2, 0x26, 
    0x10e, 0x3, 0x2, 0x2, 0x2, 0x28, 0x110, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x115, 
    0x3, 0x2, 0x2, 0x2, 0x2c, 0x118, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x11c, 0x3, 
    0x2, 0x2, 0x2, 0x30, 0x11e, 0x3, 0x2, 0x2, 0x2, 0x32, 0x135, 0x3, 0x2, 
    0x2, 0x2, 0x34, 0x140, 0x3, 0x2, 0x2, 0x2, 0x36, 0x142, 0x3, 0x2, 0x2, 
    0x2, 0x38, 0x144, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x146, 0x3, 0x2, 0x2, 0x2, 
    0x3c, 0x156, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x15a, 0x3, 0x2, 0x2, 0x2, 0x40, 
    0x15c, 0x3, 0x2, 0x2, 0x2, 0x42, 0x164, 0x3, 0x2, 0x2, 0x2, 0x44, 0x170, 
    0x3, 0x2, 0x2, 0x2, 0x46, 0x172, 0x3, 0x2, 0x2, 0x2, 0x48, 0x174, 0x3, 
    0x2, 0x2, 0x2, 0x4a, 0x17d, 0x3, 0x2, 0x2, 0x2, 0x4c, 0x17f, 0x3, 0x2, 
    0x2, 0x2, 0x4e, 0x188, 0x3, 0x2, 0x2, 0x2, 0x50, 0x18a, 0x3, 0x2, 0x2, 
    0x2, 0x52, 0x193, 0x3, 0x2, 0x2, 0x2, 0x54, 0x195, 0x3, 0x2, 0x2, 0x2, 
    0x56, 0x19e, 0x3, 0x2, 0x2, 0x2, 0x58, 0x1a0, 0x3, 0x2, 0x2, 0x2, 0x5a, 
    0x1a9, 0x3, 0x2, 0x2, 0x2, 0x5c, 0x1ab, 0x3, 0x2, 0x2, 0x2, 0x5e, 0x1b3, 
    0x3, 0x2, 0x2, 0x2, 0x60, 0x1bb, 0x3, 0x2, 0x2, 0x2, 0x62, 0x1c3, 0x3, 
    0x2, 0x2, 0x2, 0x64, 0x1cb, 0x3, 0x2, 0x2, 0x2, 0x66, 0x1d3, 0x3, 0x2, 
    0x2, 0x2, 0x68, 0x6a, 0x5, 0x4, 0x3, 0x2, 0x69, 0x68, 0x3, 0x2, 0x2, 
    0x2, 0x69, 0x6a, 0x3, 0x2, 0x2, 0x2, 0x6a, 0x6b, 0x3, 0x2, 0x2, 0x2, 
    0x6b, 0x6c, 0x7, 0x2, 0x2, 0x3, 0x6c, 0x3, 0x3, 0x2, 0x2, 0x2, 0x6d, 
    0x6f, 0x5, 0x6, 0x4, 0x2, 0x6e, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x70, 
    0x3, 0x2, 0x2, 0x2, 0x70, 0x6e, 0x3, 0x2, 0x2, 0x2, 0x70, 0x71, 0x3, 
    0x2, 0x2, 0x2, 0x71, 0x5, 0x3, 0x2, 0x2, 0x2, 0x72, 0x76, 0x5, 0x8, 
    0x5, 0x2, 0x73, 0x76, 0x5, 0x18, 0xd, 0x2, 0x74, 0x76, 0x7, 0x29, 0x2, 
    0x2, 0x75, 0x72, 0x3, 0x2, 0x2, 0x2, 0x75, 0x73, 0x3, 0x2, 0x2, 0x2, 
    0x75, 0x74, 0x3, 0x2, 0x2, 0x2, 0x76, 0x7, 0x3, 0x2, 0x2, 0x2, 0x77, 
    0x7a, 0x5, 0xa, 0x6, 0x2, 0x78, 0x7a, 0x5, 0x12, 0xa, 0x2, 0x79, 0x77, 
    0x3, 0x2, 0x2, 0x2, 0x79, 0x78, 0x3, 0x2, 0x2, 0x2, 0x7a, 0x9, 0x3, 
    0x2, 0x2, 0x2, 0x7b, 0x7c, 0x7, 0x19, 0x2, 0x2, 0x7c, 0x7d, 0x5, 0xc, 
    0x7, 0x2, 0x7d, 0x82, 0x5, 0xe, 0x8, 0x2, 0x7e, 0x7f, 0x7, 0x28, 0x2, 
    0x2, 0x7f, 0x81, 0x5, 0xe, 0x8, 0x2, 0x80, 0x7e, 0x3, 0x2, 0x2, 0x2, 
    0x81, 0x84, 0x3, 0x2, 0x2, 0x2, 0x82, 0x80, 0x3, 0x2, 0x2, 0x2, 0x82, 
    0x83, 0x3, 0x2, 0x2, 0x2, 0x83, 0x85, 0x3, 0x2, 0x2, 0x2, 0x84, 0x82, 
    0x3, 0x2, 0x2, 0x2, 0x85, 0x86, 0x7, 0x29, 0x2, 0x2, 0x86, 0xb, 0x3, 
    0x2, 0x2, 0x2, 0x87, 0x88, 0x9, 0x2, 0x2, 0x2, 0x88, 0xd, 0x3, 0x2, 
    0x2, 0x2, 0x89, 0x90, 0x7, 0x3a, 0x2, 0x2, 0x8a, 0x8b, 0x7, 0x24, 0x2, 
    0x2, 0x8b, 0x8c, 0x5, 0x38, 0x1d, 0x2, 0x8c, 0x8d, 0x7, 0x25, 0x2, 0x2, 
    0x8d, 0x8f, 0x3, 0x2, 0x2, 0x2, 0x8e, 0x8a, 0x3, 0x2, 0x2, 0x2, 0x8f, 
    0x92, 0x3, 0x2, 0x2, 0x2, 0x90, 0x8e, 0x3, 0x2, 0x2, 0x2, 0x90, 0x91, 
    0x3, 0x2, 0x2, 0x2, 0x91, 0x93, 0x3, 0x2, 0x2, 0x2, 0x92, 0x90, 0x3, 
    0x2, 0x2, 0x2, 0x93, 0x94, 0x7, 0x4, 0x2, 0x2, 0x94, 0x95, 0x5, 0x10, 
    0x9, 0x2, 0x95, 0xf, 0x3, 0x2, 0x2, 0x2, 0x96, 0xa4, 0x5, 0x38, 0x1d, 
    0x2, 0x97, 0xa0, 0x7, 0x26, 0x2, 0x2, 0x98, 0x9d, 0x5, 0x10, 0x9, 0x2, 
    0x99, 0x9a, 0x7, 0x28, 0x2, 0x2, 0x9a, 0x9c, 0x5, 0x10, 0x9, 0x2, 0x9b, 
    0x99, 0x3, 0x2, 0x2, 0x2, 0x9c, 0x9f, 0x3, 0x2, 0x2, 0x2, 0x9d, 0x9b, 
    0x3, 0x2, 0x2, 0x2, 0x9d, 0x9e, 0x3, 0x2, 0x2, 0x2, 0x9e, 0xa1, 0x3, 
    0x2, 0x2, 0x2, 0x9f, 0x9d, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x98, 0x3, 0x2, 
    0x2, 0x2, 0xa0, 0xa1, 0x3, 0x2, 0x2, 0x2, 0xa1, 0xa2, 0x3, 0x2, 0x2, 
    0x2, 0xa2, 0xa4, 0x7, 0x27, 0x2, 0x2, 0xa3, 0x96, 0x3, 0x2, 0x2, 0x2, 
    0xa3, 0x97, 0x3, 0x2, 0x2, 0x2, 0xa4, 0x11, 0x3, 0x2, 0x2, 0x2, 0xa5, 
    0xa6, 0x5, 0xc, 0x7, 0x2, 0xa6, 0xab, 0x5, 0x14, 0xb, 0x2, 0xa7, 0xa8, 
    0x7, 0x28, 0x2, 0x2, 0xa8, 0xaa, 0x5, 0x14, 0xb, 0x2, 0xa9, 0xa7, 0x3, 
    0x2, 0x2, 0x2, 0xaa, 0xad, 0x3, 0x2, 0x2, 0x2, 0xab, 0xa9, 0x3, 0x2, 
    0x2, 0x2, 0xab, 0xac, 0x3, 0x2, 0x2, 0x2, 0xac, 0xae, 0x3, 0x2, 0x2, 
    0x2, 0xad, 0xab, 0x3, 0x2, 0x2, 0x2, 0xae, 0xaf, 0x7, 0x29, 0x2, 0x2, 
    0xaf, 0x13, 0x3, 0x2, 0x2, 0x2, 0xb0, 0xb7, 0x7, 0x3a, 0x2, 0x2, 0xb1, 
    0xb2, 0x7, 0x24, 0x2, 0x2, 0xb2, 0xb3, 0x5, 0x38, 0x1d, 0x2, 0xb3, 0xb4, 
    0x7, 0x25, 0x2, 0x2, 0xb4, 0xb6, 0x3, 0x2, 0x2, 0x2, 0xb5, 0xb1, 0x3, 
    0x2, 0x2, 0x2, 0xb6, 0xb9, 0x3, 0x2, 0x2, 0x2, 0xb7, 0xb5, 0x3, 0x2, 
    0x2, 0x2, 0xb7, 0xb8, 0x3, 0x2, 0x2, 0x2, 0xb8, 0xc7, 0x3, 0x2, 0x2, 
    0x2, 0xb9, 0xb7, 0x3, 0x2, 0x2, 0x2, 0xba, 0xc1, 0x7, 0x3a, 0x2, 0x2, 
    0xbb, 0xbc, 0x7, 0x24, 0x2, 0x2, 0xbc, 0xbd, 0x5, 0x38, 0x1d, 0x2, 0xbd, 
    0xbe, 0x7, 0x25, 0x2, 0x2, 0xbe, 0xc0, 0x3, 0x2, 0x2, 0x2, 0xbf, 0xbb, 
    0x3, 0x2, 0x2, 0x2, 0xc0, 0xc3, 0x3, 0x2, 0x2, 0x2, 0xc1, 0xbf, 0x3, 
    0x2, 0x2, 0x2, 0xc1, 0xc2, 0x3, 0x2, 0x2, 0x2, 0xc2, 0xc4, 0x3, 0x2, 
    0x2, 0x2, 0xc3, 0xc1, 0x3, 0x2, 0x2, 0x2, 0xc4, 0xc5, 0x7, 0x4, 0x2, 
    0x2, 0xc5, 0xc7, 0x5, 0x16, 0xc, 0x2, 0xc6, 0xb0, 0x3, 0x2, 0x2, 0x2, 
    0xc6, 0xba, 0x3, 0x2, 0x2, 0x2, 0xc7, 0x15, 0x3, 0x2, 0x2, 0x2, 0xc8, 
    0xd6, 0x5, 0x36, 0x1c, 0x2, 0xc9, 0xd2, 0x7, 0x26, 0x2, 0x2, 0xca, 0xcf, 
    0x5, 0x16, 0xc, 0x2, 0xcb, 0xcc, 0x7, 0x28, 0x2, 0x2, 0xcc, 0xce, 0x5, 
    0x16, 0xc, 0x2, 0xcd, 0xcb, 0x3, 0x2, 0x2, 0x2, 0xce, 0xd1, 0x3, 0x2, 
    0x2, 0x2, 0xcf, 0xcd, 0x3, 0x2, 0x2, 0x2, 0xcf, 0xd0, 0x3, 0x2, 0x2, 
    0x2, 0xd0, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xd1, 0xcf, 0x3, 0x2, 0x2, 0x2, 
    0xd2, 0xca, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xd3, 
    0xd4, 0x3, 0x2, 0x2, 0x2, 0xd4, 0xd6, 0x7, 0x27, 0x2, 0x2, 0xd5, 0xc8, 
    0x3, 0x2, 0x2, 0x2, 0xd5, 0xc9, 0x3, 0x2, 0x2, 0x2, 0xd6, 0x17, 0x3, 
    0x2, 0x2, 0x2, 0xd7, 0xd8, 0x5, 0x1a, 0xe, 0x2, 0xd8, 0xd9, 0x7, 0x3a, 
    0x2, 0x2, 0xd9, 0xdb, 0x7, 0x22, 0x2, 0x2, 0xda, 0xdc, 0x5, 0x1c, 0xf, 
    0x2, 0xdb, 0xda, 0x3, 0x2, 0x2, 0x2, 0xdb, 0xdc, 0x3, 0x2, 0x2, 0x2, 
    0xdc, 0xdd, 0x3, 0x2, 0x2, 0x2, 0xdd, 0xde, 0x7, 0x23, 0x2, 0x2, 0xde, 
    0xdf, 0x5, 0x20, 0x11, 0x2, 0xdf, 0x19, 0x3, 0x2, 0x2, 0x2, 0xe0, 0xe1, 
    0x9, 0x3, 0x2, 0x2, 0xe1, 0x1b, 0x3, 0x2, 0x2, 0x2, 0xe2, 0xe7, 0x5, 
    0x1e, 0x10, 0x2, 0xe3, 0xe4, 0x7, 0x28, 0x2, 0x2, 0xe4, 0xe6, 0x5, 0x1e, 
    0x10, 0x2, 0xe5, 0xe3, 0x3, 0x2, 0x2, 0x2, 0xe6, 0xe9, 0x3, 0x2, 0x2, 
    0x2, 0xe7, 0xe5, 0x3, 0x2, 0x2, 0x2, 0xe7, 0xe8, 0x3, 0x2, 0x2, 0x2, 
    0xe8, 0x1d, 0x3, 0x2, 0x2, 0x2, 0xe9, 0xe7, 0x3, 0x2, 0x2, 0x2, 0xea, 
    0xeb, 0x5, 0xc, 0x7, 0x2, 0xeb, 0xf7, 0x7, 0x3a, 0x2, 0x2, 0xec, 0xed, 
    0x7, 0x24, 0x2, 0x2, 0xed, 0xf4, 0x7, 0x25, 0x2, 0x2, 0xee, 0xef, 0x7, 
    0x24, 0x2, 0x2, 0xef, 0xf0, 0x5, 0x38, 0x1d, 0x2, 0xf0, 0xf1, 0x7, 0x25, 
    0x2, 0x2, 0xf1, 0xf3, 0x3, 0x2, 0x2, 0x2, 0xf2, 0xee, 0x3, 0x2, 0x2, 
    0x2, 0xf3, 0xf6, 0x3, 0x2, 0x2, 0x2, 0xf4, 0xf2, 0x3, 0x2, 0x2, 0x2, 
    0xf4, 0xf5, 0x3, 0x2, 0x2, 0x2, 0xf5, 0xf8, 0x3, 0x2, 0x2, 0x2, 0xf6, 
    0xf4, 0x3, 0x2, 0x2, 0x2, 0xf7, 0xec, 0x3, 0x2, 0x2, 0x2, 0xf7, 0xf8, 
    0x3, 0x2, 0x2, 0x2, 0xf8, 0x1f, 0x3, 0x2, 0x2, 0x2, 0xf9, 0xfb, 0x7, 
    0x26, 0x2, 0x2, 0xfa, 0xfc, 0x5, 0x22, 0x12, 0x2, 0xfb, 0xfa, 0x3, 0x2, 
    0x2, 0x2, 0xfb, 0xfc, 0x3, 0x2, 0x2, 0x2, 0xfc, 0xfd, 0x3, 0x2, 0x2, 
    0x2, 0xfd, 0xfe, 0x7, 0x27, 0x2, 0x2, 0xfe, 0x21, 0x3, 0x2, 0x2, 0x2, 
    0xff, 0x101, 0x5, 0x24, 0x13, 0x2, 0x100, 0xff, 0x3, 0x2, 0x2, 0x2, 
    0x101, 0x102, 0x3, 0x2, 0x2, 0x2, 0x102, 0x100, 0x3, 0x2, 0x2, 0x2, 
    0x102, 0x103, 0x3, 0x2, 0x2, 0x2, 0x103, 0x23, 0x3, 0x2, 0x2, 0x2, 0x104, 
    0x107, 0x5, 0x8, 0x5, 0x2, 0x105, 0x107, 0x5, 0x26, 0x14, 0x2, 0x106, 
    0x104, 0x3, 0x2, 0x2, 0x2, 0x106, 0x105, 0x3, 0x2, 0x2, 0x2, 0x107, 
    0x25, 0x3, 0x2, 0x2, 0x2, 0x108, 0x10f, 0x5, 0x28, 0x15, 0x2, 0x109, 
    0x10f, 0x5, 0x2c, 0x17, 0x2, 0x10a, 0x10f, 0x5, 0x2e, 0x18, 0x2, 0x10b, 
    0x10f, 0x5, 0x30, 0x19, 0x2, 0x10c, 0x10f, 0x5, 0x32, 0x1a, 0x2, 0x10d, 
    0x10f, 0x5, 0x34, 0x1b, 0x2, 0x10e, 0x108, 0x3, 0x2, 0x2, 0x2, 0x10e, 
    0x109, 0x3, 0x2, 0x2, 0x2, 0x10e, 0x10a, 0x3, 0x2, 0x2, 0x2, 0x10e, 
    0x10b, 0x3, 0x2, 0x2, 0x2, 0x10e, 0x10c, 0x3, 0x2, 0x2, 0x2, 0x10e, 
    0x10d, 0x3, 0x2, 0x2, 0x2, 0x10f, 0x27, 0x3, 0x2, 0x2, 0x2, 0x110, 0x111, 
    0x5, 0x3a, 0x1e, 0x2, 0x111, 0x112, 0x5, 0x2a, 0x16, 0x2, 0x112, 0x113, 
    0x5, 0x36, 0x1c, 0x2, 0x113, 0x114, 0x7, 0x29, 0x2, 0x2, 0x114, 0x29, 
    0x3, 0x2, 0x2, 0x2, 0x115, 0x116, 0x9, 0x4, 0x2, 0x2, 0x116, 0x2b, 0x3, 
    0x2, 0x2, 0x2, 0x117, 0x119, 0x5, 0x36, 0x1c, 0x2, 0x118, 0x117, 0x3, 
    0x2, 0x2, 0x2, 0x118, 0x119, 0x3, 0x2, 0x2, 0x2, 0x119, 0x11a, 0x3, 
    0x2, 0x2, 0x2, 0x11a, 0x11b, 0x7, 0x29, 0x2, 0x2, 0x11b, 0x2d, 0x3, 
    0x2, 0x2, 0x2, 0x11c, 0x11d, 0x5, 0x20, 0x11, 0x2, 0x11d, 0x2f, 0x3, 
    0x2, 0x2, 0x2, 0x11e, 0x11f, 0x7, 0x1b, 0x2, 0x2, 0x11f, 0x120, 0x7, 
    0x22, 0x2, 0x2, 0x120, 0x121, 0x5, 0x66, 0x34, 0x2, 0x121, 0x122, 0x7, 
    0x23, 0x2, 0x2, 0x122, 0x125, 0x5, 0x26, 0x14, 0x2, 0x123, 0x124, 0x7, 
    0x1c, 0x2, 0x2, 0x124, 0x126, 0x5, 0x26, 0x14, 0x2, 0x125, 0x123, 0x3, 
    0x2, 0x2, 0x2, 0x125, 0x126, 0x3, 0x2, 0x2, 0x2, 0x126, 0x31, 0x3, 0x2, 
    0x2, 0x2, 0x127, 0x128, 0x7, 0x1d, 0x2, 0x2, 0x128, 0x129, 0x7, 0x22, 
    0x2, 0x2, 0x129, 0x12a, 0x5, 0x66, 0x34, 0x2, 0x12a, 0x12b, 0x7, 0x23, 
    0x2, 0x2, 0x12b, 0x12c, 0x5, 0x26, 0x14, 0x2, 0x12c, 0x136, 0x3, 0x2, 
    0x2, 0x2, 0x12d, 0x12e, 0x7, 0x1f, 0x2, 0x2, 0x12e, 0x12f, 0x5, 0x26, 
    0x14, 0x2, 0x12f, 0x130, 0x7, 0x1d, 0x2, 0x2, 0x130, 0x131, 0x7, 0x22, 
    0x2, 0x2, 0x131, 0x132, 0x5, 0x66, 0x34, 0x2, 0x132, 0x133, 0x7, 0x23, 
    0x2, 0x2, 0x133, 0x134, 0x7, 0x29, 0x2, 0x2, 0x134, 0x136, 0x3, 0x2, 
    0x2, 0x2, 0x135, 0x127, 0x3, 0x2, 0x2, 0x2, 0x135, 0x12d, 0x3, 0x2, 
    0x2, 0x2, 0x136, 0x33, 0x3, 0x2, 0x2, 0x2, 0x137, 0x138, 0x7, 0x21, 
    0x2, 0x2, 0x138, 0x141, 0x7, 0x29, 0x2, 0x2, 0x139, 0x13a, 0x7, 0x20, 
    0x2, 0x2, 0x13a, 0x141, 0x7, 0x29, 0x2, 0x2, 0x13b, 0x13d, 0x7, 0x1a, 
    0x2, 0x2, 0x13c, 0x13e, 0x5, 0x36, 0x1c, 0x2, 0x13d, 0x13c, 0x3, 0x2, 
    0x2, 0x2, 0x13d, 0x13e, 0x3, 0x2, 0x2, 0x2, 0x13e, 0x13f, 0x3, 0x2, 
    0x2, 0x2, 0x13f, 0x141, 0x7, 0x29, 0x2, 0x2, 0x140, 0x137, 0x3, 0x2, 
    0x2, 0x2, 0x140, 0x139, 0x3, 0x2, 0x2, 0x2, 0x140, 0x13b, 0x3, 0x2, 
    0x2, 0x2, 0x141, 0x35, 0x3, 0x2, 0x2, 0x2, 0x142, 0x143, 0x5, 0x66, 
    0x34, 0x2, 0x143, 0x37, 0x3, 0x2, 0x2, 0x2, 0x144, 0x145, 0x5, 0x66, 
    0x34, 0x2, 0x145, 0x39, 0x3, 0x2, 0x2, 0x2, 0x146, 0x14d, 0x7, 0x3a, 
    0x2, 0x2, 0x147, 0x148, 0x7, 0x24, 0x2, 0x2, 0x148, 0x149, 0x5, 0x36, 
    0x1c, 0x2, 0x149, 0x14a, 0x7, 0x25, 0x2, 0x2, 0x14a, 0x14c, 0x3, 0x2, 
    0x2, 0x2, 0x14b, 0x147, 0x3, 0x2, 0x2, 0x2, 0x14c, 0x14f, 0x3, 0x2, 
    0x2, 0x2, 0x14d, 0x14b, 0x3, 0x2, 0x2, 0x2, 0x14d, 0x14e, 0x3, 0x2, 
    0x2, 0x2, 0x14e, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x14f, 0x14d, 0x3, 0x2, 0x2, 
    0x2, 0x150, 0x151, 0x7, 0x22, 0x2, 0x2, 0x151, 0x152, 0x5, 0x36, 0x1c, 
    0x2, 0x152, 0x153, 0x7, 0x23, 0x2, 0x2, 0x153, 0x157, 0x3, 0x2, 0x2, 
    0x2, 0x154, 0x157, 0x5, 0x3a, 0x1e, 0x2, 0x155, 0x157, 0x5, 0x3e, 0x20, 
    0x2, 0x156, 0x150, 0x3, 0x2, 0x2, 0x2, 0x156, 0x154, 0x3, 0x2, 0x2, 
    0x2, 0x156, 0x155, 0x3, 0x2, 0x2, 0x2, 0x157, 0x3d, 0x3, 0x2, 0x2, 0x2, 
    0x158, 0x15b, 0x7, 0x38, 0x2, 0x2, 0x159, 0x15b, 0x7, 0x39, 0x2, 0x2, 
    0x15a, 0x158, 0x3, 0x2, 0x2, 0x2, 0x15a, 0x159, 0x3, 0x2, 0x2, 0x2, 
    0x15b, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x15c, 0x161, 0x5, 0x42, 0x22, 0x2, 
    0x15d, 0x15e, 0x7, 0x28, 0x2, 0x2, 0x15e, 0x160, 0x5, 0x42, 0x22, 0x2, 
    0x15f, 0x15d, 0x3, 0x2, 0x2, 0x2, 0x160, 0x163, 0x3, 0x2, 0x2, 0x2, 
    0x161, 0x15f, 0x3, 0x2, 0x2, 0x2, 0x161, 0x162, 0x3, 0x2, 0x2, 0x2, 
    0x162, 0x41, 0x3, 0x2, 0x2, 0x2, 0x163, 0x161, 0x3, 0x2, 0x2, 0x2, 0x164, 
    0x165, 0x5, 0x36, 0x1c, 0x2, 0x165, 0x43, 0x3, 0x2, 0x2, 0x2, 0x166, 
    0x171, 0x5, 0x3c, 0x1f, 0x2, 0x167, 0x168, 0x7, 0x3a, 0x2, 0x2, 0x168, 
    0x16a, 0x7, 0x22, 0x2, 0x2, 0x169, 0x16b, 0x5, 0x40, 0x21, 0x2, 0x16a, 
    0x169, 0x3, 0x2, 0x2, 0x2, 0x16a, 0x16b, 0x3, 0x2, 0x2, 0x2, 0x16b, 
    0x16c, 0x3, 0x2, 0x2, 0x2, 0x16c, 0x171, 0x7, 0x23, 0x2, 0x2, 0x16d, 
    0x16e, 0x5, 0x46, 0x24, 0x2, 0x16e, 0x16f, 0x5, 0x44, 0x23, 0x2, 0x16f, 
    0x171, 0x3, 0x2, 0x2, 0x2, 0x170, 0x166, 0x3, 0x2, 0x2, 0x2, 0x170, 
    0x167, 0x3, 0x2, 0x2, 0x2, 0x170, 0x16d, 0x3, 0x2, 0x2, 0x2, 0x171, 
    0x45, 0x3, 0x2, 0x2, 0x2, 0x172, 0x173, 0x9, 0x5, 0x2, 0x2, 0x173, 0x47, 
    0x3, 0x2, 0x2, 0x2, 0x174, 0x17a, 0x5, 0x44, 0x23, 0x2, 0x175, 0x176, 
    0x5, 0x4a, 0x26, 0x2, 0x176, 0x177, 0x5, 0x44, 0x23, 0x2, 0x177, 0x179, 
    0x3, 0x2, 0x2, 0x2, 0x178, 0x175, 0x3, 0x2, 0x2, 0x2, 0x179, 0x17c, 
    0x3, 0x2, 0x2, 0x2, 0x17a, 0x178, 0x3, 0x2, 0x2, 0x2, 0x17a, 0x17b, 
    0x3, 0x2, 0x2, 0x2, 0x17b, 0x49, 0x3, 0x2, 0x2, 0x2, 0x17c, 0x17a, 0x3, 
    0x2, 0x2, 0x2, 0x17d, 0x17e, 0x9, 0x6, 0x2, 0x2, 0x17e, 0x4b, 0x3, 0x2, 
    0x2, 0x2, 0x17f, 0x185, 0x5, 0x48, 0x25, 0x2, 0x180, 0x181, 0x5, 0x4e, 
    0x28, 0x2, 0x181, 0x182, 0x5, 0x48, 0x25, 0x2, 0x182, 0x184, 0x3, 0x2, 
    0x2, 0x2, 0x183, 0x180, 0x3, 0x2, 0x2, 0x2, 0x184, 0x187, 0x3, 0x2, 
    0x2, 0x2, 0x185, 0x183, 0x3, 0x2, 0x2, 0x2, 0x185, 0x186, 0x3, 0x2, 
    0x2, 0x2, 0x186, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x187, 0x185, 0x3, 0x2, 0x2, 
    0x2, 0x188, 0x189, 0x9, 0x7, 0x2, 0x2, 0x189, 0x4f, 0x3, 0x2, 0x2, 0x2, 
    0x18a, 0x190, 0x5, 0x4c, 0x27, 0x2, 0x18b, 0x18c, 0x5, 0x52, 0x2a, 0x2, 
    0x18c, 0x18d, 0x5, 0x4c, 0x27, 0x2, 0x18d, 0x18f, 0x3, 0x2, 0x2, 0x2, 
    0x18e, 0x18b, 0x3, 0x2, 0x2, 0x2, 0x18f, 0x192, 0x3, 0x2, 0x2, 0x2, 
    0x190, 0x18e, 0x3, 0x2, 0x2, 0x2, 0x190, 0x191, 0x3, 0x2, 0x2, 0x2, 
    0x191, 0x51, 0x3, 0x2, 0x2, 0x2, 0x192, 0x190, 0x3, 0x2, 0x2, 0x2, 0x193, 
    0x194, 0x9, 0x8, 0x2, 0x2, 0x194, 0x53, 0x3, 0x2, 0x2, 0x2, 0x195, 0x19b, 
    0x5, 0x50, 0x29, 0x2, 0x196, 0x197, 0x5, 0x56, 0x2c, 0x2, 0x197, 0x198, 
    0x5, 0x50, 0x29, 0x2, 0x198, 0x19a, 0x3, 0x2, 0x2, 0x2, 0x199, 0x196, 
    0x3, 0x2, 0x2, 0x2, 0x19a, 0x19d, 0x3, 0x2, 0x2, 0x2, 0x19b, 0x199, 
    0x3, 0x2, 0x2, 0x2, 0x19b, 0x19c, 0x3, 0x2, 0x2, 0x2, 0x19c, 0x55, 0x3, 
    0x2, 0x2, 0x2, 0x19d, 0x19b, 0x3, 0x2, 0x2, 0x2, 0x19e, 0x19f, 0x9, 
    0x9, 0x2, 0x2, 0x19f, 0x57, 0x3, 0x2, 0x2, 0x2, 0x1a0, 0x1a6, 0x5, 0x54, 
    0x2b, 0x2, 0x1a1, 0x1a2, 0x5, 0x5a, 0x2e, 0x2, 0x1a2, 0x1a3, 0x5, 0x54, 
    0x2b, 0x2, 0x1a3, 0x1a5, 0x3, 0x2, 0x2, 0x2, 0x1a4, 0x1a1, 0x3, 0x2, 
    0x2, 0x2, 0x1a5, 0x1a8, 0x3, 0x2, 0x2, 0x2, 0x1a6, 0x1a4, 0x3, 0x2, 
    0x2, 0x2, 0x1a6, 0x1a7, 0x3, 0x2, 0x2, 0x2, 0x1a7, 0x59, 0x3, 0x2, 0x2, 
    0x2, 0x1a8, 0x1a6, 0x3, 0x2, 0x2, 0x2, 0x1a9, 0x1aa, 0x9, 0xa, 0x2, 
    0x2, 0x1aa, 0x5b, 0x3, 0x2, 0x2, 0x2, 0x1ab, 0x1b0, 0x5, 0x58, 0x2d, 
    0x2, 0x1ac, 0x1ad, 0x7, 0xf, 0x2, 0x2, 0x1ad, 0x1af, 0x5, 0x58, 0x2d, 
    0x2, 0x1ae, 0x1ac, 0x3, 0x2, 0x2, 0x2, 0x1af, 0x1b2, 0x3, 0x2, 0x2, 
    0x2, 0x1b0, 0x1ae, 0x3, 0x2, 0x2, 0x2, 0x1b0, 0x1b1, 0x3, 0x2, 0x2, 
    0x2, 0x1b1, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x1b2, 0x1b0, 0x3, 0x2, 0x2, 0x2, 
    0x1b3, 0x1b8, 0x5, 0x5c, 0x2f, 0x2, 0x1b4, 0x1b5, 0x7, 0x13, 0x2, 0x2, 
    0x1b5, 0x1b7, 0x5, 0x5c, 0x2f, 0x2, 0x1b6, 0x1b4, 0x3, 0x2, 0x2, 0x2, 
    0x1b7, 0x1ba, 0x3, 0x2, 0x2, 0x2, 0x1b8, 0x1b6, 0x3, 0x2, 0x2, 0x2, 
    0x1b8, 0x1b9, 0x3, 0x2, 0x2, 0x2, 0x1b9, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x1ba, 
    0x1b8, 0x3, 0x2, 0x2, 0x2, 0x1bb, 0x1c0, 0x5, 0x5e, 0x30, 0x2, 0x1bc, 
    0x1bd, 0x7, 0x14, 0x2, 0x2, 0x1bd, 0x1bf, 0x5, 0x5e, 0x30, 0x2, 0x1be, 
    0x1bc, 0x3, 0x2, 0x2, 0x2, 0x1bf, 0x1c2, 0x3, 0x2, 0x2, 0x2, 0x1c0, 
    0x1be, 0x3, 0x2, 0x2, 0x2, 0x1c0, 0x1c1, 0x3, 0x2, 0x2, 0x2, 0x1c1, 
    0x61, 0x3, 0x2, 0x2, 0x2, 0x1c2, 0x1c0, 0x3, 0x2, 0x2, 0x2, 0x1c3, 0x1c8, 
    0x5, 0x60, 0x31, 0x2, 0x1c4, 0x1c5, 0x7, 0x30, 0x2, 0x2, 0x1c5, 0x1c7, 
    0x5, 0x60, 0x31, 0x2, 0x1c6, 0x1c4, 0x3, 0x2, 0x2, 0x2, 0x1c7, 0x1ca, 
    0x3, 0x2, 0x2, 0x2, 0x1c8, 0x1c6, 0x3, 0x2, 0x2, 0x2, 0x1c8, 0x1c9, 
    0x3, 0x2, 0x2, 0x2, 0x1c9, 0x63, 0x3, 0x2, 0x2, 0x2, 0x1ca, 0x1c8, 0x3, 
    0x2, 0x2, 0x2, 0x1cb, 0x1d0, 0x5, 0x62, 0x32, 0x2, 0x1cc, 0x1cd, 0x7, 
    0x31, 0x2, 0x2, 0x1cd, 0x1cf, 0x5, 0x62, 0x32, 0x2, 0x1ce, 0x1cc, 0x3, 
    0x2, 0x2, 0x2, 0x1cf, 0x1d2, 0x3, 0x2, 0x2, 0x2, 0x1d0, 0x1ce, 0x3, 
    0x2, 0x2, 0x2, 0x1d0, 0x1d1, 0x3, 0x2, 0x2, 0x2, 0x1d1, 0x65, 0x3, 0x2, 
    0x2, 0x2, 0x1d2, 0x1d0, 0x3, 0x2, 0x2, 0x2, 0x1d3, 0x1d9, 0x5, 0x64, 
    0x33, 0x2, 0x1d4, 0x1d5, 0x7, 0x15, 0x2, 0x2, 0x1d5, 0x1d6, 0x5, 0x36, 
    0x1c, 0x2, 0x1d6, 0x1d7, 0x7, 0x16, 0x2, 0x2, 0x1d7, 0x1d8, 0x5, 0x66, 
    0x34, 0x2, 0x1d8, 0x1da, 0x3, 0x2, 0x2, 0x2, 0x1d9, 0x1d4, 0x3, 0x2, 
    0x2, 0x2, 0x1d9, 0x1da, 0x3, 0x2, 0x2, 0x2, 0x1da, 0x67, 0x3, 0x2, 0x2, 
    0x2, 0x30, 0x69, 0x70, 0x75, 0x79, 0x82, 0x90, 0x9d, 0xa0, 0xa3, 0xab, 
    0xb7, 0xc1, 0xc6, 0xcf, 0xd2, 0xd5, 0xdb, 0xe7, 0xf4, 0xf7, 0xfb, 0x102, 
    0x106, 0x10e, 0x118, 0x125, 0x135, 0x13d, 0x140, 0x14d, 0x156, 0x15a, 
    0x161, 0x16a, 0x170, 0x17a, 0x185, 0x190, 0x19b, 0x1a6, 0x1b0, 0x1b8, 
    0x1c0, 0x1c8, 0x1d0, 0x1d9, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

SysYParser::Initializer SysYParser::_init;
