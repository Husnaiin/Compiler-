#include "lexer.h"
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <map>
using namespace std;

Lexer::Lexer(const string& src) : input(src), pos(0), line(1), column(1) {
    currentChar = input.empty() ? '\0' : input[0];
    initTokenPatterns();
}

void Lexer::advance() {
    if (pos < input.length()) {
        if (currentChar == '\n') {
            line++;
            column = 1; // Reset column to 1 on new line
        } else {
            column++;
        }
        pos++;
        currentChar = (pos < input.length()) ? input[pos] : '\0';
    }
}

char Lexer::peek() {
    return (pos + 1 < input.length()) ? input[pos + 1] : '\0';
}

void Lexer::initTokenPatterns() {
    // Keywords (C/C++98/03) - isKeyword flag set to true
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
    tokenPatterns.emplace_back("\\bstruct\\b", TokenType::T_STATIC, true);
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
    
    // Additional keywords for test cases
    tokenPatterns.emplace_back("\\bfn\\b", TokenType::T_FN, true);
    tokenPatterns.emplace_back("\\bstring\\b", TokenType::T_STRING, true);
    tokenPatterns.emplace_back("\\bwapsi\\b", TokenType::T_RETURN, true);
    tokenPatterns.emplace_back("\\bduhrao\\b", TokenType::T_FOR, true);
    tokenPatterns.emplace_back("\\bagar\\b", TokenType::T_IF, true);
    tokenPatterns.emplace_back("\\btoro\\b", TokenType::T_BREAK, true);
    tokenPatterns.emplace_back("\\bwarna\\b", TokenType::T_ELSE, true);
    tokenPatterns.emplace_back("\\bginti\\b", TokenType::T_INT, true);
    
    // Operators - Multi-character first for correct matching
    tokenPatterns.emplace_back("<<=", TokenType::T_LEFT_SHIFT_ASSIGN);
    tokenPatterns.emplace_back(">>=", TokenType::T_RIGHT_SHIFT_ASSIGN);
    tokenPatterns.emplace_back("\\+=", TokenType::T_PLUS_ASSIGN);
    tokenPatterns.emplace_back("-=", TokenType::T_MINUS_ASSIGN);
    tokenPatterns.emplace_back("\\*=", TokenType::T_MULTIPLY_ASSIGN);
    tokenPatterns.emplace_back("/=", TokenType::T_DIVIDE_ASSIGN);
    tokenPatterns.emplace_back("%=", TokenType::T_MOD_ASSIGN);
    tokenPatterns.emplace_back("&=", TokenType::T_BITWISE_AND_ASSIGN);
    tokenPatterns.emplace_back("\\|=", TokenType::T_BITWISE_OR_ASSIGN);
    tokenPatterns.emplace_back("\\^=", TokenType::T_BITWISE_XOR_ASSIGN);
    tokenPatterns.emplace_back("->", TokenType::T_ARROW);
    tokenPatterns.emplace_back("\\+\\+", TokenType::T_INCREMENT);
    tokenPatterns.emplace_back("--", TokenType::T_DECREMENT);
    tokenPatterns.emplace_back("<<", TokenType::T_LEFT_SHIFT);
    tokenPatterns.emplace_back(">>", TokenType::T_RIGHT_SHIFT);
    tokenPatterns.emplace_back("&&", TokenType::T_LOGICAL_AND);
    tokenPatterns.emplace_back("\\|\\|", TokenType::T_LOGICAL_OR);
    tokenPatterns.emplace_back("==", TokenType::T_EQUAL);
    tokenPatterns.emplace_back("!=", TokenType::T_NOT_EQUAL);
    tokenPatterns.emplace_back("<=", TokenType::T_LESS_EQUAL);
    tokenPatterns.emplace_back(">=", TokenType::T_GREATER_EQUAL);
    tokenPatterns.emplace_back("::", TokenType::T_SCOPE);
    
    // Single character operators
    tokenPatterns.emplace_back("\\+", TokenType::T_PLUS);
    tokenPatterns.emplace_back("-", TokenType::T_MINUS);
    tokenPatterns.emplace_back("\\*", TokenType::T_ASTERISK);
    tokenPatterns.emplace_back("/", TokenType::T_DIVIDE);
    tokenPatterns.emplace_back("%", TokenType::T_MOD);
    tokenPatterns.emplace_back("=", TokenType::T_ASSIGN);
    tokenPatterns.emplace_back("<", TokenType::T_LESS);
    tokenPatterns.emplace_back(">", TokenType::T_GREATER);
    tokenPatterns.emplace_back("!", TokenType::T_LOGICAL_NOT);
    tokenPatterns.emplace_back("&", TokenType::T_AMPERSAND);
    tokenPatterns.emplace_back("\\|", TokenType::T_BITWISE_OR);
    tokenPatterns.emplace_back("\\^", TokenType::T_BITWISE_XOR);
    tokenPatterns.emplace_back("~", TokenType::T_BITWISE_NOT);
    
    // Punctuators
    tokenPatterns.emplace_back(";", TokenType::T_SEMICOLON);
    tokenPatterns.emplace_back(",", TokenType::T_COMMA);
    tokenPatterns.emplace_back("\\.", TokenType::T_DOT);
    tokenPatterns.emplace_back(":", TokenType::T_COLON);
    tokenPatterns.emplace_back("\\?", TokenType::T_QUESTION);
    tokenPatterns.emplace_back("\\(", TokenType::T_LPAREN);
    tokenPatterns.emplace_back("\\)", TokenType::T_RPAREN);
    tokenPatterns.emplace_back("\\[", TokenType::T_LBRACKET);
    tokenPatterns.emplace_back("\\]", TokenType::T_RBRACKET);
    tokenPatterns.emplace_back("\\{", TokenType::T_LBRACE);
    tokenPatterns.emplace_back("\\}", TokenType::T_RBRACE);
    
    // Literals
    tokenPatterns.emplace_back("0[xX][0-9a-fA-F]+", TokenType::T_INTLIT); // Hex
    tokenPatterns.emplace_back("0[0-7]+", TokenType::T_INTLIT);          // Octal
    tokenPatterns.emplace_back("[0-9]+\\.[0-9]+([eE][+-]?[0-9]+)?", TokenType::T_FLOATLIT);
    tokenPatterns.emplace_back("[0-9]+[eE][+-]?[0-9]+", TokenType::T_FLOATLIT);
    tokenPatterns.emplace_back("[0-9]+", TokenType::T_INTLIT);           // Decimal

    // Identifiers
    tokenPatterns.emplace_back("[a-zA-Z_][a-zA-Z0-9_]*", TokenType::T_IDENTIFIER);
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    while (currentChar != '\0') {
        const int startLine = line;
        const int startColumn = column;

        // Skip whitespace
        if (isspace(currentChar)) {
            advance();
            continue;
        }

        // Comments
        if (currentChar == '/') {
            if (peek() == '/') { // Single-line comment
                while (currentChar != '\0' && currentChar != '\n') {
                    advance();
                }
                continue;
            }
            if (peek() == '*') { // Block comment
                advance(); // Consume '/'
                advance(); // Consume '*'

                while (currentChar != '\0') {
                    if (currentChar == '*' && peek() == '/') {
                        break;
                    }
                    advance();
                }
                
                // If loop finished because of EOF, it's an unterminated comment
                if (currentChar == '\0') {
                    tokens.push_back(Token(TokenType::T_ERROR, "Unterminated block comment", startLine, startColumn));
                } else {
                    advance(); // Consume '*'
                    advance(); // Consume '/'
                }
                continue;
            }
        }

        // String Literals
        if (currentChar == '"') {
            string value;
            advance(); // Consume opening '"'
            while (currentChar != '\0' && currentChar != '"' && currentChar != '\n') {
                if (currentChar == '\\') {
                    advance(); // Consume '\'
                    if (currentChar == '\0' || currentChar == '\n') break; // Unterminated escape
                    
                    switch (currentChar) {
                        case 'n': value += "\\n"; break;
                        case 't': value += "\\t"; break;
                        case 'r': value += "\\r"; break;
                        case '"': value += "\\\""; break;
                        case '\'': value += "\\'"; break;
                        case '\\': value += "\\\\"; break;
                        case '0': value += "\\0"; break;
                        case 'a': value += "\\a"; break;
                        case 'b': value += "\\b"; break;
                        case 'f': value += "\\f"; break;
                        case 'v': value += "\\v"; break;
                        case 'x': // Hex escape sequences
                            if (peek() != '\0' && isxdigit(peek())) {
                                value += "\\x";
                                advance();
                                value += currentChar;
                                if (peek() != '\0' && isxdigit(peek())) {
                                    advance();
                                    value += currentChar;
                                }
                            } else {
                                value += "\\x";
                            }
                            break;
                        default:
                            value += '\\';
                            value += currentChar;
                            break;
                    }
                } else {
                    value += currentChar;
                }
                advance();
            }

            if (currentChar == '"') {
                tokens.push_back(Token(TokenType::T_STRINGLIT, value, startLine, startColumn));
                advance(); // Consume closing '"'
            } else {
                tokens.push_back(Token(TokenType::T_ERROR, "Unterminated string literal", startLine, startColumn));
            }
            continue;
        }
        
        // Character Literals
        if (currentChar == '\'') {
            string value;
            advance(); // Consume opening '''
            
            if (currentChar == '\\') { // Escape sequence
                advance(); // Consume '\'
                if(currentChar != '\0' && currentChar != '\n') {
                    switch (currentChar) {
                        case 'n': value = "\\n"; break;
                        case 't': value = "\\t"; break;
                        case 'r': value = "\\r"; break;
                        case '\\': value = "\\\\"; break;
                        case '\'': value = "\\'"; break;
                        case '0': value = "\\0"; break;
                        case 'a': value = "\\a"; break;
                        case 'b': value = "\\b"; break;
                        case 'f': value = "\\f"; break;
                        case 'v': value = "\\v"; break;
                        case 'x': // Hex escape sequences
                            if (peek() != '\0' && isxdigit(peek())) {
                                value = "\\x";
                                advance();
                                value += currentChar;
                                if (peek() != '\0' && isxdigit(peek())) {
                                    advance();
                                    value += currentChar;
                                }
                            } else {
                                value = "\\x";
                            }
                            break;
                        default:
                            value = "\\";
                            value += currentChar;
                            break;
                    }
                    advance();
                }
            } else if (currentChar != '\0' && currentChar != '\'' && currentChar != '\n') {
                value = string(1, currentChar);
                advance();
            }

            if (currentChar == '\'') {
                tokens.push_back(Token(TokenType::T_CHARLIT, value, startLine, startColumn));
                advance(); // Consume closing '''
            } else {
                tokens.push_back(Token(TokenType::T_ERROR, "Unterminated character literal", startLine, startColumn));
            }
            continue;
        }

        // Special check for invalid identifiers like "123abc"
        string remaining = input.substr(pos);
        smatch invalid_ident_match;
        regex invalid_ident_regex("^([0-9]+[a-zA-Z_][a-zA-Z0-9_]*)");
        if (regex_search(remaining, invalid_ident_match, invalid_ident_regex)) {
            string matchedStr = invalid_ident_match.str(1);
            tokens.push_back(Token(TokenType::T_ERROR, "Invalid identifier: " + matchedStr, startLine, startColumn));
            for(size_t i = 0; i < matchedStr.length(); ++i) advance();
            continue;
        }

        // Match against other patterns (operators, keywords, valid identifiers, etc.)
        bool matched = false;
        for (const auto& pattern : tokenPatterns) {
            smatch match;
            if (regex_search(remaining, match, pattern.pattern, regex_constants::match_continuous)) {
                string matchedStr = match.str(0);

                // Handle keyword ambiguity (e.g., "int" vs "integer")
                if (pattern.isKeyword) {
                    size_t nextPos = pos + matchedStr.length();
                    if (nextPos < input.length() && (isalnum(input[nextPos]) || input[nextPos] == '_')) {
                        continue; // It's a prefix of a longer identifier, so skip this keyword match
                    }
                }

                tokens.push_back(Token(pattern.type, matchedStr, startLine, startColumn));
                for (size_t i = 0; i < matchedStr.length(); i++) {
                    advance();
                }
                matched = true;
                break;
            }
        }

        if (!matched) {
            tokens.push_back(Token(TokenType::T_ERROR, "Unknown character: '" + string(1, currentChar) + "'", line, column));
            advance();
        }
    }
    tokens.push_back(Token(TokenType::T_EOF, "", line, column));
    return tokens;
}