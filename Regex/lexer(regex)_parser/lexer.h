#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <regex>
#include "token.h"

using namespace std;

// Defines a pattern for the lexer to match.
// Each pattern has a regex, a corresponding token type, and a flag for keywords.
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

    // Initializes the list of regex patterns for tokens.
    void initTokenPatterns();
    // Moves to the next character in the input string.
    void advance();
    // Looks at the next character without advancing the position.
    char peek();

public:
    // Constructor initializes the lexer with source code.
    Lexer(const string& src);
    // The main function that scans the input and produces a vector of tokens.
    vector<Token> tokenize();
};

#endif // LEXER_H
