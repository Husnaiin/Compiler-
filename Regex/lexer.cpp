#include "lexer.h"
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <map>
using namespace std;
Lexer::Lexer(const string& src) : input(src), pos(0), line(1), column(1) {
    if (!input.empty()) {
        currentChar = input[0];
    }
    initTokenPatterns();
}
void Lexer::initTokenPatterns() {
    // C++ Keywords
    tokenPatterns.emplace_back("\\bauto\\b", TokenType::T_AUTO, true);
    tokenPatterns.emplace_back("\\bbreak\\b", TokenType::T_BREAK, true);
    tokenPatterns.emplace_back("\\bcase\\b", TokenType::T_CASE, true);
    tokenPatterns.emplace_back("\\bchar\\b", TokenType::T_CHAR, true);
    tokenPatterns.emplace_back("\\bconst\\b", TokenType::T_CONST, true);
    tokenPatterns.emplace_back("\\bcontinue\\b", TokenType::T_CONTINUE, true);
    tokenPatterns.emplace_back("\\bdefault\\b", TokenType::T_DEFAULT, true);
    tokenPatterns.emplace_back("\\bdo\\b", TokenType::T_DO, true);
    tokenPatterns.emplace_back("\\bdouble\\b", TokenType::T_DOUBLE, true);
    tokenPatterns.emplace_back("\\belse\\b", TokenType::T_ELSE, true);
    tokenPatterns.emplace_back("\\benum\\b", TokenType::T_ENUM, true);
    tokenPatterns.emplace_back("\\bextern\\b", TokenType::T_EXTERN, true);
    tokenPatterns.emplace_back("\\bfloat\\b", TokenType::T_FLOAT, true);
    tokenPatterns.emplace_back("\\bfor\\b", TokenType::T_FOR, true);
    tokenPatterns.emplace_back("\\bgoto\\b", TokenType::T_GOTO, true);
    tokenPatterns.emplace_back("\\bif\\b", TokenType::T_IF, true);
    tokenPatterns.emplace_back("\\bint\\b", TokenType::T_INT, true);
    tokenPatterns.emplace_back("\\blong\\b", TokenType::T_LONG, true);
    tokenPatterns.emplace_back("\\bregister\\b", TokenType::T_REGISTER, true);
    tokenPatterns.emplace_back("\\breturn\\b", TokenType::T_RETURN, true);
    tokenPatterns.emplace_back("\\bshort\\b", TokenType::T_SHORT, true);
    tokenPatterns.emplace_back("\\bsigned\\b", TokenType::T_SIGNED, true);
    tokenPatterns.emplace_back("\\bsizeof\\b", TokenType::T_SIZEOF, true);
    tokenPatterns.emplace_back("\\bstatic\\b", TokenType::T_STATIC, true);
    tokenPatterns.emplace_back("\\bstruct\\b", TokenType::T_STRUCT, true);
    tokenPatterns.emplace_back("\\bswitch\\b", TokenType::T_SWITCH, true);
    tokenPatterns.emplace_back("\\btypedef\\b", TokenType::T_TYPEDEF, true);
    tokenPatterns.emplace_back("\\bunion\\b", TokenType::T_UNION, true);
    tokenPatterns.emplace_back("\\bunsigned\\b", TokenType::T_UNSIGNED, true);
    tokenPatterns.emplace_back("\\bvoid\\b", TokenType::T_VOID, true);
    tokenPatterns.emplace_back("\\bvolatile\\b", TokenType::T_VOLATILE, true);
    tokenPatterns.emplace_back("\\bwhile\\b", TokenType::T_WHILE, true);
    tokenPatterns.emplace_back("\\bclass\\b", TokenType::T_CLASS, true);
    tokenPatterns.emplace_back("\\bpublic\\b", TokenType::T_PUBLIC, true);
    tokenPatterns.emplace_back("\\bprivate\\b", TokenType::T_PRIVATE, true);
    tokenPatterns.emplace_back("\\bprotected\\b", TokenType::T_PROTECTED, true);
    tokenPatterns.emplace_back("\\btemplate\\b", TokenType::T_TEMPLATE, true);
    tokenPatterns.emplace_back("\\bthis\\b", TokenType::T_THIS, true);
    tokenPatterns.emplace_back("\\bvirtual\\b", TokenType::T_VIRTUAL, true);
    tokenPatterns.emplace_back("\\bnew\\b", TokenType::T_NEW, true);
    tokenPatterns.emplace_back("\\bdelete\\b", TokenType::T_DELETE, true);
    tokenPatterns.emplace_back("\\btry\\b", TokenType::T_TRY, true);
    tokenPatterns.emplace_back("\\bcatch\\b", TokenType::T_CATCH, true);
    tokenPatterns.emplace_back("\\bthrow\\b", TokenType::T_THROW, true);
    tokenPatterns.emplace_back("\\boperator\\b", TokenType::T_OPERATOR, true);
    tokenPatterns.emplace_back("\\bfriend\\b", TokenType::T_FRIEND, true);
    tokenPatterns.emplace_back("\\binline\\b", TokenType::T_INLINE, true);
    tokenPatterns.emplace_back("\\bnamespace\\b", TokenType::T_NAMESPACE, true);
    tokenPatterns.emplace_back("\\busing\\b", TokenType::T_USING, true);
    tokenPatterns.emplace_back("\\basm\\b", TokenType::T_ASM, true);
    tokenPatterns.emplace_back("\\bbool\\b", TokenType::T_BOOL, true);
    tokenPatterns.emplace_back("\\bwchar_t\\b", TokenType::T_WCHAR_T, true);
    tokenPatterns.emplace_back("\\btypeid\\b", TokenType::T_TYPEID, true);
    tokenPatterns.emplace_back("\\bdynamic_cast\\b", TokenType::T_DYNAMIC_CAST, true);
    tokenPatterns.emplace_back("\\bstatic_cast\\b", TokenType::T_STATIC_CAST, true);
    tokenPatterns.emplace_back("\\breinterpret_cast\\b", TokenType::T_REINTERPRET_CAST, true);
    tokenPatterns.emplace_back("\\bconst_cast\\b", TokenType::T_CONST_CAST, true);
    tokenPatterns.emplace_back("\\bexplicit\\b", TokenType::T_EXPLICIT, true);
    tokenPatterns.emplace_back("\\bmutable\\b", TokenType::T_MUTABLE, true);
    tokenPatterns.emplace_back("\\btrue\\b", TokenType::T_TRUE, true);
    tokenPatterns.emplace_back("\\bfalse\\b", TokenType::T_FALSE, true);
    tokenPatterns.emplace_back("\\bnullptr\\b", TokenType::T_NULLPTR, true);

    // Operators - Multi-character first
    tokenPatterns.emplace_back("\\+\\+", TokenType::T_INCREMENT);
    tokenPatterns.emplace_back("--", TokenType::T_DECREMENT);
    tokenPatterns.emplace_back("==", TokenType::T_EQUAL);
    tokenPatterns.emplace_back("!=", TokenType::T_NOT_EQUAL);
    tokenPatterns.emplace_back("<=", TokenType::T_LESS_EQUAL);
    tokenPatterns.emplace_back(">=", TokenType::T_GREATER_EQUAL);
    tokenPatterns.emplace_back("&&", TokenType::T_LOGICAL_AND);
    tokenPatterns.emplace_back("\\|\\|", TokenType::T_LOGICAL_OR);
    tokenPatterns.emplace_back("->", TokenType::T_ARROW);
    tokenPatterns.emplace_back("::", TokenType::T_SCOPE);
    tokenPatterns.emplace_back("<<", TokenType::T_LEFT_SHIFT);
    tokenPatterns.emplace_back(">>", TokenType::T_RIGHT_SHIFT);
    tokenPatterns.emplace_back("\\+=", TokenType::T_PLUS_ASSIGN);
    tokenPatterns.emplace_back("-=", TokenType::T_MINUS_ASSIGN);
    tokenPatterns.emplace_back("\\*=", TokenType::T_MULTIPLY_ASSIGN);
    tokenPatterns.emplace_back("/=", TokenType::T_DIVIDE_ASSIGN);
    tokenPatterns.emplace_back("%=", TokenType::T_MOD_ASSIGN);
    tokenPatterns.emplace_back("\\?\\?", TokenType::T_TERNARY); 
    
    // Single character operators
    tokenPatterns.emplace_back("\\+", TokenType::T_PLUS);
    tokenPatterns.emplace_back("-", TokenType::T_MINUS);
    tokenPatterns.emplace_back("\\*", TokenType::T_MULTIPLY);
    tokenPatterns.emplace_back("/", TokenType::T_DIVIDE);
    tokenPatterns.emplace_back("%", TokenType::T_MOD);
    tokenPatterns.emplace_back("=", TokenType::T_ASSIGN);
    tokenPatterns.emplace_back("<", TokenType::T_LESS);
    tokenPatterns.emplace_back(">", TokenType::T_GREATER);
    tokenPatterns.emplace_back("!", TokenType::T_LOGICAL_NOT);
    tokenPatterns.emplace_back("&", TokenType::T_BITWISE_AND);
    tokenPatterns.emplace_back("\\|", TokenType::T_BITWISE_OR);
    tokenPatterns.emplace_back("\\^", TokenType::T_BITWISE_XOR);
    tokenPatterns.emplace_back("~", TokenType::T_BITWISE_NOT);
    
    // Punctuators
    tokenPatterns.emplace_back("\\{", TokenType::T_LBRACE);
    tokenPatterns.emplace_back("\\}", TokenType::T_RBRACE);
    tokenPatterns.emplace_back("\\(", TokenType::T_LPAREN);
    tokenPatterns.emplace_back("\\)", TokenType::T_RPAREN);
    tokenPatterns.emplace_back("\\[", TokenType::T_LBRACKET);
    tokenPatterns.emplace_back("\\]", TokenType::T_RBRACKET);
    tokenPatterns.emplace_back(";", TokenType::T_SEMICOLON);
    tokenPatterns.emplace_back(",", TokenType::T_COMMA);
    tokenPatterns.emplace_back(":", TokenType::T_COLON);
    tokenPatterns.emplace_back("\\.", TokenType::T_DOT);
    tokenPatterns.emplace_back("\\?", TokenType::T_QUESTION);
    
    // Literals
    tokenPatterns.emplace_back("0[xX][0-9a-fA-F]+", TokenType::T_INTLIT); // Hex
    tokenPatterns.emplace_back("0[0-7]*", TokenType::T_INTLIT); // Octal
    tokenPatterns.emplace_back("[1-9][0-9]*", TokenType::T_INTLIT); // Decimal
    tokenPatterns.emplace_back("[0-9]*\\.[0-9]+([eE][+-]?[0-9]+)?", TokenType::T_FLOATLIT);
    tokenPatterns.emplace_back("[0-9]+[eE][+-]?[0-9]+", TokenType::T_FLOATLIT);
    
    // Identifiers
    tokenPatterns.emplace_back("[a-zA-Z_][a-zA-Z0-9_]*", TokenType::T_IDENTIFIER);
    
    // Preprocessor directives bhi kar hi li hain
    tokenPatterns.emplace_back("#include", TokenType::T_INCLUDE);
    tokenPatterns.emplace_back("#define", TokenType::T_DEFINE);
    tokenPatterns.emplace_back("#undef", TokenType::T_UNDEF);
    tokenPatterns.emplace_back("#ifdef", TokenType::T_IFDEF);
    tokenPatterns.emplace_back("#ifndef", TokenType::T_IFNDEF);
    tokenPatterns.emplace_back("#elif", TokenType::T_ELIF);
    tokenPatterns.emplace_back("#endif", TokenType::T_ENDIF);
    tokenPatterns.emplace_back("#pragma", TokenType::T_PRAGMA);
    tokenPatterns.emplace_back("#error", TokenType::T_ERROR);
    tokenPatterns.emplace_back("#line", TokenType::T_LINE);
}

