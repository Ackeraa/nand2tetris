#include "CompilationEngine.h"
#include <iostream>

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
  while (jackTokenizer->Keyword() == "static" ||
         jackTokenizer->Keyword() == "field") {
    CompileClassVarDeC();
  }
  // subroutine
  while (jackTokenizer->Keyword() == "constructor" ||
         jackTokenizer->Keyword() == "function" ||
         jackTokenizer->Keyword() == "method") {
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
  while (jackTokenizer->Symbol() == ",") {
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
  EatKeyword();
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
  while (jackTokenizer->Keyword() == "var") {
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
  if (jackTokenizer->Symbol() == ".") {
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
  while (jackTokenizer->Symbol() != ")") {
    // type
    EatType();
    // varName
    EatIdentifier();
    if (jackTokenizer->Symbol() == ",") {
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
  while (jackTokenizer->Symbol() == ",") {
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
  while (1) {
    if (jackTokenizer->Keyword() == "let") {
      CompileLet();
    } else if (jackTokenizer->Keyword() == "if") {
      CompileIf();
    } else if (jackTokenizer->Keyword() == "while") {
      CompileWhile();
    } else if (jackTokenizer->Keyword() == "do") {
      CompileDo();
    } else if (jackTokenizer->Keyword() == "return") {
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
  if (jackTokenizer->Symbol() == "[") {
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
  if (jackTokenizer->Symbol() != ";") {
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
  if (jackTokenizer->Keyword() == "else") {
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
  while (jackTokenizer->Symbol() == "+" || jackTokenizer->Symbol() == "-" ||
         jackTokenizer->Symbol() == "*" || jackTokenizer->Symbol() == "/" ||
         jackTokenizer->Symbol() == "&" || jackTokenizer->Symbol() == "|" ||
         jackTokenizer->Symbol() == "<" || jackTokenizer->Symbol() == ">" ||
         jackTokenizer->Symbol() == "=") {
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
  if (jackTokenizer->TokenType() == "INT_CONST") {
    EatIntegerConstant();
  } else if (jackTokenizer->TokenType() == "STRING_CONST") {
    EatStringConstant();
  } else if (jackTokenizer->TokenType() == "KEYWORD") {
    EatKeyword();
  } else if (jackTokenizer->TokenType() == "IDENTIFIER") {
    // varName | varName[expression] | subroutineCall
    jackTokenizer->Advance();
    if (jackTokenizer->Symbol() == "[") {
      // varName
      jackTokenizer->Retreat();
      EatIdentifier();
      // [
      EatSymbol();
      // expression
      CompileExpression();
      // ]
      EatSymbol();
    } else if (jackTokenizer->Symbol() == "(" ||
               jackTokenizer->Symbol() == ".") {
      // subroutineCall
      jackTokenizer->Retreat();
      CompileSubroutineCall();
    } else {
      // varName
      EatIdentifier();
    }
  } else if (jackTokenizer->Symbol() == "(") {
    // (expression)
    // (
    EatSymbol();
    // expression
    CompileExpression();
    // )
    EatSymbol();
  } else if (jackTokenizer->Symbol() == "-" || jackTokenizer->Symbol() == "~") {
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
  if (jackTokenizer->Symbol() != ")") {
    CompileExpression();

    while (jackTokenizer->Symbol() == ",") {
      // ,
      EatSymbol();
      // expression
      CompileExpression();
    }
  }
  EatEndTag("expressionList");
}

void CompilationEngine::EatKeyword() {
  outputFile << "<keyword> " << jackTokenizer->Keyword() << " </keyword>"
             << std::endl;
  jackTokenizer->Advance();
}

void CompilationEngine::EatSymbol() {
  outputFile << "<symbol> " << jackTokenizer->Symbol() << " </symbol>"
             << std::endl;
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
  if (jackTokenizer->TokenType() == "KEYWORD") {
    EatKeyword();
  } else if (jackTokenizer->TokenType() == "IDENTIFIER") {
    EatIdentifier();
  }
}

void CompilationEngine::EatBeginTag(const std::string &tag) {
  outputFile << "<" << tag << ">" << std::endl;
}

void CompilationEngine::EatEndTag(const std::string &tag) {
  outputFile << "</" << tag << ">" << std::endl;
}
