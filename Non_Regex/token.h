#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string> 

using namespace std;

enum class TokenType {
    // Keywords
    T_FUNCTION, T_INT, T_FLOAT, T_STRING, T_BOOL, T_CHAR, 
    T_IF, T_ELSE, T_ELIF, T_WHILE, T_FOR, T_DO, 
    T_SWITCH, T_CASE, T_DEFAULT,
    T_RETURN, T_BREAK, T_CONTINUE, 
    T_PRINT, T_READ, 
    T_TRUE, T_FALSE, T_NULL, T_VOID, T_INCLUDE,

    // Arithmetic Operators
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_MODULO,

    // Relational Operators
    T_EQUAL, T_NOT_EQUAL, T_GREATER_THAN, T_LESS_THAN, 
    T_GREATER_THAN_EQUAL, T_LESS_THAN_EQUAL,

    // Logical Operators
    T_AND, T_OR, T_NOT,

    // Bitwise Operators
    T_BIT_AND, T_BIT_OR, T_BIT_XOR, T_BIT_NOT, 
    T_LEFT_SHIFT, T_RIGHT_SHIFT,

    // Assignment Operators
    T_ASSIGN, 
    T_PLUS_ASSIGN, T_MINUS_ASSIGN, T_MULT_ASSIGN, 
    T_DIV_ASSIGN, T_MOD_ASSIGN, 
    T_AND_ASSIGN, T_OR_ASSIGN, T_XOR_ASSIGN, 
    T_LSHIFT_ASSIGN, T_RSHIFT_ASSIGN,

    // Increment/Decrement
    T_INCREMENT, T_DECREMENT,

    // Punctuation / Delimiters
    T_COMMA, T_SEMICOLON, T_COLON, T_DOT, 
    T_LPAREN, T_RPAREN, 
    T_LBRACE, T_RBRACE, 
    T_LBRACKET, T_RBRACKET, 
    T_QUESTION, T_QUOTES,

    // Literals
    T_INT_LITERAL, T_FLOAT_LITERAL, T_STRING_LITERAL, 
    T_CHAR_LITERAL, T_BOOL_LITERAL, T_NULL_LITERAL,
    T_HEX_LITERAL, T_OCTAL_LITERAL, T_BINARY_LITERAL,

    // Identifiers
    T_IDENTIFIER, 

    // Special
    T_EOF, T_ERROR
};


class Token {
    private:
    TokenType type;
    string type_str;
    string lexeme;
    int line;
    int column;
    string getTokenName(TokenType token)const;
    
    public:
    Token(TokenType type, const string& value, int line, int column);
    string toString() const;
    void print() const;
    TokenType getType() const { return type; }
    string getLexeme() const { return lexeme; }
    int getLine() const { return line; }
    int getColumn() const { return column; }
};

#endif