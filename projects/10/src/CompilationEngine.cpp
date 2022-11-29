#include "CompilationEngine.h"
#include <iostream>

using namespace std;

CompilationEngine::CompilationEngine(JackTokenizer *JackTokenizer,
                                     const std::string &outputName) {
  this->jackTokenizer = JackTokenizer;
  // open output file
  outputFile.open(outputName);

  if (!outputFile.is_open()) {
    throw std::runtime_error("Failed to open output file");
  }
}

CompilationEngine::~CompilationEngine() { outputFile.close(); }

void CompilationEngine::Compile() {
  jackTokenizer->Advance();
  CompileClass();
}

void CompilationEngine::CompileClass() {
  EatBeginTag("class");
  // class
  EatKeyword();
  // className
  EatIdentifier();
  // {
  EatSymbol();
  // classVarDec*
  while (jackTokenizer->TokenType() == TOKEN_TYPE::KEYWORD &&
         (jackTokenizer->Keyword() == KEYWORD_TYPE::STATIC ||
          jackTokenizer->Keyword() == KEYWORD_TYPE::FIELD)) {
    CompileClassVarDeC();
  }
  // subroutine
  while (jackTokenizer->TokenType() == TOKEN_TYPE::KEYWORD &&
         (jackTokenizer->Keyword() == KEYWORD_TYPE::CONSTRUCTOR ||
          jackTokenizer->Keyword() == KEYWORD_TYPE::FUNCTION ||
          jackTokenizer->Keyword() == KEYWORD_TYPE::METHOD)) {
    CompileSubroutine();
  }
  // }
  EatSymbol();

  EatEndTag("class");
}

void CompilationEngine::CompileClassVarDeC() {
  EatBeginTag("classVarDec");
  // static | field
  EatKeyword();
  // type
  EatType();
  // varName
  EatIdentifier();
  // (, varName)*
  while (jackTokenizer->TokenType() == TOKEN_TYPE::SYMBOL &&
         jackTokenizer->Symbol() == SYMBOL_TYPE::COMMA) {
    // ,
    EatSymbol();
    // varName
    EatIdentifier();
  }
  // ;
  EatSymbol();
  EatEndTag("classVarDec");
}

void CompilationEngine::CompileSubroutine() {
  EatBeginTag("subroutineDec");
  // constructor | function | method
  EatKeyword();
  // void | type
  EatType();
  // subroutineName
  EatIdentifier();
  // (
  EatSymbol();
  // parameterList
  CompileParameterList();
  // )
  EatSymbol();

  // subroutineBody
  EatBeginTag("subroutineBody");
  // {
  EatSymbol();
  // varDec*
  while (jackTokenizer->TokenType() == TOKEN_TYPE::KEYWORD &&
         jackTokenizer->Keyword() == KEYWORD_TYPE::VAR) {
    CompileVarDeC();
  }
  // statements
  CompileStatements();
  // }
  EatSymbol();
  EatEndTag("subroutineBody");

  EatEndTag("subroutineDec");
}

void CompilationEngine::CompileSubroutineCall() {
  // subroutineName
  EatIdentifier();
  if (jackTokenizer->Symbol() == SYMBOL_TYPE::DOT) {
    // .
    EatSymbol();
    // subroutineName
    EatIdentifier();
  }
  // (
  EatSymbol();
  // expressionList
  CompileExpressionList();
  // )
  EatSymbol();
}

void CompilationEngine::CompileParameterList() {
  EatBeginTag("parameterList");
  // ((type varName) (, type varName)*)?
  while (jackTokenizer->TokenType() != TOKEN_TYPE::SYMBOL ||
         jackTokenizer->Symbol() != SYMBOL_TYPE::RIGHT_PAREN) {
    // type
    EatType();
    // varName
    EatIdentifier();
    if (jackTokenizer->Symbol() == SYMBOL_TYPE::COMMA) {
      // ,
      EatSymbol();
    }
  }
  EatEndTag("parameterList");
}

void CompilationEngine::CompileVarDeC() {
  EatBeginTag("varDec");
  // static | field
  EatKeyword();
  // type
  EatType();
  // varName
  EatIdentifier();
  // (, varName)*
  while (jackTokenizer->TokenType() == TOKEN_TYPE::SYMBOL &&
         jackTokenizer->Symbol() == SYMBOL_TYPE::COMMA) {
    // ,
    EatSymbol();
    // varName
    EatIdentifier();
  }
  // ;
  EatSymbol();
  EatEndTag("varDec");
}

