#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include "token.h"
using namespace std;
class Lexer {
private:
    string input;
    size_t pos;
    int line;
    int column;
    char currentChar;
    struct TokenPattern;
    vector<TokenPattern> tokenPatterns;
    void initTokenPatterns();
    void skipWhitespaceAndComment();
    Token processString();
public:
    Lexer(const string& src);
    vector<Token> tokenize();   
};
#endif