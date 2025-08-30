#include "lexer.h"
#include <cctype>
#include <stdexcept>
#include <regex>
#include <sstream>
using namespace std;

Lexer::Lexer(const string& src) : input(src), pos(0), line(1), column(1) {
    if (!input.empty()) {
        currentChar = input[0];
    }
    initTokenPatterns();
}

struct Lexer::TokenPattern {
    regex pattern;
    TokenType type;
    bool isKeyword;
    TokenPattern(const string& regexStr, TokenType t, bool keyword = false) 
        : pattern(regexStr, regex_constants::ECMAScript), type(t), isKeyword(keyword) {}
};

void Lexer::initTokenPatterns() {
    // Keywords
    tokenPatterns.emplace_back("\\bfn\\b", TokenType::T_Function, true);
    tokenPatterns.emplace_back("\\bint\\b", TokenType::T_INT, true);
    tokenPatterns.emplace_back("\\bfloat\\b", TokenType::T_FLOAT, true);
    tokenPatterns.emplace_back("\\bstring\\b", TokenType::T_STRING, true);
    tokenPatterns.emplace_back("\\bbool\\b", TokenType::T_BOOL, true);
    tokenPatterns.emplace_back("\\bchar\\b", TokenType::T_CHAR, true);
    tokenPatterns.emplace_back("\\bif\\b", TokenType::T_IF, true);
    tokenPatterns.emplace_back("\\belse\\b", TokenType::T_ELSE, true);
    tokenPatterns.emplace_back("\\bwhile\\b", TokenType::T_WHILE, true);
    tokenPatterns.emplace_back("\\bfor\\b", TokenType::T_FOR, true);
    tokenPatterns.emplace_back("\\breturn\\b", TokenType::T_RETURN, true);

    // Operators
    tokenPatterns.emplace_back("==", TokenType::T_EQUALSOP);
    tokenPatterns.emplace_back("!=", TokenType::T_NOTEQUALS);
    tokenPatterns.emplace_back("<=", TokenType::T_LESSEQUAL);
    tokenPatterns.emplace_back(">=", TokenType::T_GREATEREQUAL);
    tokenPatterns.emplace_back("&&", TokenType::T_AND);
    tokenPatterns.emplace_back("\\|\\|", TokenType::T_OR);
    tokenPatterns.emplace_back("=", TokenType::T_ASSSSIGNOP);
    tokenPatterns.emplace_back("\\+", TokenType::T_PLUS);
    tokenPatterns.emplace_back("-", TokenType::T_MINUS);
    tokenPatterns.emplace_back("\\*", TokenType::T_MULTIPLY);
    tokenPatterns.emplace_back("/", TokenType::T_DIVIDE);
    tokenPatterns.emplace_back("%", TokenType::T_MOD);

    // Punctuators
    tokenPatterns.emplace_back("\\(", TokenType::T_PARENL);
    tokenPatterns.emplace_back("\\)", TokenType::T_PARENR);
    tokenPatterns.emplace_back("\\{", TokenType::T_BRACEL);
    tokenPatterns.emplace_back("\\}", TokenType::T_BRACER);
    tokenPatterns.emplace_back(";", TokenType::T_SEMICOLON);
    tokenPatterns.emplace_back(",", TokenType::T_COMMA);
    tokenPatterns.emplace_back("\"", TokenType::T_QOUTES);
    tokenPatterns.emplace_back("\\[", TokenType::T_BRACKETL);
    tokenPatterns.emplace_back("\\]", TokenType::T_BRACKETR);

    // Literals and identifiers
    tokenPatterns.emplace_back("[0-9]+", TokenType::T_INLIT);
    tokenPatterns.emplace_back("[a-zA-Z_][a-zA-Z0-9_]*", TokenType::T_IDENTIFIER);
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
        if (pos + 1 < input.length() && input[pos] == '/' && input[pos + 1] == '/') {
            pos += 2;
            while (pos < input.length() && input[pos] != '\n') {
                pos++;
            }
            continue;
        }
        if (pos + 1 < input.length() && input[pos] == '/' && input[pos + 1] == '*') {
            pos += 2;
            int startLine = line;
            int startColumn = column;
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
                throw runtime_error("Unterminated comment at line " + to_string(startLine) + ", column " + to_string(startColumn));
            }
            pos += 2; // Skip closing */
            column += 2;
            continue;
        }
        break;
    }
}

Token Lexer::processString() {
    size_t startColumn = column;
    pos++; // Skip opening quote
    column++;
    string content;
    
    while (pos < input.length() && input[pos] != '"') {
        if (input[pos] == '\\') {
            pos++;
            column++;
            if (pos >= input.length()) {
                throw runtime_error("Unterminated string literal at line " + to_string(line) + ", column " + to_string(startColumn));
            }
            switch (input[pos]) {
                case 'n': content += '\n'; break;
                case 't': content += '\t'; break;
                case 'r': content += '\r'; break;
                case '\\': content += '\\'; break;
                case '"': content += '"'; break;
                case '0': content += '\0'; break;
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
        throw runtime_error("Unterminated string literal at line " + to_string(line) + ", column " + to_string(startColumn));
    }
    
    pos++; // Skip closing quote
    column++;
    return Token(TokenType::T_STRINGLIT, content, line, startColumn);
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    
    while (pos < input.length()) {
        skipWhitespaceAndComment();
        if (pos >= input.length()) break;
        
        int currentLine = line;
        int currentColumn = column;
        
        // Handle string literals first
        if (input[pos] == '"') {
            tokens.push_back(Token(TokenType::T_QOUTES, "\"", line, column));
            Token stringToken = processString();
            tokens.push_back(stringToken);
            if (pos < input.length() && input[pos] == '"') {
                tokens.push_back(Token(TokenType::T_QOUTES, "\"", line, column));
                pos++;
                column++;
            }
            continue;
        }
        
        bool matched = false;
        for (const auto& pattern : tokenPatterns) {
            smatch match;
            string substr = input.substr(pos);
            
            if (regex_search(substr, match, pattern.pattern, regex_constants::match_continuous)) {
                if (!match.empty() && match.position() == 0) {
                    string matchedStr = match.str();
                    
                    // Check if it's a keyword and not part of a larger identifier
                    if (pattern.isKeyword) {
                        size_t nextPos = pos + matchedStr.length();
                        if (nextPos < input.length() && (isalnum(input[nextPos]) || input[nextPos] == '_')) {
                            continue; // Not a keyword, part of larger identifier
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