void CompilationEngine::CompileStatements() {
  EatBeginTag("statements");
  // statement*
  while (jackTokenizer->TokenType() == TOKEN_TYPE::KEYWORD) {
    if (jackTokenizer->Keyword() == KEYWORD_TYPE::LET) {
      CompileLet();
    } else if (jackTokenizer->Keyword() == KEYWORD_TYPE::IF) {
      CompileIf();
    } else if (jackTokenizer->Keyword() == KEYWORD_TYPE::WHILE) {
      CompileWhile();
    } else if (jackTokenizer->Keyword() == KEYWORD_TYPE::DO) {
      CompileDo();
    } else if (jackTokenizer->Keyword() == KEYWORD_TYPE::RETURN) {
      CompileReturn();
    } else {
      break;
    }
  }
  EatEndTag("statements");
}

void CompilationEngine::CompileDo() {
  EatBeginTag("doStatement");
  // do
  EatKeyword();
  // subroutineCall
  CompileSubroutineCall();
  // ;
  EatSymbol();
  EatEndTag("doStatement");
}

void CompilationEngine::CompileLet() {
  EatBeginTag("letStatement");
  // let
  EatKeyword();
  // varName
  EatIdentifier();
  // ( [ expression ] )?
  if (jackTokenizer->Symbol() == SYMBOL_TYPE::LEFT_BRACKET) {
    // [
    EatSymbol();
    // expression
    CompileExpression();
    // ]
    EatSymbol();
  }
  // =
  EatSymbol();
  // expression
  CompileExpression();
  // ;
  EatSymbol();
  EatEndTag("letStatement");
}

void CompilationEngine::CompileWhile() {
  EatBeginTag("whileStatement");
  // while
  EatKeyword();
  // (
  EatSymbol();
  // expression
  CompileExpression();
  // )
  EatSymbol();
  // {
  EatSymbol();
  // statements
  CompileStatements();
  // }
  EatSymbol();
  EatEndTag("whileStatement");
}

void CompilationEngine::CompileReturn() {
  EatBeginTag("returnStatement");
  // return
  EatKeyword();
  // expression?
  if (jackTokenizer->TokenType() != TOKEN_TYPE::SYMBOL ||
      jackTokenizer->Symbol() != SYMBOL_TYPE::SEMICOLON) {
    CompileExpression();
  }
  // ;
  EatSymbol();
  EatEndTag("returnStatement");
}

void CompilationEngine::CompileIf() {
  EatBeginTag("ifStatement");
  // if
  EatKeyword();
  // (
  EatSymbol();
  // expression
  CompileExpression();
  // )
  EatSymbol();
  // {
  EatSymbol();
  // statements
  CompileStatements();
  // }
  EatSymbol();
  // (else { statements })?
  if (jackTokenizer->Keyword() == KEYWORD_TYPE::ELSE) {
    // else
    EatKeyword();
    // {
    EatSymbol();
    // statements
    CompileStatements();
    // }
    EatSymbol();
  }
  EatEndTag("ifStatement");
}

void CompilationEngine::CompileExpression() {
  EatBeginTag("expression");
  // term
  CompileTerm();
  // (op term)*
  while (jackTokenizer->TokenType() == TOKEN_TYPE::SYMBOL &&
         (jackTokenizer->Symbol() == SYMBOL_TYPE::PLUS ||
          jackTokenizer->Symbol() == SYMBOL_TYPE::MINUS ||
          jackTokenizer->Symbol() == SYMBOL_TYPE::STAR ||
          jackTokenizer->Symbol() == SYMBOL_TYPE::SLASH ||
          jackTokenizer->Symbol() == SYMBOL_TYPE::AMPERSAND ||
          jackTokenizer->Symbol() == SYMBOL_TYPE::VERTICAL_BAR ||
          jackTokenizer->Symbol() == SYMBOL_TYPE::LESS_THAN ||
          jackTokenizer->Symbol() == SYMBOL_TYPE::GREATER_THAN ||
          jackTokenizer->Symbol() == SYMBOL_TYPE::EQUAL)) {
    // op
    EatSymbol();
    // term
    CompileTerm();
  }
  EatEndTag("expression");
}

