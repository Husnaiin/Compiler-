#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class Lexer {
    private:
    string input;
    vector<Token> tokens;
    int start = 0; 
    int currentIndex = 0;
    int lineNumber = 1;
    int columnNumber = 1;
    int startColumn = 1;

    public:
    Lexer(const string& input, bool isFile);
    char peek() const;
    char peek(int offset) const;
    char advance();
    bool isAtEnd() const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isSpace(char c) const; 
    bool addToken(TokenType type, const string& lexeme);
    bool addToken(TokenType type);
    void scanNextToken();
    void scanNumber();
    void scanIdentifier();
    void scanString();
    void scanChar();
    TokenType getKeywordType(const string& text);

    vector<Token> tokenize();

};

#endif