void Lexer::skipWhitespaceAndComment() {
    while (pos < input.length()) {
        if (input[pos] == '\n') {
            line++;
            column = 1;
            pos++;
            continue;
        }
        if (isspace(input[pos])) {
            pos++;
            column++;
            continue;
        }
        if (pos + 1 < input.length() && input[pos] == '/' && input[pos + 1] == '/') {    //  yahn pr Single line comments
            pos += 2;
            column += 2;
            while (pos < input.length() && input[pos] != '\n') {
                pos++;
                column++;
            }
            continue;
        }
        if (pos + 1 < input.length() && input[pos] == '/' && input[pos + 1] == '*') {  // idhr Multi-line comments
            int startLine = line;
            int startColumn = column;
            pos += 2;
            column += 2;
            
            while (pos + 1 < input.length() && !(input[pos] == '*' && input[pos + 1] == '/')) {
                if (input[pos] == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                pos++;
            }
            
            if (pos + 1 >= input.length()) {
                throw runtime_error("Unterminated comment at line " + to_string(startLine) + 
                                  ", column " + to_string(startColumn));
            }
            pos += 2; 
            column += 2;
            continue;
        }
        break;
    }
}
Token Lexer::processString() {
    size_t startColumn = column;
    pos++; 
    column++;
    string content;
    
    bool isRawString = false;
    bool isWideString = false;
    if (pos > 1 && input[pos-2] == 'L') {
        isWideString = true;
    }
    if (pos > 1 && input[pos-2] == 'R') {
        isRawString = true;
    }
    
    while (pos < input.length() && input[pos] != '"') {
        if (!isRawString && input[pos] == '\\') {
            pos++;
            column++;
            if (pos >= input.length()) {
                throw runtime_error("Unterminated string literal at line " + 
                                  to_string(line) + ", column " + to_string(startColumn));
            }
            switch (input[pos]) {
                case 'n': content += '\n'; break;
                case 't': content += '\t'; break;
                case 'r': content += '\r'; break;
                case '\\': content += '\\'; break;
                case '"': content += '"'; break;
                case '0': content += '\0'; break;
                case 'a': content += '\a'; break;
                case 'b': content += '\b'; break;
                case 'f': content += '\f'; break;
                case 'v': content += '\v'; break;
                case 'x': // yahn pr hex escape sequences ka case ha
                    if (pos + 2 < input.length() && isxdigit(input[pos+1]) && isxdigit(input[pos+2])) {
                        content += "\\x";
                        content += input[pos+1];
                        content += input[pos+2];
                        pos += 2;
                        column += 2;
                    }
                    break;
                default:
                    content += '\\';
                    content += input[pos];
                    break;
            }
            pos++;
            column++;
        } else {
            content += input[pos];
            pos++;
            column++;
        }
    }
    if (pos >= input.length()) {
        throw runtime_error("Unterminated string literal at line " + 
                          to_string(line) + ", column " + to_string(startColumn));
    }
    pos++;
    column++;
    return Token(TokenType::T_STRINGLIT, content, line, startColumn);
}
Token Lexer::processChar() {
    size_t startColumn = column;
    pos++;
    column++;
    string content;
    
    if (input[pos] == '\\') {
        pos++;
        column++;
        if (pos >= input.length()) {
            throw runtime_error("Unterminated character literal at line " + 
                              to_string(line) + ", column " + to_string(startColumn));
        }
        switch (input[pos]) {
            case 'n': content = "\\n"; break;
            case 't': content = "\\t"; break;
            case 'r': content = "\\r"; break;
            case '\\': content = "\\\\"; break;
            case '\'': content = "\\'"; break;
            case '0': content = "\\0"; break;
            default: content = string(1, input[pos]); break;
        }
        pos++;
        column++;
    } else {
        content = string(1, input[pos]);
        pos++;
        column++;
    }
    
    if (pos >= input.length() || input[pos] != '\'') {
        throw runtime_error("Unterminated character literal at line " + 
                          to_string(line) + ", column " + to_string(startColumn));
    }
    
    pos++; 
    column++;
    return Token(TokenType::T_CHARLIT, content, line, startColumn);
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    
    while (pos < input.length()) {
        skipWhitespaceAndComment();
        if (pos >= input.length()) break;
        int currentLine = line;
        int currentColumn = column;
        if (input[pos] == '"') {   // Handle string literals ki checking
            tokens.push_back(processString());
            continue;
        }
        if (input[pos] == '\'') {     // Handle character literals  ki checking
            tokens.push_back(processChar());
            continue;
        }
        bool matched = false;
        for (const auto& pattern : tokenPatterns) {
            smatch match;
            string substr = input.substr(pos);
            if (regex_search(substr, match, pattern.pattern, regex_constants::match_continuous)) {
                if (!match.empty() && match.position() == 0) {
                    string matchedStr = match.str();   
                    if (pattern.isKeyword) {
                        size_t nextPos = pos + matchedStr.length();
                        if (nextPos < input.length() && (isalnum(input[nextPos]) || input[nextPos] == '_')) {
                            continue; 
                        }
                    }
                    // Validate identifiers
                    if (pattern.type == TokenType::T_IDENTIFIER) {
                        if (!isalpha(matchedStr[0]) && matchedStr[0] != '_') {
                            throw runtime_error("Invalid identifier '" + matchedStr + 
                                               "' at line " + to_string(currentLine) + 
                                               ", column " + to_string(currentColumn));
                        }
                    }
                    
                    tokens.push_back(Token(pattern.type, matchedStr, currentLine, currentColumn));
                    pos += matchedStr.length();
                    column += matchedStr.length();
                    matched = true;
                    break;
                }
            }
        }
        
        if (!matched) {
            throw runtime_error("Unknown token at line " + to_string(currentLine) + 
                               ", column " + to_string(currentColumn) + 
                               ": '" + string(1, input[pos]) + "'");
        }
    }
    tokens.push_back(Token(TokenType::T_EOF, "", line, column));
    return tokens;
}
