#ifndef Token_H
#define Token_H
#include <string>
using namespace std;
enum class TokenType {
    T_Function,T_INT,T_FLOAT,T_STRING,T_BOOL,
    T_IF,T_ELSE,T_WHILE,T_FOR,T_RETURN,T_CHAR, //Yahn pr keywords wali shitt ay gi
    T_IDENTIFIER, T_INLIT,T_STRINGLIT, //yahn pr identifiers and literals wali cheez include  karni ha
    T_ASSSSIGNOP,T_EQUALSOP,T_NOTEQUALS,T_LESSEQUAL,T_GREATEREQUAL,T_AND,T_OR,T_PLUS,T_MINUS,T_MULTIPLY,T_DIVIDE,T_MOD,// OPERATORS
    T_PARENL,T_PARENR,T_BRACEL,T_BRACER,T_SEMICOLON,T_COMMA,T_QOUTES,T_BRACKETL,T_BRACKETR, //Punctuators
    T_EOF,T_ERROR
};

struct Token {
    TokenType type;
    string value;
    int line;
    int column;
    Token(TokenType type, const string&  v="", int line=1, int column=1);
    string  toString()const;
};

#endif