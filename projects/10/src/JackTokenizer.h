#pragma once
#include <string>
#include <fstream>

class JackTokenizer {
public:
    JackTokenizer(std::string fileName);
    ~JackTokenizer();
    bool HasMoreTokens();
    void Advance();
    std::string TokenType();
    std::string GetToken();
    std::string Keyword();
    std::string Symbol();
    std::string Identifier();
    int IntVal();
    std::string StringVal();
    void Retreat();
private:
    std::string fileContent;
    std::string currentToken;
    std::string currentTokenType;
    int cursor;
    int length;

    int lastCursor;
    std::string lastToken;
    std::string lastTokenType;

    bool IsSymbol(char c);
    bool IsDigit(char c);
    bool IsLetter(char c);
    bool IsLetterOrDigitOr_(char c);
    bool IsKeyword(std::string s);

};


