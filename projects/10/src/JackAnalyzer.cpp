#include <iostream>
#include <string>
#include "JackTokenizer.h"

int main(int argc, char *argv[]) {
  // check if argv[1] is a .jack file or a directory
  if (argc != 2) {
    std::cout << "Usage: JackAnalyzer <file.jack or directory>" << std::endl;
    return 1;
  } 
  std::string fileOrDir = argv[1];
  // check if fileOrDir end with .jack
  if (fileOrDir.substr(fileOrDir.find_last_of(".") + 1) == "jack") {
    try {
      JackTokenizer jackTokenizer(fileOrDir);
      std::string outputFileName = fileOrDir.substr(0, fileOrDir.find_last_of(".")) + "_T.xml";
      std::ofstream outputFile(outputFileName);
      outputFile << "<tokens>" << std::endl;
      while (jackTokenizer.HasMoreTokens()) {
        jackTokenizer.Advance();
        outputFile << "<" << jackTokenizer.TokenType() << "> ";
        if (jackTokenizer.TokenType() == "KEYWORD") {
          outputFile << jackTokenizer.Keyword();
        } else if (jackTokenizer.TokenType() == "SYMBOL") {
          outputFile << jackTokenizer.Symbol();
        } else if (jackTokenizer.TokenType() == "IDENTIFIER") {
          outputFile << jackTokenizer.Identifier();
        } else if (jackTokenizer.TokenType() == "INT_CONST") {
          outputFile << jackTokenizer.IntVal();
        } else if (jackTokenizer.TokenType() == "STRING_CONST") {
          outputFile << jackTokenizer.StringVal();
        }

        outputFile << " </" << jackTokenizer.TokenType() << ">" << std::endl;

      }
      outputFile << "</tokens>" << std::endl;
    } catch (std::runtime_error e) {
      std::cout << "runtime error: " << e.what() << std::endl;
      return 1;
    }

  } else {

  }

  
  return 0;
}
