#include <string>
#include <fstream>
#include "JackTokenizer.h"

class CompilationEngine {
public:
  CompilationEngine(JackTokenizer *JackTokenizer, const std::string &outputName);
  ~CompilationEngine();

  void Compile();
  void CompileClass();
  void CompileClassVarDeC();
  void CompileSubroutine();
  void CompileSubroutineCall();

  void CompileParameterList();
  void CompileVarDeC();
  void CompileStatements();
  void CompileDo();
  void CompileLet();
  void CompileWhile();
  void CompileReturn();
  void CompileIf();
  void CompileExpression();
  void CompileTerm();
  void CompileExpressionList();

private:
  void EatKeyword();
  void EatSymbol();
  void EatIdentifier();
  void EatType();
  void EatIntegerConstant();
  void EatStringConstant();

  void EatBeginTag(const std::string &tag);
  void EatEndTag(const std::string &tag);

private:
  JackTokenizer *jackTokenizer;
  std::ofstream outputFile;

};
