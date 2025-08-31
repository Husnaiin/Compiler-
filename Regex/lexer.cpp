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
    // Keywords (C/C++98/03) - isKeyword flag set to true
    tokenPatterns.emplace_back("\\bauto\\b", TokenType::T_AUTO, true);
    tokenPatterns.emplace_back("\\bbreak\\b", TokenType::T_BREAK, true);
    // ... (all other existing keywords from your original file) ...
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
    tokenPatterns.emplace_back("\\btrue\\b", TokenType::T_TRUE, true);
    tokenPatterns.emplace_back("\\bfalse\\b", TokenType::T_FALSE, true);
    tokenPatterns.emplace_back("\\btypeid\\b", TokenType::T_TYPEID, true);
    tokenPatterns.emplace_back("\\bdynamic_cast\\b", TokenType::T_DYNAMIC_CAST, true);
    tokenPatterns.emplace_back("\\bstatic_cast\\b", TokenType::T_STATIC_CAST, true);
    tokenPatterns.emplace_back("\\breinterpret_cast\\b", TokenType::T_REINTERPRET_CAST, true);
    tokenPatterns.emplace_back("\\bconst_cast\\b", TokenType::T_CONST_CAST, true);
    tokenPatterns.emplace_back("\\bexplicit\\b", TokenType::T_EXPLICIT, true);
    tokenPatterns.emplace_back("\\bmutable\\b", TokenType::T_MUTABLE, true);
    
    // Modern C++ Keywords (C++11 and newer) - isKeyword flag set to true
    tokenPatterns.emplace_back("\\bnullptr\\b", TokenType::T_NULLPTR, true);
    tokenPatterns.emplace_back("\\bconstexpr\\b", TokenType::T_CONSTEXPR, true);
    tokenPatterns.emplace_back("\\bnoexcept\\b", TokenType::T_NOEXCEPT, true);
    tokenPatterns.emplace_back("\\bdecltype\\b", TokenType::T_DECLTYPE, true);
    tokenPatterns.emplace_back("\\bstatic_assert\\b", TokenType::T_STATIC_ASSERT, true);
    tokenPatterns.emplace_back("\\bchar16_t\\b", TokenType::T_CHAR16_T, true);
    tokenPatterns.emplace_back("\\bchar32_t\\b", TokenType::T_CHAR32_T, true);
    tokenPatterns.emplace_back("\\balignas\\b", TokenType::T_ALIGNAS, true);
    tokenPatterns.emplace_back("\\balignof\\b", TokenType::T_ALIGNOF, true);
    tokenPatterns.emplace_back("\\bco_await\\b", TokenType::T_CO_AWAIT, true);
    tokenPatterns.emplace_back("\\bco_yield\\b", TokenType::T_CO_YIELD, true);
    tokenPatterns.emplace_back("\\bco_return\\b", TokenType::T_CO_RETURN, true);
    tokenPatterns.emplace_back("\\bconcept\\b", TokenType::T_CONCEPT, true);
    tokenPatterns.emplace_back("\\brequires\\b", TokenType::T_REQUIRES, true);

    // Operators - Multi-character first to ensure longest match
    tokenPatterns.emplace_back("<=>", TokenType::T_SPACESHIP);
    tokenPatterns.emplace_back("->\\*", TokenType::T_ARROW_ASTERISK);
    tokenPatterns.emplace_back("\\.\\.\\.", TokenType::T_ELLIPSIS);
    tokenPatterns.emplace_back("<<=", TokenType::T_LEFT_SHIFT_ASSIGN);
    tokenPatterns.emplace_back(">>=", TokenType::T_RIGHT_SHIFT_ASSIGN);
    tokenPatterns.emplace_back("\\+=" , TokenType::T_PLUS_ASSIGN);
    tokenPatterns.emplace_back("-=", TokenType::T_MINUS_ASSIGN);
    tokenPatterns.emplace_back("\\*=", TokenType::T_MULTIPLY_ASSIGN);
    tokenPatterns.emplace_back("/=", TokenType::T_DIVIDE_ASSIGN);
    tokenPatterns.emplace_back("%=", TokenType::T_MOD_ASSIGN);
    tokenPatterns.emplace_back("&=", TokenType::T_BITWISE_AND_ASSIGN);
    tokenPatterns.emplace_back("\\|=", TokenType::T_BITWISE_OR_ASSIGN);
    tokenPatterns.emplace_back("\\^=", TokenType::T_BITWISE_XOR_ASSIGN);
    tokenPatterns.emplace_back("<<", TokenType::T_LEFT_SHIFT);
    tokenPatterns.emplace_back(">>", TokenType::T_RIGHT_SHIFT);
    tokenPatterns.emplace_back("\\+\\+", TokenType::T_INCREMENT);
    tokenPatterns.emplace_back("--", TokenType::T_DECREMENT);
    tokenPatterns.emplace_back("->", TokenType::T_ARROW);
    tokenPatterns.emplace_back("==", TokenType::T_EQUAL);
    tokenPatterns.emplace_back("!=", TokenType::T_NOT_EQUAL);
    tokenPatterns.emplace_back("<=", TokenType::T_LESS_EQUAL);
    tokenPatterns.emplace_back(">=", TokenType::T_GREATER_EQUAL);
    tokenPatterns.emplace_back("&&", TokenType::T_LOGICAL_AND);
    tokenPatterns.emplace_back("\\|\\|", TokenType::T_LOGICAL_OR);
    tokenPatterns.emplace_back("::", TokenType::T_SCOPE);
    tokenPatterns.emplace_back("\\.\\*", TokenType::T_DOT_ASTERISK);
    
    // Single-character Operators and Punctuators
    tokenPatterns.emplace_back("\\+", TokenType::T_PLUS);
    tokenPatterns.emplace_back("-", TokenType::T_MINUS);
    tokenPatterns.emplace_back("\\*", TokenType::T_ASTERISK); // Refactored
    tokenPatterns.emplace_back("/", TokenType::T_DIVIDE);
    tokenPatterns.emplace_back("%", TokenType::T_MOD);
    tokenPatterns.emplace_back("=", TokenType::T_ASSIGN);
    tokenPatterns.emplace_back("<", TokenType::T_LESS);
    tokenPatterns.emplace_back(">", TokenType::T_GREATER);
    tokenPatterns.emplace_back("!", TokenType::T_LOGICAL_NOT);
    tokenPatterns.emplace_back("&", TokenType::T_AMPERSAND); // Refactored
    tokenPatterns.emplace_back("\\|", TokenType::T_BITWISE_OR);
    tokenPatterns.emplace_back("\\^", TokenType::T_BITWISE_XOR);
    tokenPatterns.emplace_back("~", TokenType::T_BITWISE_NOT);
    tokenPatterns.emplace_back("\\?", TokenType::T_QUESTION);
    tokenPatterns.emplace_back(";", TokenType::T_SEMICOLON);
    tokenPatterns.emplace_back(",", TokenType::T_COMMA);
    tokenPatterns.emplace_back(":", TokenType::T_COLON);
    tokenPatterns.emplace_back("\\.", TokenType::T_DOT);
    tokenPatterns.emplace_back("\\{", TokenType::T_LBRACE);
    tokenPatterns.emplace_back("\\}", TokenType::T_RBRACE);
    tokenPatterns.emplace_back("\\(", TokenType::T_LPAREN);
    tokenPatterns.emplace_back("\\)", TokenType::T_RPAREN);
    tokenPatterns.emplace_back("\\[", TokenType::T_LBRACKET);
    tokenPatterns.emplace_back("\\]", TokenType::T_RBRACKET);
    tokenPatterns.emplace_back("#", TokenType::T_HASH);
    
    // Literals (Improved Regex)
    // Note: String and Char literals are handled by special functions, not regex.
    tokenPatterns.emplace_back("(0[xX][0-9a-fA-F]+|0[bB][01]+|[0-9]+)[uUlL]{0,3}", TokenType::T_INTLIT);
    tokenPatterns.emplace_back("([0-9]+\\.[0-9]*|[0-9]*\\.[0-9]+)([eE][+-]?[0-9]+)?[fFlL]?", TokenType::T_FLOATLIT);
    tokenPatterns.emplace_back("[0-9]+[eE][+-]?[0-9]+[fFlL]?", TokenType::T_FLOATLIT);

    // Identifiers (must be checked after keywords)
    tokenPatterns.emplace_back("[a-zA-Z_][a-zA-Z0-9_]*", TokenType::T_IDENTIFIER);
    
    // Preprocessor directives
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
