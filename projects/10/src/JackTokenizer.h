#pragma once
#include <fstream>
#include <string>
#include <unordered_map>
#include "Utils.h"

class JackTokenizer {
public:
  JackTokenizer(std::string fileName);
  ~JackTokenizer();
  bool HasMoreTokens();
  void Advance();
  TOKEN_TYPE TokenType();
  KEYWORD_TYPE Keyword();
  std::string KeywordString();
  SYMBOL_TYPE Symbol();
  std::string SymbolString();
  std::string Identifier();
  int IntVal();
  std::string StringVal();
  void Retreat();

private:
  std::string fileContent;
  std::string currentToken;
  enum TOKEN_TYPE currentTokenType;

  int cursor;
  int length;

  int lastCursor;
  std::string lastToken;
  enum TOKEN_TYPE lastTokenType;

  bool IsSymbol(char c);
  bool IsDigit(char c);
  bool IsLetter(char c);
  bool IsLetterOrDigitOr_(char c);
  bool IsKeyword(std::string s);
};
