#include "token.h"
#include <sstream>
using namespace std;
Token::Token(TokenType type, const string& v, int line, int column)
    : type(type), value(v), line(line), column(column) {}

string Token::toString() const {
    ostringstream oss;
    switch ((type))
    {
    case TokenType::T_Function:return "T_Function";
    case TokenType::T_INT:return "[T_INT]";
    case TokenType::T_FLOAT:return "[T_FLOAT]";
    case TokenType::T_STRING:return "[T_STRING]";
    case TokenType::T_BOOL:return "[T_BOOL]";
    case TokenType::T_RETURN:return "[T_RETURN]";
    case TokenType::T_IF:return "[T_IF]";
    case TokenType::T_ELSE:return "[T_ELSE]";
    case TokenType::T_WHILE:return "[T_WHILE]";
    case TokenType::T_FOR:return "[T_FOR]";
    case TokenType::T_CHAR:return "[T_CHAR]";
    case TokenType::T_IDENTIFIER:return "[T_IDENTIFIER(\""+value+"\")]";
    case TokenType::T_INLIT:return "[T_INLIT(\""+value+"\")]";
    case TokenType::T_STRINGLIT:return "[T_STRINGLIT(\""+value+"\")]";
    case TokenType::T_ASSSSIGNOP:return "[T_ASSSSIGNOP]";
    case TokenType::T_EQUALSOP:return "[T_EQUALSOP]";
    case TokenType::T_NOTEQUALS:return "[T_NOTEQUALS]";
    case TokenType::T_LESSEQUAL:return "[T_LESSEQUAL]";
    case TokenType::T_GREATEREQUAL:return "[T_GREATEREQUAL]";
    case TokenType::T_AND:return "[T_AND]";
    case TokenType::T_OR:return "[T_OR]";
    case TokenType::T_PLUS:return "[T_PLUS]";
    case TokenType::T_MINUS:return "[T_MINUS]";
    case TokenType::T_MULTIPLY:return "[T_MULTIPLY]";
    case TokenType::T_DIVIDE:return "[T_DIVIDE]";
    case TokenType::T_MOD:return "[T_MOD]";
    case TokenType::T_PARENL:return "[T_PARENL]";
    case TokenType::T_PARENR:return "[T_PARENR]";
    case TokenType::T_BRACEL:return "[T_BRACEL]";
    case TokenType::T_BRACER:return "[T_BRACER]";
    case TokenType::T_SEMICOLON:return "[T_SEMICOLON]";
    case TokenType::T_COMMA:return "[T_COMMA]";
    case TokenType::T_QOUTES:return "[T_QOUTES]";
    case TokenType::T_BRACKETL:return "[T_BRACKETL]";
    case TokenType::T_BRACKETR:return "[T_BRACKETR]";
    case TokenType::T_EOF:return "[T_EOF]";
    case TokenType::T_ERROR:return "[T_ERROR]";
    default:return "[UNKNOWN]";
    }
}