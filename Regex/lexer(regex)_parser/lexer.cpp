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
            column = 1;
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
    // Standard C++ keywords
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

    // Custom language keywords - CORRECTED MAPPINGS
    tokenPatterns.emplace_back("\\bfn\\b", TokenType::T_FN, true);
    tokenPatterns.emplace_back("\\bstring\\b", TokenType::T_STRING, true);
    tokenPatterns.emplace_back("\\bwapsi\\b", TokenType::T_RETURN, true);
    tokenPatterns.emplace_back("\\bduhrao\\b", TokenType::T_FOR, true);
    tokenPatterns.emplace_back("\\bagar\\b", TokenType::T_IF, true);
    tokenPatterns.emplace_back("\\btoro\\b", TokenType::T_BREAK, true);
    tokenPatterns.emplace_back("\\bwarna\\b", TokenType::T_ELSE, true);
    tokenPatterns.emplace_back("\\bginti\\b", TokenType::T_INT, true);

    // Operators - Multi-character first
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
    tokenPatterns.emplace_back("0[xX][0-9a-fA-F]+", TokenType::T_INTLIT);
    tokenPatterns.emplace_back("0[0-7]+", TokenType::T_INTLIT);
    tokenPatterns.emplace_back("[0-9]+\\.[0-9]+([eE][+-]?[0-9]+)?", TokenType::T_FLOATLIT);
    tokenPatterns.emplace_back("[0-9]+[eE][+-]?[0-9]+", TokenType::T_FLOATLIT);
    tokenPatterns.emplace_back("[0-9]+", TokenType::T_INTLIT);

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
            if (peek() == '/') {
                while (currentChar != '\0' && currentChar != '\n') {
                    advance();
                }
                continue;
            }
            if (peek() == '*') {
                advance();
                advance();
                while (currentChar != '\0') {
                    if (currentChar == '*' && peek() == '/') break;
                    advance();
                }
                if (currentChar == '\0') {
                    tokens.push_back(Token(TokenType::T_ERROR, "Unterminated block comment", startLine, startColumn));
                } else {
                    advance();
                    advance();
                }
                continue;
            }
        }

        // String Literals
        if (currentChar == '"') {
            string value;
            advance();
            while (currentChar != '\0' && currentChar != '"' && currentChar != '\n') {
                if (currentChar == '\\') {
                    advance();
                    if (currentChar == '\0' || currentChar == '\n') break;
                    switch (currentChar) {
                        case 'n': value += '\n'; break;
                        case 't': value += '\t'; break;
                        case 'r': value += '\r'; break;
                        case '"': value += '"'; break;
                        case '\\': value += '\\'; break;
                        default: value += currentChar; break;
                    }
                } else {
                    value += currentChar;
                }
                advance();
            }
            if (currentChar == '"') {
                tokens.push_back(Token(TokenType::T_STRINGLIT, value, startLine, startColumn));
                advance();
            } else {
                tokens.push_back(Token(TokenType::T_ERROR, "Unterminated string literal", startLine, startColumn));
            }
            continue;
        }
        
        // Character Literals
        if (currentChar == '\'') {
            string value;
            advance();
            if (currentChar == '\\') {
                advance();
                if(currentChar != '\0' && currentChar != '\n') {
                    switch (currentChar) {
                        case 'n': value = "\n"; break;
                        case 't': value = "\t"; break;
                        case 'r': value = "\r"; break;
                        case '\\': value = "\\"; break;
                        case '\'': value = "'"; break;
                        default: value = string(1, currentChar); break;
                    }
                    advance();
                }
            } else if (currentChar != '\0' && currentChar != '\'' && currentChar != '\n') {
                value = string(1, currentChar);
                advance();
            }
            if (currentChar == '\'') {
                tokens.push_back(Token(TokenType::T_CHARLIT, value, startLine, startColumn));
                advance();
            } else {
                tokens.push_back(Token(TokenType::T_ERROR, "Unterminated character literal", startLine, startColumn));
            }
            continue;
        }

        // Match against patterns
        string remaining = input.substr(pos);
        bool matched = false;
        
        for (const auto& pattern : tokenPatterns) {
            smatch match;
            if (regex_search(remaining, match, pattern.pattern, regex_constants::match_continuous)) {
                string matchedStr = match.str(0);

                // Handle keyword ambiguity
                if (pattern.isKeyword) {
                    size_t nextPos = pos + matchedStr.length();
                    if (nextPos < input.length() && (isalnum(input[nextPos]) || input[nextPos] == '_')) {
                        continue;
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
            // Check for invalid identifiers starting with numbers
            regex invalid_ident_regex("^([0-9]+[a-zA-Z_][a-zA-Z0-9_]*)");
            smatch invalid_ident_match;
            if (regex_search(remaining, invalid_ident_match, invalid_ident_regex)) {
                string matchedStr = invalid_ident_match.str(1);
                tokens.push_back(Token(TokenType::T_ERROR, "Invalid identifier: " + matchedStr, startLine, startColumn));
                for(size_t i = 0; i < matchedStr.length(); ++i) advance();
            } else {
                tokens.push_back(Token(TokenType::T_ERROR, "Unknown character: '" + string(1, currentChar) + "'", line, column));
                advance();
            }
        }
    }
    tokens.push_back(Token(TokenType::T_EOF, "", line, column));
    return tokens;
}