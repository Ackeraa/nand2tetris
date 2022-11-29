#include <iostream>
#include <string>
#include <filesystem>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

void CompileFile(std::string fileName) {
  std::string outputName = fileName.substr(0, fileName.find_last_of('.')) + ".xml_";
  JackTokenizer *jackTokenizer = new JackTokenizer(fileName);
  CompilationEngine *compilationEngine = new CompilationEngine(jackTokenizer, outputName);
  compilationEngine->Compile();
  delete compilationEngine;
  delete jackTokenizer;
}

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
      CompileFile(fileOrDir);
    } catch (std::runtime_error &e) {
      std::cout << e.what() << std::endl;
      return 1;
    }
  } else {
    // Iterate through all .jack files in the directory
    // and compile them
    for (const auto &entry : std::filesystem::directory_iterator(fileOrDir)) {
      std::string fileName = entry.path();
      if (fileName.substr(fileName.find_last_of(".") + 1) == "jack") {
        try {
          CompileFile(fileName);
        } catch (std::runtime_error &e) {
          std::cout << e.what() << std::endl;
          return 1;
        }
      }
    }
  }
  return 0;
}
