#include "parser.h"
#include <stdexcept>
#include <iostream>
#include <map>

using namespace std;

Parser::Parser(vector<Token> tokens) : tokens(tokens), current(0) {}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::T_EOF;
}

Token Parser::consume(TokenType type, const string& errorMessage) {
    if (check(type)) return advance();
    throw errorAtCurrent(errorMessage);
}

ParseError Parser::errorAtCurrent(const string& message) {
    return errorAtToken(peek(), message);
}

ParseError Parser::errorAtToken(Token token, const string& message) {
    ParseError error(ParseErrorType::UnexpectedToken, token, message);
    errors.push_back(error);
    return error;
}

int Parser::getPrecedence(TokenType type) const {
    static const map<TokenType, int> precedence = {
        {TokenType::T_ASSIGN, 10},
        {TokenType::T_LOGICAL_OR, 20},
        {TokenType::T_LOGICAL_AND, 30},
        {TokenType::T_EQUAL, 40}, {TokenType::T_NOT_EQUAL, 40},
        {TokenType::T_LESS, 50}, {TokenType::T_LESS_EQUAL, 50},
        {TokenType::T_GREATER, 50}, {TokenType::T_GREATER_EQUAL, 50},
        {TokenType::T_PLUS, 60}, {TokenType::T_MINUS, 60},
        {TokenType::T_ASTERISK, 70}, {TokenType::T_DIVIDE, 70}, {TokenType::T_MOD, 70},
    };
    
    auto it = precedence.find(type);
    return it != precedence.end() ? it->second : 0;
}

ExprPtr Parser::parseExpression(int minPrecedence) {
    ExprPtr left = parsePrimary();
    
    while (true) {
        Token opToken = peek();
        if (opToken.type == TokenType::T_EOF) break;
        
        int opPrecedence = getPrecedence(opToken.type);
        if (opPrecedence <= minPrecedence) break;
        
        advance();
        ExprPtr right = parseExpression(opPrecedence);
        left = make_unique<BinaryExpr>(opToken.type, move(left), move(right));
    }
    
    return left;
}

