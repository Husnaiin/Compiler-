#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include "token.h"
#include <regex>
using namespace std;
struct LexerTokenPattern {
    std::regex pattern;
    TokenType type;
    bool isKeyword;
    LexerTokenPattern(const string& regexStr, TokenType t, bool keyword = false) 
        : pattern(regexStr, std::regex_constants::ECMAScript), type(t), isKeyword(keyword) {}
};
class Lexer {
private:
    string input;
    size_t pos;
    int line;
    int column;
    char currentChar;
    vector<LexerTokenPattern> tokenPatterns;
    void initTokenPatterns();
    void skipWhitespaceAndComment();
    Token processString();
    Token processChar();
public:
    Lexer(const string& src);
    vector<Token> tokenize();   
};
#endif