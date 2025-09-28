#include "token.h"

Token::Token(TokenType type, const string& value, int line, int column)
    : type(type), lexeme(value), line(line), column(column) {
    type_str = getTokenName(type);
}

string Token::toString() const {
    return "[" + type_str + "(\"" + lexeme + "\"), Line: " + to_string(line) + ", Col: " + to_string(column) + "]";
}

void Token::print() const {
    cout << type_str + "(" + lexeme + ")";
}

string Token::getTokenName(TokenType token) const {
    string typeName;

    switch (token) {
        // Keywords
        case TokenType::T_FUNCTION: typeName = "T_FUNCTION"; break;
        case TokenType::T_INT: typeName = "T_INT"; break;
        case TokenType::T_FLOAT: typeName = "T_FLOAT"; break;
        case TokenType::T_STRING: typeName = "T_STRING"; break;
        case TokenType::T_BOOL: typeName = "T_BOOL"; break;
        case TokenType::T_CHAR: typeName = "T_CHAR"; break;
        case TokenType::T_IF: typeName = "T_IF"; break;
        case TokenType::T_ELSE: typeName = "T_ELSE"; break;
        case TokenType::T_ELIF: typeName = "T_ELIF"; break;
        case TokenType::T_WHILE: typeName = "T_WHILE"; break;
        case TokenType::T_FOR: typeName = "T_FOR"; break;
        case TokenType::T_DO: typeName = "T_DO"; break;
        case TokenType::T_SWITCH: typeName = "T_SWITCH"; break;
        case TokenType::T_CASE: typeName = "T_CASE"; break;
        case TokenType::T_DEFAULT: typeName = "T_DEFAULT"; break;
        case TokenType::T_RETURN: typeName = "T_RETURN"; break;
        case TokenType::T_BREAK: typeName = "T_BREAK"; break;
        case TokenType::T_CONTINUE: typeName = "T_CONTINUE"; break;
        case TokenType::T_PRINT: typeName = "T_PRINT"; break;
        case TokenType::T_READ: typeName = "T_READ"; break;
        case TokenType::T_TRUE: typeName = "T_TRUE"; break;
        case TokenType::T_FALSE: typeName = "T_FALSE"; break;
        case TokenType::T_NULL: typeName = "T_NULL"; break;
        case TokenType::T_VOID: typeName = "T_VOID"; break;
        case TokenType::T_INCLUDE: typeName = "T_INCLUDE"; break;

        // Arithmetic
        case TokenType::T_PLUS: typeName = "T_PLUS"; break;
        case TokenType::T_MINUS: typeName = "T_MINUS"; break;
        case TokenType::T_MULTIPLY: typeName = "T_MULTIPLY"; break;
        case TokenType::T_DIVIDE: typeName = "T_DIVIDE"; break;
        case TokenType::T_MODULO: typeName = "T_MODULO"; break;

        // Relational
        case TokenType::T_EQUAL: typeName = "T_EQUAL"; break;
        case TokenType::T_NOT_EQUAL: typeName = "T_NOT_EQUAL"; break;
        case TokenType::T_GREATER_THAN: typeName = "T_GREATER_THAN"; break;
        case TokenType::T_LESS_THAN: typeName = "T_LESS_THAN"; break;
        case TokenType::T_GREATER_THAN_EQUAL: typeName = "T_GREATER_THAN_EQUAL"; break;
        case TokenType::T_LESS_THAN_EQUAL: typeName = "T_LESS_THAN_EQUAL"; break;

        // Logical
        case TokenType::T_AND: typeName = "T_AND"; break;
        case TokenType::T_OR: typeName = "T_OR"; break;
        case TokenType::T_NOT: typeName = "T_NOT"; break;

        // Bitwise
        case TokenType::T_BIT_AND: typeName = "T_BIT_AND"; break;
        case TokenType::T_BIT_OR: typeName = "T_BIT_OR"; break;
        case TokenType::T_BIT_XOR: typeName = "T_BIT_XOR"; break;
        case TokenType::T_BIT_NOT: typeName = "T_BIT_NOT"; break;
        case TokenType::T_LEFT_SHIFT: typeName = "T_LEFT_SHIFT"; break;
        case TokenType::T_RIGHT_SHIFT: typeName = "T_RIGHT_SHIFT"; break;

        // Assignment
        case TokenType::T_ASSIGN: typeName = "T_ASSIGN"; break;
        case TokenType::T_PLUS_ASSIGN: typeName = "T_PLUS_ASSIGN"; break;
        case TokenType::T_MINUS_ASSIGN: typeName = "T_MINUS_ASSIGN"; break;
        case TokenType::T_MULT_ASSIGN: typeName = "T_MULT_ASSIGN"; break;
        case TokenType::T_DIV_ASSIGN: typeName = "T_DIV_ASSIGN"; break;
        case TokenType::T_MOD_ASSIGN: typeName = "T_MOD_ASSIGN"; break;
        case TokenType::T_AND_ASSIGN: typeName = "T_AND_ASSIGN"; break;
        case TokenType::T_OR_ASSIGN: typeName = "T_OR_ASSIGN"; break;
        case TokenType::T_XOR_ASSIGN: typeName = "T_XOR_ASSIGN"; break;
        case TokenType::T_LSHIFT_ASSIGN: typeName = "T_LSHIFT_ASSIGN"; break;
        case TokenType::T_RSHIFT_ASSIGN: typeName = "T_RSHIFT_ASSIGN"; break;

        // Increment/Decrement
        case TokenType::T_INCREMENT: typeName = "T_INCREMENT"; break;
        case TokenType::T_DECREMENT: typeName = "T_DECREMENT"; break;

        // Punctuation
        case TokenType::T_COMMA: typeName = "T_COMMA"; break;
        case TokenType::T_SEMICOLON: typeName = "T_SEMICOLON"; break;
        case TokenType::T_COLON: typeName = "T_COLON"; break;
        case TokenType::T_DOT: typeName = "T_DOT"; break;
        case TokenType::T_LPAREN: typeName = "T_LPAREN"; break;
        case TokenType::T_RPAREN: typeName = "T_RPAREN"; break;
        case TokenType::T_LBRACE: typeName = "T_LBRACE"; break;
        case TokenType::T_RBRACE: typeName = "T_RBRACE"; break;
        case TokenType::T_LBRACKET: typeName = "T_LBRACKET"; break;
        case TokenType::T_RBRACKET: typeName = "T_RBRACKET"; break;
        case TokenType::T_QUESTION: typeName = "T_QUESTION"; break;
        case TokenType::T_QUOTES: typeName = "T_QUOTES"; break;

        // Literals
        case TokenType::T_INT_LITERAL: typeName = "T_INT_LITERAL"; break;
        case TokenType::T_FLOAT_LITERAL: typeName = "T_FLOAT_LITERAL"; break;
        case TokenType::T_STRING_LITERAL: typeName = "T_STRING_LITERAL"; break;
        case TokenType::T_CHAR_LITERAL: typeName = "T_CHAR_LITERAL"; break;
        case TokenType::T_BOOL_LITERAL: typeName = "T_BOOL_LITERAL"; break;
        case TokenType::T_NULL_LITERAL: typeName = "T_NULL_LITERAL"; break;
        case TokenType::T_HEX_LITERAL: typeName = "T_HEX_LITERAL"; break;
        case TokenType::T_OCTAL_LITERAL: typeName = "T_OCTAL_LITERAL"; break;
        case TokenType::T_BINARY_LITERAL: typeName = "T_BINARY_LITERAL"; break;

        // Identifiers
        case TokenType::T_IDENTIFIER: typeName = "T_IDENTIFIER"; break;

        // Special
        case TokenType::T_EOF: typeName = "T_EOF"; break;
        case TokenType::T_ERROR: typeName = "T_ERROR"; break;

        default: typeName = "UNKNOWN_TOKEN"; break;
    }

    return typeName;
}