ExprPtr Parser::parsePrimary() {
    if (match(TokenType::T_INTLIT)) {
        try {
            int value = stoi(previous().value);
            return make_unique<IntLit>(value);
        } catch (...) {
            throw errorAtToken(previous(), "Invalid integer literal");
        }
    }
    
    if (match(TokenType::T_FLOATLIT)) {
        try {
            double value = stod(previous().value);
            return make_unique<FloatLit>(value);
        } catch (...) {
            throw errorAtToken(previous(), "Invalid float literal");
        }
    }
    
    if (match(TokenType::T_BOOLLIT)) {
        bool value = previous().value == "true";
        return make_unique<BoolLit>(value);
    }
    
    if (match(TokenType::T_STRINGLIT)) {
        return make_unique<StringLit>(previous().value);
    }
    
    if (match(TokenType::T_IDENTIFIER)) {
        string name = previous().value;
        if (match(TokenType::T_LPAREN)) {
            vector<ExprPtr> args;
            if (!check(TokenType::T_RPAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::T_COMMA));
            }
            consume(TokenType::T_RPAREN, "Expected ')' after arguments");
            return make_unique<CallExpr>(name, move(args));
        }
        return make_unique<Ident>(name);
    }
    
    if (match(TokenType::T_LPAREN)) {
        ExprPtr expr = parseExpression();
        consume(TokenType::T_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    if (match(TokenType::T_MINUS) || match(TokenType::T_LOGICAL_NOT)) {
        TokenType op = previous().type;
        ExprPtr expr = parseExpression(getPrecedence(op));
        return make_unique<UnaryExpr>(op, move(expr));
    }
    
    throw errorAtCurrent("Expected expression");
}

DeclPtr Parser::parseDeclaration() {
    try {
        if (match(TokenType::T_FN)) {
            return parseFunctionDeclaration();
        }
        
        // Check for variable declaration types
        if (peek().type == TokenType::T_INT || peek().type == TokenType::T_FLOAT || 
            peek().type == TokenType::T_BOOL || peek().type == TokenType::T_STRING) {
            return parseVariableDeclaration();
        }
        
        // Try to parse as expression statement (for error recovery)
        if (check(TokenType::T_IDENTIFIER) || check(TokenType::T_INTLIT) || 
            check(TokenType::T_FLOATLIT) || check(TokenType::T_STRINGLIT)) {
            return nullptr; // Let statement parsing handle it
        }
        
        throw errorAtCurrent("Expected declaration");
    } catch (const ParseError& e) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<FnDecl> Parser::parseFunctionDeclaration() {
    // Parse return type (custom language uses T_INT for "ginti")
    Token returnTypeToken = advance(); // Consume the type (should be T_INT for "ginti")
    
    // Function name
    Token nameToken = consume(TokenType::T_IDENTIFIER, "Expected function name");
    
    // Parameters
    consume(TokenType::T_LPAREN, "Expected '(' after function name");
    vector<VarDecl> params = parseParameterList();
    consume(TokenType::T_RPAREN, "Expected ')' after parameters");
    
    // Function body
    consume(TokenType::T_LBRACE, "Expected '{' before function body");
    vector<StmtPtr> body;
    while (!check(TokenType::T_RBRACE) && !isAtEnd()) {
        body.push_back(parseStatement());
    }
    consume(TokenType::T_RBRACE, "Expected '}' after function body");
    
    // Optional dot after function (for custom language)
    if (check(TokenType::T_DOT)) {
        advance();
    }
    
    return make_unique<FnDecl>(returnTypeToken.type, nameToken.value, move(params), move(body));
}

std::unique_ptr<VarDecl> Parser::parseVariableDeclaration() {
    Token typeToken = advance();
    Token nameToken = consume(TokenType::T_IDENTIFIER, "Expected variable name");
    
    ExprPtr init = nullptr;
    if (match(TokenType::T_ASSIGN)) {
        init = parseExpression();
    }
    
    // Custom language uses dot as statement terminator
    if (check(TokenType::T_DOT)) {
        advance();
    }
    
    return make_unique<VarDecl>(typeToken.type, nameToken.value, move(init));
}

StmtPtr Parser::parseStatement() {
    try {
        if (match(TokenType::T_RETURN)) return parseReturnStatement();
        if (match(TokenType::T_IF)) return parseIfStatement();
        if (match(TokenType::T_FOR)) return parseForStatement();
        if (match(TokenType::T_BREAK)) return parseBreakStatement();
        if (match(TokenType::T_LBRACE)) return parseBlock();
        
        // Try to parse as expression statement or variable declaration
        if (peek().type == TokenType::T_INT || peek().type == TokenType::T_FLOAT || 
            peek().type == TokenType::T_BOOL || peek().type == TokenType::T_STRING) {
            return parseVariableDeclaration();
        }
        
        return parseExprStatement();
    } catch (const ParseError& e) {
        synchronize();
        return make_unique<ExprStmt>(make_unique<IntLit>(0)); // Return dummy statement
    }
}

StmtPtr Parser::parseExprStatement() {
    ExprPtr expr = parseExpression();
    if (check(TokenType::T_DOT)) {
        advance();
    }
    return make_unique<ExprStmt>(move(expr));
}

StmtPtr Parser::parseReturnStatement() {
    ExprPtr expr = nullptr;
    if (!check(TokenType::T_DOT)) {
        expr = parseExpression();
    }
    if (check(TokenType::T_DOT)) {
        advance();
    }
    return make_unique<ReturnStmt>(move(expr));
}

StmtPtr Parser::parseBreakStatement() {
    if (check(TokenType::T_DOT)) {
        advance();
    }
    return make_unique<BreakStmt>();
}

StmtPtr Parser::parseIfStatement() {
    consume(TokenType::T_LPAREN, "Expected '(' after 'if'");
    ExprPtr cond = parseExpression();
    consume(TokenType::T_RPAREN, "Expected ')' after condition");
    
    vector<StmtPtr> thenBlock;
    if (match(TokenType::T_LBRACE)) {
        while (!check(TokenType::T_RBRACE) && !isAtEnd()) {
            thenBlock.push_back(parseStatement());
        }
        consume(TokenType::T_RBRACE, "Expected '}' after if block");
    } else {
        thenBlock.push_back(parseStatement());
    }
    
    vector<StmtPtr> elseBlock;
    if (match(TokenType::T_ELSE)) {
        if (match(TokenType::T_LBRACE)) {
            while (!check(TokenType::T_RBRACE) && !isAtEnd()) {
                elseBlock.push_back(parseStatement());
            }
            consume(TokenType::T_RBRACE, "Expected '}' after else block");
        } else {
            elseBlock.push_back(parseStatement());
        }
    }
    
    return make_unique<IfStmt>(move(cond), move(thenBlock), move(elseBlock));
}

StmtPtr Parser::parseForStatement() {
    consume(TokenType::T_LPAREN, "Expected '(' after 'for'");
    
    unique_ptr<VarDecl> init = nullptr;
    if (!check(TokenType::T_SEMICOLON)) {
        if (peek().type == TokenType::T_INT || peek().type == TokenType::T_FLOAT || 
            peek().type == TokenType::T_BOOL || peek().type == TokenType::T_STRING) {
            init = parseVariableDeclaration();
        } else {
            // Expression init (not supported in custom language)
            advance(); // Skip
        }
    }
    
    ExprPtr cond = nullptr;
    if (!check(TokenType::T_SEMICOLON) && !check(TokenType::T_DOT)) {
        cond = parseExpression();
    }
    if (check(TokenType::T_DOT)) {
        advance();
    }
    
    ExprPtr update = nullptr;
    if (!check(TokenType::T_RPAREN)) {
        update = parseExpression();
    }
    consume(TokenType::T_RPAREN, "Expected ')' after for loop update");
    
    vector<StmtPtr> body;
    if (match(TokenType::T_LBRACE)) {
        while (!check(TokenType::T_RBRACE) && !isAtEnd()) {
            body.push_back(parseStatement());
        }
        consume(TokenType::T_RBRACE, "Expected '}' after for loop body");
    } else {
        body.push_back(parseStatement());
    }
    
    return make_unique<ForStmt>(move(init), move(cond), move(update), move(body));
}

StmtPtr Parser::parseBlock() {
    vector<StmtPtr> statements;
    while (!check(TokenType::T_RBRACE) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }
    consume(TokenType::T_RBRACE, "Expected '}' after block");
    return make_unique<BlockStmt>(move(statements));
}

vector<VarDecl> Parser::parseParameterList() {
    vector<VarDecl> params;
    if (!check(TokenType::T_RPAREN)) {
        do {
            Token typeToken = advance(); // Parameter type
            Token nameToken = consume(TokenType::T_IDENTIFIER, "Expected parameter name");
            params.emplace_back(typeToken.type, nameToken.value, nullptr);
        } while (match(TokenType::T_COMMA));
    }
    return params;
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::T_DOT) return;
        
        switch (peek().type) {
            case TokenType::T_FN:
            case TokenType::T_INT:
            case TokenType::T_FLOAT:
            case TokenType::T_BOOL:
            case TokenType::T_STRING:
            case TokenType::T_RETURN:
            case TokenType::T_IF:
            case TokenType::T_FOR:
            case TokenType::T_BREAK:
                return;
            default:
                advance();
        }
    }
}

Program Parser::parse() {
    Program program;
    while (!isAtEnd()) {
        try {
            DeclPtr decl = parseDeclaration();
            if (decl) {
                program.push_back(move(decl));
            } else {
                // Skip unexpected tokens
                if (!isAtEnd()) advance();
            }
        } catch (const ParseError& e) {
            synchronize();
        }
    }
    return program;
}

const vector<ParseError>& Parser::getErrors() const {
    return errors;
}