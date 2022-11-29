#include "JackTokenizer.h"
#include <algorithm>
#include <iostream>

extern std::unordered_map<std::string, KEYWORD_TYPE> keywordMap;
extern std::unordered_map<std::string, SYMBOL_TYPE> symbolMap;

JackTokenizer::JackTokenizer(std::string fileName) {
  std::string line;
  std::ifstream myfile(fileName);
  std::string fileContent_ = "";
  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      // remove line comments
      if (line.find("//") != std::string::npos) {
        line = line.substr(0, line.find("//"));
      }
      // remove \r
      line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

      fileContent_ += line;
    }
    myfile.close();

    // remove block comments
    int n = fileContent_.length();
    int l = 0, r = 0;
    while (r < fileContent_.size()) {
      l = r;
      while (r < n && (fileContent_[r] != '/' || fileContent_[r + 1] != '*'))
        r++;
      fileContent += fileContent_.substr(l, r - l);
      r += 2;
      while (r < n && (fileContent_[r] != '*' || fileContent_[r + 1] != '/'))
        r++;
      r += 2;
    }

    cursor = 0;
    length = fileContent.length();
  } else {
    throw std::runtime_error("Unable to open file");
  }
}

JackTokenizer::~JackTokenizer() {}

bool JackTokenizer::HasMoreTokens() { return cursor < length; }

void JackTokenizer::Advance() {
  lastCursor = cursor;
  lastToken = currentToken;
  lastTokenType = currentTokenType;

  // skip spaces and tabs
  while (fileContent[cursor] == ' ' || fileContent[cursor] == '\t') {
    cursor++;
  }
  if (cursor >= length) {
    return;
  }
  char c = fileContent[cursor];
  currentToken = "";
  currentTokenType = TOKEN_TYPE::INVALID;
  if (IsSymbol(c)) {
    currentToken = c;
    currentTokenType = TOKEN_TYPE::SYMBOL;
    cursor++;
  } else if (IsDigit(c)) {
    currentToken = c;
    while (IsDigit(fileContent[++cursor])) {
      currentToken += fileContent[cursor];
    }
    currentTokenType = TOKEN_TYPE::INT_CONST;
  } else if (IsLetter(c)) {
    currentToken = c;
    while (IsLetterOrDigitOr_(fileContent[++cursor])) {
      currentToken += fileContent[cursor];
    }
    if (IsKeyword(currentToken)) {
      currentTokenType = TOKEN_TYPE::KEYWORD;
    } else {
      currentTokenType = TOKEN_TYPE::IDENTIFIER;
    }
  } else if (c == '"') {
    currentToken = "";
    while (fileContent[++cursor] != '"') {
      currentToken += fileContent[cursor];
    }
    currentTokenType = TOKEN_TYPE::STRING_CONST;
    cursor++;
  } else {
    throw std::runtime_error("Invalid char: " + std::to_string(int(c)));
  }
}

void JackTokenizer::Retreat() {
  if (cursor == 0) {
    throw std::runtime_error("Cannot retreat");
  }
  cursor = lastCursor;
  currentToken = lastToken;
  currentTokenType = lastTokenType;
}

enum TOKEN_TYPE JackTokenizer::TokenType() { return currentTokenType; }

KEYWORD_TYPE JackTokenizer::Keyword() {
  if (currentTokenType == TOKEN_TYPE::KEYWORD) {
    return keywordMap[currentToken];
  } else {
    throw std::runtime_error("Current token is not a keyword");
  }
}

std::string JackTokenizer::KeywordString() {
  if (currentTokenType == TOKEN_TYPE::KEYWORD) {
    return currentToken;
  } else {
    throw std::runtime_error("Current token is not a keyword");
  }
}

SYMBOL_TYPE JackTokenizer::Symbol() {
  if (currentTokenType == TOKEN_TYPE::SYMBOL) {
    return symbolMap[currentToken];
  } else {
    throw std::runtime_error("Current token is not a symbol");
  }
}

std::string JackTokenizer::SymbolString() {
  if (currentTokenType == TOKEN_TYPE::SYMBOL) {
    return currentToken;
  } else {
    throw std::runtime_error("Current token is not a symbol");
  }
}

std::string JackTokenizer::Identifier() {
  if (currentTokenType == TOKEN_TYPE::IDENTIFIER) {
    return currentToken;
  } else {
    throw std::runtime_error("Current token is not an identifier");
  }
}

int JackTokenizer::IntVal() {
  if (currentTokenType == TOKEN_TYPE::INT_CONST) {
    return std::stoi(currentToken);
  } else {
    throw std::runtime_error("Current token is not an integer");
  }
}

std::string JackTokenizer::StringVal() {
  if (currentTokenType == TOKEN_TYPE::STRING_CONST) {
    return currentToken;
  } else {
    throw std::runtime_error("Current token is not a string");
  }
}

bool JackTokenizer::IsDigit(char c) {
  // 0 - 9
  return c >= 48 && c <= 57;
}

bool JackTokenizer::IsLetter(char c) {
  // a-z, A - Z
  return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

bool JackTokenizer::IsLetterOrDigitOr_(char c) {
  return IsLetter(c) || IsDigit(c) || c == '_';
}

bool JackTokenizer::IsSymbol(char c) {
  // check if c in symbolMap
  return symbolMap.find(std::string(1, c)) != symbolMap.end();
}

bool JackTokenizer::IsKeyword(std::string s) {
  // check if s in keywordMap
  return keywordMap.find(s) != keywordMap.end();
}
