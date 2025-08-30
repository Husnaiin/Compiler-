#include "lexer.h"

Lexer::Lexer(const string& input, bool isFile=false)
    : input(input), currentIndex(0), lineNumber(1), columnNumber(1) {
    if(isFile){
        ifstream file(input);
        if(!file.is_open()){
            throw runtime_error("Could not open file: " + input);
        }
        stringstream buffer;
        buffer << file.rdbuf();
        this->input = buffer.str();
    }

}

vector<Token> Lexer::tokenize() {
    tokens.clear();
    currentIndex = 0;
    lineNumber = 1;
    columnNumber = 1;

    while(!isAtEnd()) {
        start = currentIndex;
        startColumn = columnNumber;
        scanNextToken();
    }

    addToken(TokenType::T_EOF, "");
    return tokens;
}

void Lexer::scanNextToken(){
    char currentChar = advance();
    switch(currentChar){
        case '(':
            addToken(TokenType::T_LPAREN);
            break;
        case ')':
            addToken(TokenType::T_RPAREN);
            break;
        case '{':
            addToken(TokenType::T_LBRACE);
            break;
        case '}':
            addToken(TokenType::T_RBRACE);
            break;
        case '[':
            addToken(TokenType::T_LBRACKET);
            break;
        case ']':
            addToken(TokenType::T_RBRACKET);
            break;
        case ';':
            addToken(TokenType::T_SEMICOLON);
            break;
        case ',':
            addToken(TokenType::T_COMMA);
            break;
        case ':':
            addToken(TokenType::T_COLON);
            break;
        case '.':
            addToken(TokenType::T_DOT);
            break;
        case '"':
            scanString();
            break;
        case '\'':
            scanChar();
            break;
        case '0':
            scanNumber();
            break;
        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9':
            // handle Number literal
            scanNumber();
            break;
        case '+':
            if (peek() == '+') {
                advance();
                addToken(TokenType::T_INCREMENT);
            } else if (peek() == '=') {
                advance();
                addToken(TokenType::T_PLUS_ASSIGN);
            } else {
                addToken(TokenType::T_PLUS);
            }
            break;
        case '-':
            if (peek() == '-') {
                advance();
                addToken(TokenType::T_DECREMENT);
            } else if (peek() == '=') {
                advance();
                addToken(TokenType::T_MINUS_ASSIGN);
            } else {
                addToken(TokenType::T_MINUS);
            }
            break;
        case '*':
            if (peek() == '=') {
                advance();
                addToken(TokenType::T_MULT_ASSIGN);
            } else {
                addToken(TokenType::T_MULTIPLY);
            }
            break;
        case '/':
            if (peek() == '=') {
                advance();
                addToken(TokenType::T_DIV_ASSIGN);
            } else if (peek() == '/') {
                // line comment
                while (!isAtEnd() && peek() != '\n') advance();
            } else if (peek() == '*') {
                // block comment
                advance(); // consume '*'
                while (!isAtEnd()) {
                    if (peek() == '*' && peek(1) == '/') {
                        advance(); // '*'
                        advance(); // '/'
                        break;
                    }
                    advance();
                }
            } else {
                addToken(TokenType::T_DIVIDE);
            }
            break;
        case '%':
            if (peek() == '=') {
                advance();
                addToken(TokenType::T_MOD_ASSIGN);
            } else {
                addToken(TokenType::T_MODULO);
            }
            break;
        case '=':
            if (peek() == '=') {
                advance();
                addToken(TokenType::T_EQUAL);
            } else {
                addToken(TokenType::T_ASSIGN);
            }
            break;
        case '!':
            if (peek() == '=') {
                advance();
                addToken(TokenType::T_NOT_EQUAL);
            } else {
                addToken(TokenType::T_NOT);
            }
            break;
        case '>':
            if (peek() == '=') {
                advance();
                addToken(TokenType::T_GREATER_THAN_EQUAL);
            } else if (peek() == '>') {
                advance();
                if (peek() == '=') {
                    advance();
                    addToken(TokenType::T_RSHIFT_ASSIGN);
                } else {
                    addToken(TokenType::T_RIGHT_SHIFT);
                }
            } else {
                addToken(TokenType::T_GREATER_THAN);
            }
            break;
        case '<':
            if (peek() == '=') {
                advance();
                addToken(TokenType::T_LESS_THAN_EQUAL);
            } else if (peek() == '<') {
                advance();
                if (peek() == '=') {
                    advance();
                    addToken(TokenType::T_LSHIFT_ASSIGN);
                } else {
                    addToken(TokenType::T_LEFT_SHIFT);
                }
            } else {
                addToken(TokenType::T_LESS_THAN);
            }
            break;
        case '&':
            if (peek() == '&') {
                advance();
                addToken(TokenType::T_AND);
            } else if (peek() == '=') {
                advance();
                addToken(TokenType::T_AND_ASSIGN);
            } else {
                addToken(TokenType::T_BIT_AND);
            }
            break;
        case '|':
            if (peek() == '|') {
                advance();
                addToken(TokenType::T_OR);
            } else if (peek() == '=') {
                advance();
                addToken(TokenType::T_OR_ASSIGN);
            } else {
                addToken(TokenType::T_BIT_OR);
            }
            break;
        case '^':
            if (peek() == '=') {
                advance();
                addToken(TokenType::T_XOR_ASSIGN);
            } else {
                addToken(TokenType::T_BIT_XOR);
            }
            break;
        case '~':
            addToken(TokenType::T_BIT_NOT);
            break;
        case '?':
            addToken(TokenType::T_QUESTION);
            break;
        case ' ':
            break;
        case '\t':
            break;
        case '\r':
            break;
        case '\n':
            break;
        default:
            if (isDigit(currentChar)) {
                scanNumber();
            } else if (isAlpha(currentChar)) {
                scanIdentifier();
            } else {
                addToken(TokenType::T_ERROR);
            }
            break;
    }
}

