#include "token.h"
#include <sstream>
#include <map>
using namespace std;

static const map<TokenType, string> tokenTypeStrings = {
    // Keywords
    {TokenType::T_AUTO, "T_AUTO"},
    {TokenType::T_BREAK, "T_BREAK"},
    {TokenType::T_CASE, "T_CASE"},
    {TokenType::T_CHAR, "T_CHAR"},
    {TokenType::T_CONST, "T_CONST"},
    {TokenType::T_CONTINUE, "T_CONTINUE"},
    {TokenType::T_DEFAULT, "T_DEFAULT"},
    {TokenType::T_DO, "T_DO"},
    {TokenType::T_DOUBLE, "T_DOUBLE"},
    {TokenType::T_ELSE, "T_ELSE"},
    {TokenType::T_ENUM, "T_ENUM"},
    {TokenType::T_EXTERN, "T_EXTERN"},
    {TokenType::T_FLOAT, "T_FLOAT"},
    {TokenType::T_FOR, "T_FOR"},
    {TokenType::T_GOTO, "T_GOTO"},
    {TokenType::T_IF, "T_IF"},
    {TokenType::T_INT, "T_INT"},
    {TokenType::T_LONG, "T_LONG"},
    {TokenType::T_REGISTER, "T_REGISTER"},
    {TokenType::T_RETURN, "T_RETURN"},
    {TokenType::T_SHORT, "T_SHORT"},
    {TokenType::T_SIGNED, "T_SIGNED"},
    {TokenType::T_SIZEOF, "T_SIZEOF"},
    {TokenType::T_STATIC, "T_STATIC"},
    {TokenType::T_STRUCT, "T_STRUCT"},
    {TokenType::T_SWITCH, "T_SWITCH"},
    {TokenType::T_TYPEDEF, "T_TYPEDEF"},
    {TokenType::T_UNION, "T_UNION"},
    {TokenType::T_UNSIGNED, "T_UNSIGNED"},
    {TokenType::T_VOID, "T_VOID"},
    {TokenType::T_VOLATILE, "T_VOLATILE"},
    {TokenType::T_WHILE, "T_WHILE"},
    {TokenType::T_CLASS, "T_CLASS"},
    {TokenType::T_PUBLIC, "T_PUBLIC"},
    {TokenType::T_PRIVATE, "T_PRIVATE"},
    {TokenType::T_PROTECTED, "T_PROTECTED"},
    {TokenType::T_TEMPLATE, "T_TEMPLATE"},
    {TokenType::T_THIS, "T_THIS"},
    {TokenType::T_VIRTUAL, "T_VIRTUAL"},
    {TokenType::T_NEW, "T_NEW"},
    {TokenType::T_DELETE, "T_DELETE"},
    {TokenType::T_TRY, "T_TRY"},
    {TokenType::T_CATCH, "T_CATCH"},
    {TokenType::T_THROW, "T_THROW"},
    {TokenType::T_OPERATOR, "T_OPERATOR"},
    {TokenType::T_FRIEND, "T_FRIEND"},
    {TokenType::T_INLINE, "T_INLINE"},
    {TokenType::T_NAMESPACE, "T_NAMESPACE"},
    {TokenType::T_USING, "T_USING"},
    {TokenType::T_ASM, "T_ASM"},
    {TokenType::T_BOOL, "T_BOOL"},
    {TokenType::T_WCHAR_T, "T_WCHAR_T"},
    {TokenType::T_TYPEID, "T_TYPEID"},
    {TokenType::T_DYNAMIC_CAST, "T_DYNAMIC_CAST"},
    {TokenType::T_STATIC_CAST, "T_STATIC_CAST"},
    {TokenType::T_REINTERPRET_CAST, "T_REINTERPRET_CAST"},
    {TokenType::T_CONST_CAST, "T_CONST_CAST"},
    {TokenType::T_EXPLICIT, "T_EXPLICIT"},
    {TokenType::T_MUTABLE, "T_MUTABLE"},
    {TokenType::T_TRUE, "T_TRUE"},
    {TokenType::T_FALSE, "T_FALSE"},
    {TokenType::T_NULLPTR, "T_NULLPTR"},
    
    // Literals
    {TokenType::T_IDENTIFIER, "T_IDENTIFIER"},
    {TokenType::T_INTLIT, "T_INTLIT"},
    {TokenType::T_FLOATLIT, "T_FLOATLIT"},
    {TokenType::T_CHARLIT, "T_CHARLIT"},
    {TokenType::T_STRINGLIT, "T_STRINGLIT"},
    {TokenType::T_BOOLLIT, "T_BOOLLIT"},
    
    // Operators
    {TokenType::T_PLUS, "T_PLUS"},
    {TokenType::T_MINUS, "T_MINUS"},
    {TokenType::T_MULTIPLY, "T_MULTIPLY"},
    {TokenType::T_DIVIDE, "T_DIVIDE"},
    {TokenType::T_MOD, "T_MOD"},
    {TokenType::T_INCREMENT, "T_INCREMENT"},
    {TokenType::T_DECREMENT, "T_DECREMENT"},
    {TokenType::T_ASSIGN, "T_ASSIGN"},
    {TokenType::T_PLUS_ASSIGN, "T_PLUS_ASSIGN"},
    {TokenType::T_MINUS_ASSIGN, "T_MINUS_ASSIGN"},
    {TokenType::T_MULTIPLY_ASSIGN, "T_MULTIPLY_ASSIGN"},
    {TokenType::T_DIVIDE_ASSIGN, "T_DIVIDE_ASSIGN"},
    {TokenType::T_MOD_ASSIGN, "T_MOD_ASSIGN"},
    {TokenType::T_EQUAL, "T_EQUAL"},
    {TokenType::T_NOT_EQUAL, "T_NOT_EQUAL"},
    {TokenType::T_LESS, "T_LESS"},
    {TokenType::T_GREATER, "T_GREATER"},
    {TokenType::T_LESS_EQUAL, "T_LESS_EQUAL"},
    {TokenType::T_GREATER_EQUAL, "T_GREATER_EQUAL"},
    {TokenType::T_LOGICAL_AND, "T_LOGICAL_AND"},
    {TokenType::T_LOGICAL_OR, "T_LOGICAL_OR"},
    {TokenType::T_LOGICAL_NOT, "T_LOGICAL_NOT"},
    {TokenType::T_BITWISE_AND, "T_BITWISE_AND"},
    {TokenType::T_BITWISE_OR, "T_BITWISE_OR"},
    {TokenType::T_BITWISE_XOR, "T_BITWISE_XOR"},
    {TokenType::T_BITWISE_NOT, "T_BITWISE_NOT"},
    {TokenType::T_LEFT_SHIFT, "T_LEFT_SHIFT"},
    {TokenType::T_RIGHT_SHIFT, "T_RIGHT_SHIFT"},
    {TokenType::T_LEFT_SHIFT_ASSIGN, "T_LEFT_SHIFT_ASSIGN"},
    {TokenType::T_RIGHT_SHIFT_ASSIGN, "T_RIGHT_SHIFT_ASSIGN"},
    {TokenType::T_BITWISE_AND_ASSIGN, "T_BITWISE_AND_ASSIGN"},
    {TokenType::T_BITWISE_OR_ASSIGN, "T_BITWISE_OR_ASSIGN"},
    {TokenType::T_BITWISE_XOR_ASSIGN, "T_BITWISE_XOR_ASSIGN"},
    {TokenType::T_ARROW, "T_ARROW"},
    {TokenType::T_DOT, "T_DOT"},
    {TokenType::T_SCOPE, "T_SCOPE"},
    {TokenType::T_DEREFERENCE, "T_DEREFERENCE"},
    {TokenType::T_ADDRESS_OF, "T_ADDRESS_OF"},
    {TokenType::T_SIZEOF, "T_SIZEOF"},
    {TokenType::T_TERNARY, "T_TERNARY"},
    
    // Punctuators
    {TokenType::T_SEMICOLON, "T_SEMICOLON"},
    {TokenType::T_COLON, "T_COLON"},
    {TokenType::T_COMMA, "T_COMMA"},
    {TokenType::T_DOT, "T_DOT"},
    {TokenType::T_QUESTION, "T_QUESTION"},
    {TokenType::T_LPAREN, "T_LPAREN"},
    {TokenType::T_RPAREN, "T_RPAREN"},
    {TokenType::T_LBRACE, "T_LBRACE"},
    {TokenType::T_RBRACE, "T_RBRACE"},
    {TokenType::T_LBRACKET, "T_LBRACKET"},
    {TokenType::T_RBRACKET, "T_RBRACKET"},
    {TokenType::T_LANGBRACKET, "T_LANGBRACKET"},
    {TokenType::T_RANGBRACKET, "T_RANGBRACKET"},
    
    // Special
    {TokenType::T_EOF, "T_EOF"},
    {TokenType::T_ERROR, "T_ERROR"},
    {TokenType::T_COMMENT, "T_COMMENT"},
    {TokenType::T_WHITESPACE, "T_WHITESPACE"}
};

Token::Token(TokenType type, const string& v, int line, int column)
    : type(type), value(v), line(line), column(column) {}

string Token::toString() const {
    ostringstream oss;
    auto it = tokenTypeStrings.find(type);
    if (it != tokenTypeStrings.end()) {
        oss << "[" << it->second;
        if (!value.empty() && (type == TokenType::T_IDENTIFIER || 
                              type == TokenType::T_INTLIT ||
                              type == TokenType::T_FLOATLIT ||
                              type == TokenType::T_CHARLIT ||
                              type == TokenType::T_STRINGLIT ||
                              type == TokenType::T_BOOLLIT)) {
            oss << "(\"" << value << "\")";
        }
        oss << "]";
    } else {
        oss << "[UNKNOWN]";
    }
    return oss.str();
}