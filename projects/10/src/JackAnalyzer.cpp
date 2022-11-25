#include <iostream>
#include <string>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

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
      std::string outputFileName = fileOrDir.substr(0, fileOrDir.find_last_of(".")) + "_.xml";
      JackTokenizer jackTokenizer(fileOrDir);
      CompilationEngine compilationEngine(&jackTokenizer, outputFileName);
      compilationEngine.Compile();
    } catch (std::runtime_error e) {
      std::cout << "runtime error: " << e.what() << std::endl;
      return 1;
    }

  } else {

  }

  
  return 0;
}
