#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

// Enum class for all possible token types in the language.
enum class TokenType {
    // Keywords
    T_AUTO, T_BREAK, T_CASE, T_CHAR, T_CONST, T_CONTINUE, T_DEFAULT, T_DO,
    T_DOUBLE, T_ELSE, T_ENUM, T_EXTERN, T_FLOAT, T_FOR, T_GOTO, T_IF,
    T_INT, T_LONG, T_REGISTER, T_RETURN, T_SHORT, T_SIGNED, T_SIZEOF, T_STATIC,
    T_STRUCT, T_SWITCH, T_TYPEDEF, T_UNION, T_UNSIGNED, T_VOID, T_VOLATILE, T_WHILE,
    T_CLASS, T_PUBLIC, T_PRIVATE, T_PROTECTED, T_TEMPLATE, T_THIS, T_VIRTUAL,
    T_NEW, T_DELETE, T_TRY, T_CATCH, T_THROW, T_OPERATOR, T_FRIEND, T_INLINE,
    T_NAMESPACE, T_USING, T_ASM, T_BOOL, T_WCHAR_T, T_TYPEID, T_DYNAMIC_CAST,
    T_STATIC_CAST, T_REINTERPRET_CAST, T_CONST_CAST, T_EXPLICIT, T_MUTABLE,
    T_TRUE, T_FALSE, T_NULLPTR,
    
    // Additional keywords for test cases
    T_FN, T_STRING,

    // Identifiers and Literals
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_CHARLIT, T_STRINGLIT, T_BOOLLIT,

    // Operators
    T_PLUS, T_MINUS, T_ASTERISK, T_DIVIDE, T_MOD, T_INCREMENT, T_DECREMENT,
    T_ASSIGN, T_PLUS_ASSIGN, T_MINUS_ASSIGN, T_MULTIPLY_ASSIGN, T_DIVIDE_ASSIGN,
    T_MOD_ASSIGN, T_EQUAL, T_NOT_EQUAL, T_LESS, T_GREATER, T_LESS_EQUAL,
    T_GREATER_EQUAL, T_LOGICAL_AND, T_LOGICAL_OR, T_LOGICAL_NOT, T_AMPERSAND,
    T_BITWISE_OR, T_BITWISE_XOR, T_BITWISE_NOT, T_LEFT_SHIFT, T_RIGHT_SHIFT,
    T_LEFT_SHIFT_ASSIGN, T_RIGHT_SHIFT_ASSIGN, T_BITWISE_AND_ASSIGN,
    T_BITWISE_OR_ASSIGN, T_BITWISE_XOR_ASSIGN, T_ARROW, T_SCOPE,
    T_ARROW_ASTERISK, T_DOT_ASTERISK, T_SPACESHIP, T_ELLIPSIS,

    // Punctuators
    T_SEMICOLON, T_LPAREN, T_RPAREN,
    T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    T_COLON, T_COMMA, T_DOT, T_QUESTION, T_HASH,

    // Special Tokens
    T_EOF, T_ERROR, T_COMMENT, T_WHITESPACE
};

// Represents a single token with its type, value, and position in the source.
struct Token {
    TokenType type;
    string value;
    int line;
    int column;

    Token(TokenType type, const string& v = "", int line = 0, int column = 0);
    string toString() const;
};

#endif // TOKEN_H