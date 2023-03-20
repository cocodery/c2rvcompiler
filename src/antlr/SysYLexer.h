
// Generated from SysY.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"




class  SysYLexer : public antlr4::Lexer {
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

  SysYLexer(antlr4::CharStream *input);
  ~SysYLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