void CompilationEngine::CompileTerm() {
  EatBeginTag("term");
  // integerConstant | stringConstant | keywordConstant | varName |
  // varName[expression] | subroutineCall | (expression) | unaryOp term

  // integerConstant | stringConstant | keywordConstant
  if (jackTokenizer->TokenType() == TOKEN_TYPE::INT_CONST) {
    EatIntegerConstant();
  } else if (jackTokenizer->TokenType() == TOKEN_TYPE::STRING_CONST) {
    EatStringConstant();
  } else if (jackTokenizer->TokenType() == TOKEN_TYPE::KEYWORD) {
    EatKeyword();
  } else if (jackTokenizer->TokenType() == TOKEN_TYPE::IDENTIFIER) {
    // varName | varName[expression] | subroutineCall
    jackTokenizer->Advance();
    if (jackTokenizer->Symbol() == SYMBOL_TYPE::LEFT_BRACKET) {
      // varName
      jackTokenizer->Retreat();
      EatIdentifier();
      // [
      EatSymbol();
      // expression
      CompileExpression();
      // ]
      EatSymbol();
    } else if (jackTokenizer->Symbol() == SYMBOL_TYPE::LEFT_PAREN ||
               jackTokenizer->Symbol() == SYMBOL_TYPE::DOT) {

      // subroutineCall
      jackTokenizer->Retreat();
      CompileSubroutineCall();
    } else {
      // varName
      jackTokenizer->Retreat();
      EatIdentifier();
    }
  } else if (jackTokenizer->Symbol() == SYMBOL_TYPE::LEFT_PAREN) {
    // (expression)
    // (
    EatSymbol();
    // expression
    CompileExpression();
    // )
    EatSymbol();
  } else if (jackTokenizer->Symbol() == SYMBOL_TYPE::MINUS ||
             jackTokenizer->Symbol() == SYMBOL_TYPE::TILDE) {
    // unaryOp term
    // unaryOp
    EatSymbol();
    // term
    CompileTerm();
  }
  EatEndTag("term");
}

void CompilationEngine::CompileExpressionList() {
  EatBeginTag("expressionList");
  // (expression (, expression)*)?

  if (jackTokenizer->TokenType() != TOKEN_TYPE::SYMBOL ||
      jackTokenizer->Symbol() != SYMBOL_TYPE::RIGHT_PAREN) {
    CompileExpression();

    while (jackTokenizer->TokenType() == TOKEN_TYPE::SYMBOL &&
           jackTokenizer->Symbol() == SYMBOL_TYPE::COMMA) {
      // ,
      EatSymbol();
      // expression
      CompileExpression();
    }
  }
  EatEndTag("expressionList");
}

void CompilationEngine::EatKeyword() {
  outputFile << "<keyword> " << jackTokenizer->KeywordString() << " </keyword>"
             << endl;
  jackTokenizer->Advance();
}

void CompilationEngine::EatSymbol() {
  SYMBOL_TYPE symbol = jackTokenizer->Symbol();
  string symbolString;
  if (symbol == SYMBOL_TYPE::LESS_THAN) {
    symbolString = "&lt;";
  } else if (symbol == SYMBOL_TYPE::GREATER_THAN) {
    symbolString = "&gt;";
  } else if (symbol == SYMBOL_TYPE::AMPERSAND) {
    symbolString = "&amp;";
  } else {
    symbolString = jackTokenizer->SymbolString();
  }
  outputFile << "<symbol> " << symbolString << " </symbol>" << endl;

  jackTokenizer->Advance();
}

void CompilationEngine::EatIdentifier() {
  outputFile << "<identifier> " << jackTokenizer->Identifier()
             << " </identifier>" << std::endl;

  jackTokenizer->Advance();
}

void CompilationEngine::EatIntegerConstant() {
  outputFile << "<integerConstant> " << jackTokenizer->IntVal()
             << " </integerConstant>" << std::endl;
  jackTokenizer->Advance();
}

void CompilationEngine::EatStringConstant() {
  outputFile << "<stringConstant> " << jackTokenizer->StringVal()
             << " </stringConstant>" << std::endl;
  jackTokenizer->Advance();
}

void CompilationEngine::EatType() {
  if (jackTokenizer->TokenType() == TOKEN_TYPE::KEYWORD) {
    EatKeyword();
  } else if (jackTokenizer->TokenType() == TOKEN_TYPE::IDENTIFIER) {
    EatIdentifier();
  }
}

void CompilationEngine::EatBeginTag(const std::string &tag) {
  outputFile << "<" << tag << ">" << std::endl;
}

void CompilationEngine::EatEndTag(const std::string &tag) {
  outputFile << "</" << tag << ">" << std::endl;
}