void Lexer::scanNumber() {
    while (isDigit(peek())) advance();
    bool isFloat = false;
    if (peek() == '.' && isDigit(peek(1))) {
        isFloat = true;
        advance();
        while (isDigit(peek())) advance();
    }

    // If after number we immediately have an alpha/underscore, it's invalid identifier like 123abc
    if (isAlpha(peek())) {
        throw runtime_error("Invalid identifier: identifiers cannot start with a number at line " + to_string(lineNumber));
    }

    if (isFloat) {
        addToken(TokenType::T_FLOAT_LITERAL);
    } else {
        addToken(TokenType::T_INT_LITERAL);
    }
}

void Lexer::scanIdentifier() {
    while (isAlphaNumeric(peek())) advance();
    
    string text = input.substr(start, currentIndex - start);
    TokenType type = getKeywordType(text);
    addToken(type);
}

void Lexer::scanString() {
    // emit opening quotes token
    addToken(TokenType::T_QUOTES, "\"");
    start = currentIndex; // start of content
    startColumn = columnNumber;
    string value;
    while (!isAtEnd()) {
        char c = peek();
        if (c == '"') break;
        if (c == '\\') {
            advance(); // consume '\\'
            char e = peek();
            if (isAtEnd()) break;
            switch (e) {
                case 'n': value.push_back('\n'); break;
                case 't': value.push_back('\t'); break;
                case 'r': value.push_back('\r'); break;
                case '\\': value.push_back('\\'); break;
                case '"': value.push_back('"'); break;
                case '\'': value.push_back('\''); break;
                default: value.push_back(e); break;
            }
            advance();
            continue;
        }
        value.push_back(c);
        advance();
    }
    if (isAtEnd()) { throw runtime_error("Unterminated string literal at line " + to_string(lineNumber)); }
    advance(); // consume closing '"'
    addToken(TokenType::T_STRING_LITERAL, value);
    // emit closing quotes token
    addToken(TokenType::T_QUOTES, "\"");
}

void Lexer::scanChar() {
    // opening quote already consumed in caller
    char c;
    if (isAtEnd()) throw runtime_error("Unterminated char literal at line " + to_string(lineNumber));
    if (peek() == '\\') {
        advance();
        if (isAtEnd()) throw runtime_error("Unterminated char escape at line " + to_string(lineNumber));
        char e = peek();
        switch (e) {
            case 'n': c = '\n'; break;
            case 't': c = '\t'; break;
            case 'r': c = '\r'; break;
            case '\\': c = '\\'; break;
            case '\'': c = '\''; break;
            case '"': c = '"'; break;
            default: c = e; break;
        }
        advance();
    } else {
        c = peek();
        advance();
    }
    if (peek() != '\'') throw runtime_error("Unterminated char literal at line " + to_string(lineNumber));
    advance(); // consume closing quote
    string value(1, c);
    addToken(TokenType::T_CHAR_LITERAL, value);
}

TokenType Lexer::getKeywordType(const string& text) {
    if (text == "function" || text == "fn") return TokenType::T_FUNCTION;
    if (text == "int") return TokenType::T_INT;
    if (text == "float") return TokenType::T_FLOAT;
    if (text == "string") return TokenType::T_STRING;
    if (text == "bool") return TokenType::T_BOOL;
    if (text == "char") return TokenType::T_CHAR;
    if (text == "if") return TokenType::T_IF;
    if (text == "else") return TokenType::T_ELSE;
    if (text == "elif") return TokenType::T_ELIF;
    if (text == "while") return TokenType::T_WHILE;
    if (text == "for") return TokenType::T_FOR;
    if (text == "do") return TokenType::T_DO;
    if (text == "switch") return TokenType::T_SWITCH;
    if (text == "case") return TokenType::T_CASE;
    if (text == "default") return TokenType::T_DEFAULT;
    if (text == "return") return TokenType::T_RETURN;
    if (text == "break") return TokenType::T_BREAK;
    if (text == "continue") return TokenType::T_CONTINUE;
    if (text == "print") return TokenType::T_PRINT;
    if (text == "read") return TokenType::T_READ;
    if (text == "true") return TokenType::T_TRUE;
    if (text == "false") return TokenType::T_FALSE;
    if (text == "null") return TokenType::T_NULL;
    if (text == "void") return TokenType::T_VOID;
    if (text == "include") return TokenType::T_INCLUDE;
    
    return TokenType::T_IDENTIFIER;
}


bool Lexer::addToken(TokenType type){
    string lexeme = input.substr(start, currentIndex - start); 
    tokens.push_back(Token(type, lexeme, lineNumber, startColumn));
    return true;
}

bool Lexer::addToken(TokenType type, const string& lexeme){
    tokens.push_back(Token(type, lexeme, lineNumber, startColumn));
    return true;
}



char Lexer::peek() const{
    if(isAtEnd()) return '\0';
    return input[currentIndex];
}

char Lexer::peek(int offset) const {
    if(currentIndex + offset >= input.length()) return '\0';
    return input[currentIndex + offset];   
}

char Lexer::advance(){
    if(isAtEnd()) return '\0';
    char c = input[currentIndex++];
    if(c == '\n'){
        lineNumber++;
        columnNumber = 1;
    } else if (c == '\t') {
        columnNumber += 4;
    } else {
        columnNumber++;
    }
    return c;   
}

bool Lexer::isAtEnd() const{
    return currentIndex >= input.length();
}

bool Lexer::isDigit(char c) const{
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) const{
    return isDigit(c) || isAlpha(c);
}

bool Lexer::isSpace(char c) const{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}


