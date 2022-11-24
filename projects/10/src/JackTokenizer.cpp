#include "JackTokenizer.h"
#include <algorithm>
#include <iostream>

// TODO: make currentToken a type of any.

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

  // skip spaces
  while (fileContent[cursor] == ' ') {
    cursor++;
  }
  char c = fileContent[cursor];
  currentToken = "";
  currentTokenType = "";
  if (IsSymbol(c)) {
    currentToken = c;
    currentTokenType = "SYMBOL";
    cursor++;
  } else if (IsDigit(c)) {
    currentToken = c;
    while (IsDigit(fileContent[++cursor])) {
      currentToken += fileContent[cursor];
    }
    currentTokenType = "INT_CONST";
  } else if (IsLetter(c)) {
    currentToken = c;
    while (IsLetterOrDigitOr_(fileContent[++cursor])) {
      currentToken += fileContent[cursor];
    }
    if (IsKeyword(currentToken)) {
      currentTokenType = "KEYWORD";
      // cout << "KEYWORD: " << currentToken << endl;
    } else {
      currentTokenType = "IDENTIFIER";
    }
  } else if (c == '"') {
    currentToken = "";
    while (fileContent[++cursor] != '"') {
      currentToken += fileContent[cursor];
    }
    currentTokenType = "STRING_CONST";
    cursor++;
  } else {
    throw std::runtime_error("Invalid char: " + std::to_string(int(c)));
  }
}

std::string JackTokenizer::TokenType() { return currentTokenType; }

std::string JackTokenizer::Keyword() {
  if (currentTokenType == "KEYWORD") {
    return currentToken;
  } else {
    throw std::runtime_error("Current token is not a keyword");
  }
}

std::string JackTokenizer::Symbol() {
  if (currentTokenType == "SYMBOL") {
    if (currentToken == "<") {
      return "&lt;";
    } else if (currentToken == ">") {
      return "&gt;";
    } else if (currentToken == "\"") {
      return "&quot;";
    } else if (currentToken == "&") {
      return "&amp;";
    } else {
      return currentToken;
    }
  } else {
    throw std::runtime_error("Current token is not a symbol");
  }
}

std::string JackTokenizer::Identifier() {
  if (currentTokenType == "IDENTIFIER") {
    return currentToken;
  } else {
    throw std::runtime_error("Current token is not an identifier");
  }
}

int JackTokenizer::IntVal() {
  if (currentTokenType == "INT_CONST") {
    return std::stoi(currentToken);
  } else {
    throw std::runtime_error("Current token is not an integer");
  }
}

std::string JackTokenizer::StringVal() {
  if (currentTokenType == "STRING_CONST") {
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
  return c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' ||
         c == '.' || c == ',' || c == ';' || c == '+' || c == '-' || c == '*' ||
         c == '/' || c == '&' || c == '|' || c == '<' || c == '>' || c == '=' ||
         c == '~';
}

bool JackTokenizer::IsKeyword(std::string s) {
  return s == "class" || s == "constructor" || s == "function" ||
         s == "method" || s == "field" || s == "static" || s == "var" ||
         s == "int" || s == "char" || s == "boolean" || s == "void" ||
         s == "true" || s == "false" || s == "null" || s == "this" ||
         s == "let" || s == "do" || s == "if" || s == "else" || s == "while" ||
         s == "return";
}
