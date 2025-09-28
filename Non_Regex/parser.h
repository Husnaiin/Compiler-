#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "token.h"
#include <vector>
#include <memory>
#include <string>

// Parse Error Types
enum class ParseError {
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
    ExpectedStatement,
    ExpectedFunctionBody,
    ExpectedParameterList,
    ExpectedBlock
};

// Forward declarations for AST nodes
class ASTNode;
class ProgramNode;
class FunctionNode;
class ParameterNode;
class StatementNode;
class ExpressionNode;
class BinaryOpNode;
class UnaryOpNode;
class LiteralNode;
class IdentifierNode;
class BlockNode;
class IfStatementNode;
class WhileStatementNode;
class ReturnStatementNode;
class AssignmentNode;
class VariableDeclarationNode;
class FunctionCallNode;

// AST Node Base Class
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
    virtual void print(int indent = 0) const = 0;
};

// Program Node
class ProgramNode : public ASTNode {
public:
    std::vector<std::shared_ptr<FunctionNode>> functions;
    std::vector<std::shared_ptr<VariableDeclarationNode>> globalVariables;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Function Node
class FunctionNode : public ASTNode {
public:
    std::string name;
    std::string returnType;
    std::vector<std::shared_ptr<ParameterNode>> parameters;
    std::shared_ptr<BlockNode> body;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Parameter Node
class ParameterNode : public ASTNode {
public:
    std::string name;
    std::string type;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Statement Node Base Class
class StatementNode : public ASTNode {
public:
    virtual ~StatementNode() = default;
};

// Expression Node Base Class
class ExpressionNode : public ASTNode {
public:
    virtual ~ExpressionNode() = default;
};

// Binary Operation Node
class BinaryOpNode : public ExpressionNode {
public:
    std::string op;
    std::shared_ptr<ExpressionNode> left;
    std::shared_ptr<ExpressionNode> right;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Unary Operation Node
class UnaryOpNode : public ExpressionNode {
public:
    std::string op;
    std::shared_ptr<ExpressionNode> operand;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Literal Node
class LiteralNode : public ExpressionNode {
public:
    std::string value;
    std::string type;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Identifier Node
class IdentifierNode : public ExpressionNode {
public:
    std::string name;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Block Node
class BlockNode : public ASTNode {
public:
    std::vector<std::shared_ptr<StatementNode>> statements;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// If Statement Node
class IfStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> condition;
    std::shared_ptr<BlockNode> thenBlock;
    std::shared_ptr<BlockNode> elseBlock;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// While Statement Node
class WhileStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> condition;
    std::shared_ptr<BlockNode> body;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// For Statement Node
class ForStatementNode : public StatementNode {
public:
    std::shared_ptr<StatementNode> initialization;
    std::shared_ptr<ExpressionNode> condition;
    std::shared_ptr<StatementNode> update;
    std::shared_ptr<BlockNode> body;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Do-While Statement Node
class DoWhileStatementNode : public StatementNode {
public:
    std::shared_ptr<BlockNode> body;
    std::shared_ptr<ExpressionNode> condition;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Return Statement Node
class ReturnStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> expression;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Break Statement Node
class BreakStatementNode : public StatementNode {
public:
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Continue Statement Node
class ContinueStatementNode : public StatementNode {
public:
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Assignment Node
class AssignmentNode : public StatementNode {
public:
    std::string variable;
    std::shared_ptr<ExpressionNode> value;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Print Statement Node
class PrintStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> expression;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Variable Declaration Node
class VariableDeclarationNode : public StatementNode {
public:
    std::string type;
    std::string name;
    std::shared_ptr<ExpressionNode> initializer;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Function Call Node
class FunctionCallNode : public ExpressionNode {
public:
    std::string functionName;
    std::vector<std::shared_ptr<ExpressionNode>> arguments;
    
    std::string toString() const override;
    void print(int indent = 0) const override;
};

// Parser Class
class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    // Helper methods
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    
    // Error handling
    void error(const std::string& message);
    ParseError synchronize();
    
    // Parsing methods
    std::shared_ptr<ProgramNode> parseProgram();
    std::shared_ptr<FunctionNode> parseFunction();
    std::shared_ptr<ParameterNode> parseParameter();
    std::shared_ptr<StatementNode> parseStatement();
    std::shared_ptr<ExpressionNode> parseExpression();
    std::shared_ptr<ExpressionNode> parseAssignment();
    std::shared_ptr<ExpressionNode> parseOr();
    std::shared_ptr<ExpressionNode> parseAnd();
    std::shared_ptr<ExpressionNode> parseEquality();
    std::shared_ptr<ExpressionNode> parseComparison();
    std::shared_ptr<ExpressionNode> parseTerm();
    std::shared_ptr<ExpressionNode> parseFactor();
    std::shared_ptr<ExpressionNode> parseUnary();
    std::shared_ptr<ExpressionNode> parsePrimary();
    
    // Statement parsing
    std::shared_ptr<IfStatementNode> parseIfStatement();
    std::shared_ptr<WhileStatementNode> parseWhileStatement();
    std::shared_ptr<ForStatementNode> parseForStatement();
    std::shared_ptr<DoWhileStatementNode> parseDoWhileStatement();
    std::shared_ptr<ReturnStatementNode> parseReturnStatement();
    std::shared_ptr<BreakStatementNode> parseBreakStatement();
    std::shared_ptr<ContinueStatementNode> parseContinueStatement();
    std::shared_ptr<PrintStatementNode> parsePrintStatement();
    std::shared_ptr<AssignmentNode> parseAssignmentStatement();
    std::shared_ptr<VariableDeclarationNode> parseVariableDeclaration();
    std::shared_ptr<VariableDeclarationNode> parseGlobalVariableDeclaration();
    std::shared_ptr<BlockNode> parseBlock();
    
    // Expression parsing helpers
    std::shared_ptr<FunctionCallNode> parseFunctionCall();
    std::shared_ptr<LiteralNode> parseLiteral();
    
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ProgramNode> parse();
    void printAST(const std::shared_ptr<ProgramNode>& program);
};

#endif
