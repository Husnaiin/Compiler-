#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "token.h"
#include "ast.h"

enum class ParseErrorType {
    UnexpectedEOF,
    FailedToFindToken,
    ExpectedTypeToken,
    ExpectedIdentifier,
    UnexpectedToken,
    ExpectedFloatLit,
    ExpectedIntLit,
    ExpectedStringLit,
    ExpectedBoolLit,
    ExpectedExpr,
};

struct ParseError {
    ParseErrorType type;
    Token token;
    std::string message;
    
    ParseError(ParseErrorType t, Token tok, const std::string& msg = "")
        : type(t), token(tok), message(msg) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    // Error handling
    ParseError errorAtCurrent(const std::string& message);
    ParseError errorAtToken(Token token, const std::string& message);

    std::vector<ParseError> errors;

    // Add these declarations:
    Token advance();
    Token peek() const;
    Token previous() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool isAtEnd() const;
    Token consume(TokenType type, const std::string& errorMessage);
    void synchronize(); // <-- This fixes your main error

    // Pratt parsing functions
    ExprPtr parseExpression(int precedence = 0);
    ExprPtr parsePrimary();
    int getPrecedence(TokenType type) const;

    // Declaration parsing
    DeclPtr parseDeclaration();
    std::unique_ptr<FnDecl> parseFunctionDeclaration();
    std::unique_ptr<VarDecl> parseVariableDeclaration(); // Add this

    // Statement parsing
    StmtPtr parseStatement();
    StmtPtr parseIfStatement();
    StmtPtr parseForStatement();
    StmtPtr parseBreakStatement(); // Add this line
    StmtPtr parseReturnStatement();
    StmtPtr parseExprStatement();
    StmtPtr parseBlock(); // Add this

    // Helper functions
    vector<VarDecl> parseParameterList();
    std::vector<ExprPtr> parseArgumentList();

public:
    Parser(std::vector<Token> tokens);
    Program parse();
    const std::vector<ParseError>& getErrors() const;
};

#endif // PARSER_H