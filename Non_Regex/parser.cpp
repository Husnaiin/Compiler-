#include "parser.h"
#include <iostream>
#include <sstream>

// AST Node Implementations

// Program Node
std::string ProgramNode::toString() const {
    std::ostringstream oss;
    oss << "Program(";
    
    bool first = true;

    // Add global variables
    for (size_t i = 0; i < globalVariables.size(); i++) {
        if (!first) oss << ", ";
        oss << "GlobalVar(" << globalVariables[i]->toString() << ")";
        first = false;
    }
    
    // Add functions
    for (size_t i = 0; i < functions.size(); i++) {
        if (!first) oss << ", ";
        oss << functions[i]->toString();
        first = false;
    }
    
    oss << ")";
    return oss.str();
}

void ProgramNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Program:" << std::endl;
    
    // Print global variables
    if (!globalVariables.empty()) {
        std::cout << std::string(indent + 2, ' ') << "Global Variables:" << std::endl;
        for (const auto& var : globalVariables) {
            var->print(indent + 4);
        }
    }
    
    // Print functions
    if (!functions.empty()) {
        std::cout << std::string(indent + 2, ' ') << "Functions:" << std::endl;
        for (const auto& func : functions) {
            func->print(indent + 4);
        }
    }
}

// Function Node
std::string FunctionNode::toString() const {
    std::ostringstream oss;
    oss << "Function(" << name << ", " << returnType << ", ";
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) oss << ", ";
        oss << parameters[i]->toString();
    }
    oss << ", " << body->toString() << ")";
    return oss.str();
}

void FunctionNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Function: " << name 
              << " (return type: " << returnType << ")" << std::endl;
    
    if (!parameters.empty()) {
        std::cout << std::string(indent + 2, ' ') << "Parameters:" << std::endl;
        for (const auto& param : parameters) {
            param->print(indent + 4);
        }
    }
    
    std::cout << std::string(indent + 2, ' ') << "Body:" << std::endl;
    body->print(indent + 4);
}

// Parameter Node
std::string ParameterNode::toString() const {
    return "Parameter(" + name + ", " + type + ")";
}

void ParameterNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Parameter: " << name 
              << " (type: " << type << ")" << std::endl;
}

// Binary Operation Node
std::string BinaryOpNode::toString() const {
    return "BinaryOp(" + op + ", " + left->toString() + ", " + right->toString() + ")";
}

void BinaryOpNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BinaryOp: " << op << std::endl;
    left->print(indent + 2);
    right->print(indent + 2);
}

// Unary Operation Node
std::string UnaryOpNode::toString() const {
    return "UnaryOp(" + op + ", " + operand->toString() + ")";
}

void UnaryOpNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "UnaryOp: " << op << std::endl;
    operand->print(indent + 2);
}

// Literal Node
std::string LiteralNode::toString() const {
    return "Literal(" + value + ", " + type + ")";
}

void LiteralNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Literal: " << value 
              << " (type: " << type << ")" << std::endl;
}

// Identifier Node
std::string IdentifierNode::toString() const {
    return "Identifier(" + name + ")";
}

void IdentifierNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Identifier: " << name << std::endl;
}

// Block Node
std::string BlockNode::toString() const {
    std::ostringstream oss;
    oss << "Block(";
    for (size_t i = 0; i < statements.size(); i++) {
        if (i > 0) oss << ", ";
        oss << statements[i]->toString();
    }
    oss << ")";
    return oss.str();
}

void BlockNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Block:" << std::endl;
    for (const auto& stmt : statements) {
        stmt->print(indent + 2);
    }
}

// If Statement Node
std::string IfStatementNode::toString() const {
    std::string elseStr = elseBlock ? elseBlock->toString() : "null";
    return "IfStatement(" + condition->toString() + ", " + thenBlock->toString() + ", " + elseStr + ")";
}

void IfStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "IfStatement:" << std::endl;
    std::cout << std::string(indent + 2, ' ') << "Condition:" << std::endl;
    condition->print(indent + 4);
    std::cout << std::string(indent + 2, ' ') << "Then:" << std::endl;
    thenBlock->print(indent + 4);
    if (elseBlock) {
        std::cout << std::string(indent + 2, ' ') << "Else:" << std::endl;
        elseBlock->print(indent + 4);
    }
}

// While Statement Node
std::string WhileStatementNode::toString() const {
    return "WhileStatement(" + condition->toString() + ", " + body->toString() + ")";
}

void WhileStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "WhileStatement:" << std::endl;
    std::cout << std::string(indent + 2, ' ') << "Condition:" << std::endl;
    condition->print(indent + 4);
    std::cout << std::string(indent + 2, ' ') << "Body:" << std::endl;
    body->print(indent + 4);
}

// For Statement Node
std::string ForStatementNode::toString() const {
    std::string initStr = initialization ? initialization->toString() : "null";
    std::string condStr = condition ? condition->toString() : "null";
    std::string updateStr = update ? update->toString() : "null";
    return "ForStatement(" + initStr + ", " + condStr + ", " + updateStr + ", " + body->toString() + ")";
}

void ForStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ForStatement:" << std::endl;
    if (initialization) {
        std::cout << std::string(indent + 2, ' ') << "Initialization:" << std::endl;
        initialization->print(indent + 4);
    }
    if (condition) {
        std::cout << std::string(indent + 2, ' ') << "Condition:" << std::endl;
        condition->print(indent + 4);
    }
    if (update) {
        std::cout << std::string(indent + 2, ' ') << "Update:" << std::endl;
        update->print(indent + 4);
    }
    std::cout << std::string(indent + 2, ' ') << "Body:" << std::endl;
    body->print(indent + 4);
}

// Do-While Statement Node
std::string DoWhileStatementNode::toString() const {
    return "DoWhileStatement(" + body->toString() + ", " + condition->toString() + ")";
}

void DoWhileStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "DoWhileStatement:" << std::endl;
    std::cout << std::string(indent + 2, ' ') << "Body:" << std::endl;
    body->print(indent + 4);
    std::cout << std::string(indent + 2, ' ') << "Condition:" << std::endl;
    condition->print(indent + 4);
}

// Return Statement Node
std::string ReturnStatementNode::toString() const {
    std::string exprStr = expression ? expression->toString() : "null";
    return "ReturnStatement(" + exprStr + ")";
}

void ReturnStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ReturnStatement:" << std::endl;
    if (expression) {
        expression->print(indent + 2);
    }
}

// Break Statement Node
std::string BreakStatementNode::toString() const {
    return "BreakStatement()";
}

void BreakStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BreakStatement" << std::endl;
}

// Continue Statement Node
std::string ContinueStatementNode::toString() const {
    return "ContinueStatement()";
}

void ContinueStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ContinueStatement" << std::endl;
}

// Assignment Node
std::string AssignmentNode::toString() const {
    return "Assignment(" + variable + ", " + value->toString() + ")";
}

void AssignmentNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Assignment: " << variable << std::endl;
    value->print(indent + 2);
}

// Print Statement Node
std::string PrintStatementNode::toString() const {
    std::string exprStr = expression ? expression->toString() : "null";
    return "PrintStatement(" + exprStr + ")";
}

void PrintStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "PrintStatement:" << std::endl;
    if (expression) {
        expression->print(indent + 2);
    }
}

// Switch Statement Node
std::string SwitchStatementNode::toString() const {
    std::ostringstream oss;
    oss << "SwitchStatement(" << expression->toString() << ", ";
    for (size_t i = 0; i < cases.size(); i++) {
        if (i > 0) oss << ", ";
        oss << cases[i]->toString();
    }
    if (defaultCase) {
        oss << ", " << defaultCase->toString();
    }
    oss << ")";
    return oss.str();
}

void SwitchStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "SwitchStatement:" << std::endl;
    std::cout << std::string(indent + 2, ' ') << "Expression:" << std::endl;
    expression->print(indent + 4);
    
    for (const auto& caseStmt : cases) {
        caseStmt->print(indent + 2);
    }
    
    if (defaultCase) {
        defaultCase->print(indent + 2);
    }
}

// Case Statement Node
std::string CaseStatementNode::toString() const {
    std::ostringstream oss;
    oss << "CaseStatement(" << value->toString() << ", ";
    for (size_t i = 0; i < statements.size(); i++) {
        if (i > 0) oss << ", ";
        oss << statements[i]->toString();
    }
    oss << ")";
    return oss.str();
}

void CaseStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "CaseStatement:" << std::endl;
    std::cout << std::string(indent + 2, ' ') << "Value:" << std::endl;
    value->print(indent + 4);
    
    for (const auto& stmt : statements) {
        stmt->print(indent + 2);
    }
}

// Default Statement Node
std::string DefaultStatementNode::toString() const {
    std::ostringstream oss;
    oss << "DefaultStatement(";
    for (size_t i = 0; i < statements.size(); i++) {
        if (i > 0) oss << ", ";
        oss << statements[i]->toString();
    }
    oss << ")";
    return oss.str();
}

void DefaultStatementNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "DefaultStatement:" << std::endl;
    for (const auto& stmt : statements) {
        stmt->print(indent + 2);
    }
}

// Variable Declaration Node
std::string VariableDeclarationNode::toString() const {
    std::string initStr = initializer ? initializer->toString() : "null";
    return "VariableDeclaration(" + type + ", " + name + ", " + initStr + ")";
}

void VariableDeclarationNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "VariableDeclaration: " << type << " " << name << std::endl;
    if (initializer) {
        std::cout << std::string(indent + 2, ' ') << "Initializer:" << std::endl;
        initializer->print(indent + 4);
    }
}

// Function Call Node
std::string FunctionCallNode::toString() const {
    std::ostringstream oss;
    oss << "FunctionCall(" << functionName << ", ";
    for (size_t i = 0; i < arguments.size(); i++) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->toString();
    }
    oss << ")";
    return oss.str();
}

void FunctionCallNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "FunctionCall: " << functionName << std::endl;
    for (const auto& arg : arguments) {
        arg->print(indent + 2);
    }
}

// Ternary Node
std::string TernaryNode::toString() const {
    return "Ternary(" + condition->toString() + ", " + trueValue->toString() + ", " + falseValue->toString() + ")";
}

void TernaryNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Ternary:" << std::endl;
    std::cout << std::string(indent + 2, ' ') << "Condition:" << std::endl;
    condition->print(indent + 4);
    std::cout << std::string(indent + 2, ' ') << "True Value:" << std::endl;
    trueValue->print(indent + 4);
    std::cout << std::string(indent + 2, ' ') << "False Value:" << std::endl;
    falseValue->print(indent + 4);
}

// Array Declaration Node
std::string ArrayDeclarationNode::toString() const {
    std::string sizeStr = size ? size->toString() : "null";
    std::string initStr = initializer ? initializer->toString() : "null";
    return "ArrayDeclaration(" + type + ", " + name + ", " + sizeStr + ", " + initStr + ")";
}

void ArrayDeclarationNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ArrayDeclaration: " << type << " " << name;
    if (size) {
        std::cout << "[" << size->toString() << "]";
    }
    std::cout << std::endl;
    if (initializer) {
        std::cout << std::string(indent + 2, ' ') << "Initializer:" << std::endl;
        initializer->print(indent + 4);
    }
}

// Array Access Node
std::string ArrayAccessNode::toString() const {
    return "ArrayAccess(" + arrayName + ", " + index->toString() + ")";
}

void ArrayAccessNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ArrayAccess: " << arrayName << "[" << index->toString() << "]" << std::endl;
}

// Array Initialization Node
std::string ArrayInitializationNode::toString() const {
    std::ostringstream oss;
    oss << "ArrayInitialization(";
    for (size_t i = 0; i < elements.size(); i++) {
        if (i > 0) oss << ", ";
        oss << elements[i]->toString();
    }
    oss << ")";
    return oss.str();
}

void ArrayInitializationNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ArrayInitialization:" << std::endl;
    for (const auto& element : elements) {
        element->print(indent + 2);
    }
}

// Pointer Declaration Node
std::string PointerDeclarationNode::toString() const {
    std::string initStr = initializer ? initializer->toString() : "null";
    return "PointerDeclaration(" + baseType + "*, " + name + ", " + initStr + ")";
}

void PointerDeclarationNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "PointerDeclaration: " << baseType << "* " << name << std::endl;
    if (initializer) {
        std::cout << std::string(indent + 2, ' ') << "Initializer:" << std::endl;
        initializer->print(indent + 4);
    }
}

// Dereference Node
std::string DereferenceNode::toString() const {
    return "Dereference(" + operand->toString() + ")";
}

void DereferenceNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Dereference:" << std::endl;
    operand->print(indent + 2);
}

// Address Of Node
std::string AddressOfNode::toString() const {
    return "AddressOf(" + operand->toString() + ")";
}

void AddressOfNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "AddressOf:" << std::endl;
    operand->print(indent + 2);
}











// Parser Implementation

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

bool Parser::isAtEnd() const {
    return peek().getType() == TokenType::T_EOF;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().getType() == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::error(const std::string& message) {
    std::cerr << "Parse Error: " << message << std::endl;
}

ParseError Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().getType() == TokenType::T_SEMICOLON) return ParseError::UnexpectedToken;
        
        switch (peek().getType()) {
            case TokenType::T_FUNCTION:
            case TokenType::T_INT:
            case TokenType::T_FLOAT:
            case TokenType::T_STRING:
            case TokenType::T_CHAR:
            case TokenType::T_BOOL:
            case TokenType::T_VOID:
            case TokenType::T_IF:
            case TokenType::T_WHILE:
            case TokenType::T_RETURN:
            case TokenType::T_PRINT:
                return ParseError::UnexpectedToken;
            default:
                advance();
        }
    }
    
    return ParseError::UnexpectedToken;
}

std::shared_ptr<ProgramNode> Parser::parse() {
    return parseProgram();
}

std::shared_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_shared<ProgramNode>();
    
    while (!isAtEnd()) {
        if (check(TokenType::T_FUNCTION)) {
            // Function declaration
            program->functions.push_back(parseFunction());
        } else if (check(TokenType::T_INT) || check(TokenType::T_FLOAT) || 
                   check(TokenType::T_STRING) || check(TokenType::T_CHAR) || 
                   check(TokenType::T_BOOL) || check(TokenType::T_VOID)) {
            // Check if this is a function declaration (has parentheses after identifier)
            size_t saveCurrent = current;
            advance(); // consume type
            if (check(TokenType::T_IDENTIFIER)) {
                advance(); // consume identifier
                if (check(TokenType::T_LPAREN)) {
                    // This is a function declaration
                    current = saveCurrent;
                    program->functions.push_back(parseFunction());
                } else {
                    // This is a global variable declaration
                    current = saveCurrent;
                    program->globalVariables.push_back(parseGlobalVariableDeclaration());
                }
            } else {
                current = saveCurrent;
                break;
            }
        } else {
            break;
        }
    }
    
    return program;
}

std::shared_ptr<FunctionNode> Parser::parseFunction() {
    auto function = std::make_shared<FunctionNode>();
    
    // Parse return type
    if (match({TokenType::T_FUNCTION})) {
        function->returnType = "void";
        if (match({TokenType::T_INT, TokenType::T_FLOAT, TokenType::T_STRING, 
                   TokenType::T_CHAR, TokenType::T_BOOL, TokenType::T_VOID})) {
            function->returnType = previous().getLexeme();
        }
    } else if (match({TokenType::T_INT, TokenType::T_FLOAT, TokenType::T_STRING, 
                      TokenType::T_CHAR, TokenType::T_BOOL, TokenType::T_VOID})) {
        function->returnType = previous().getLexeme();
    } else {
        error("Expected function return type");
        return nullptr;
    }
    
    // Parse function name
    if (!check(TokenType::T_IDENTIFIER)) {
        error("Expected function name");
        return nullptr;
    }
    Token nameToken = advance();
    function->name = nameToken.getLexeme();
    
    // Parse parameters
    if (!check(TokenType::T_LPAREN)) {
        error("Expected '(' after function name");
        return nullptr;
    }
    advance(); // consume '('
    
    if (!check(TokenType::T_RPAREN)) {
        do {
            function->parameters.push_back(parseParameter());
        } while (match({TokenType::T_COMMA}));
    }
    
    if (!check(TokenType::T_RPAREN)) {
        error("Expected ')' after parameters");
        return nullptr;
    }
    advance(); // consume ')'
    
    // Parse function body
    function->body = parseBlock();
    
    return function;
}

std::shared_ptr<ParameterNode> Parser::parseParameter() {
    auto param = std::make_shared<ParameterNode>();
    
    // Parse parameter type
    if (!match({TokenType::T_INT, TokenType::T_FLOAT, TokenType::T_STRING, 
                TokenType::T_CHAR, TokenType::T_BOOL})) {
        error("Expected parameter type");
        return nullptr;
    }
    param->type = previous().getLexeme();
    
    // Parse parameter name
    if (!check(TokenType::T_IDENTIFIER)) {
        error("Expected parameter name");
        return nullptr;
    }
    param->name = advance().getLexeme();
    
    return param;
}

std::shared_ptr<StatementNode> Parser::parseStatement() {
    if (match({TokenType::T_IF})) {
        return parseIfStatement();
    }
    if (match({TokenType::T_WHILE})) {
        return parseWhileStatement();
    }
    if (match({TokenType::T_FOR})) {
        return parseForStatement();
    }
    if (match({TokenType::T_DO})) {
        return parseDoWhileStatement();
    }
    if (match({TokenType::T_SWITCH})) {
        return parseSwitchStatement();
    }
    if (match({TokenType::T_RETURN})) {
        return parseReturnStatement();
    }
    if (match({TokenType::T_BREAK})) {
        return parseBreakStatement();
    }
    if (match({TokenType::T_CONTINUE})) {
        return parseContinueStatement();
    }
    if (match({TokenType::T_PRINT})) {
        return parsePrintStatement();
    }
    if (match({TokenType::T_INT, TokenType::T_FLOAT, TokenType::T_STRING, 
               TokenType::T_CHAR, TokenType::T_BOOL})) {
        return parseVariableDeclaration();
    }
    if (check(TokenType::T_IDENTIFIER)) {
        return parseAssignmentStatement();
    }
    
    error("Expected statement");
    return nullptr;
}

std::shared_ptr<ExpressionNode> Parser::parseExpression() {
    return parseAssignment();
}

std::shared_ptr<ExpressionNode> Parser::parseAssignment() {
    auto expr = parseTernary();
    
    if (match({TokenType::T_ASSIGN, TokenType::T_PLUS_ASSIGN, TokenType::T_MINUS_ASSIGN,
               TokenType::T_MULT_ASSIGN, TokenType::T_DIV_ASSIGN, TokenType::T_MOD_ASSIGN,
               TokenType::T_AND_ASSIGN, TokenType::T_OR_ASSIGN, TokenType::T_XOR_ASSIGN,
               TokenType::T_LSHIFT_ASSIGN, TokenType::T_RSHIFT_ASSIGN})) {
        Token equals = previous();
        auto value = parseAssignment();
        
        auto assignment = std::make_shared<BinaryOpNode>();
        assignment->op = equals.getLexeme();
        assignment->left = expr;
        assignment->right = value;
        
        return assignment;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseOr() {
    return parseLogicalOr();
}

std::shared_ptr<ExpressionNode> Parser::parseAnd() {
    return parseLogicalAnd();
}

std::shared_ptr<ExpressionNode> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match({TokenType::T_EQUAL, TokenType::T_NOT_EQUAL})) {
        Token op = previous();
        auto right = parseComparison();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseComparison() {
    auto expr = parseShift();
    
    while (match({TokenType::T_GREATER_THAN, TokenType::T_GREATER_THAN_EQUAL,
                 TokenType::T_LESS_THAN, TokenType::T_LESS_THAN_EQUAL})) {
        Token op = previous();
        auto right = parseShift();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseTerm() {
    return parseAdditive();
}

std::shared_ptr<ExpressionNode> Parser::parseAdditive() {
    auto expr = parseMultiplicative();
    
    while (match({TokenType::T_PLUS, TokenType::T_MINUS})) {
        Token op = previous();
        auto right = parseMultiplicative();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseMultiplicative() {
    auto expr = parseUnary();
    
    while (match({TokenType::T_MULTIPLY, TokenType::T_DIVIDE, TokenType::T_MODULO})) {
        Token op = previous();
        auto right = parseUnary();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseFactor() {
    return parseMultiplicative();
}

std::shared_ptr<ExpressionNode> Parser::parseUnary() {
    if (match({TokenType::T_NOT, TokenType::T_MINUS, TokenType::T_BIT_NOT})) {
        Token op = previous();
        auto right = parseUnary();
        
        auto unary = std::make_shared<UnaryOpNode>();
        unary->op = op.getLexeme();
        unary->operand = right;
        return unary;
    }
    
    // Handle dereference operator (*)
    if (match({TokenType::T_MULTIPLY})) {
        auto operand = parseUnary();
        auto deref = std::make_shared<DereferenceNode>();
        deref->operand = operand;
        return deref;
    }
    
    // Handle address-of operator (&)
    if (match({TokenType::T_BIT_AND})) {
        auto operand = parseUnary();
        auto addr = std::make_shared<AddressOfNode>();
        addr->operand = operand;
        return addr;
    }
    
    // Handle increment/decrement operators
    if (match({TokenType::T_INCREMENT, TokenType::T_DECREMENT})) {
        Token op = previous();
        if (!check(TokenType::T_IDENTIFIER)) {
            error("Expected identifier after " + op.getLexeme());
            return nullptr;
        }
        auto identifier = std::make_shared<IdentifierNode>();
        identifier->name = advance().getLexeme();
        
        auto unary = std::make_shared<UnaryOpNode>();
        unary->op = op.getLexeme();
        unary->operand = identifier;
        return unary;
    }
    
    return parsePrimary();
}

std::shared_ptr<ExpressionNode> Parser::parsePrimary() {
    if (match({TokenType::T_TRUE, TokenType::T_FALSE})) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = previous().getLexeme();
        literal->type = "bool";
        return literal;
    }
    
    if (match({TokenType::T_INT_LITERAL, TokenType::T_HEX_LITERAL, 
               TokenType::T_OCTAL_LITERAL, TokenType::T_BINARY_LITERAL})) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = previous().getLexeme();
        literal->type = "int";
        return literal;
    }
    
    if (match({TokenType::T_FLOAT_LITERAL})) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = previous().getLexeme();
        literal->type = "float";
        return literal;
    }
    
    if (match({TokenType::T_STRING_LITERAL})) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = previous().getLexeme();
        literal->type = "string";
        return literal;
    }
    
    if (match({TokenType::T_CHAR_LITERAL})) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = previous().getLexeme();
        literal->type = "char";
        return literal;
    }
    
    if (match({TokenType::T_IDENTIFIER})) {
        if (check(TokenType::T_LPAREN)) {
            return parseFunctionCall();
        } else {
            auto identifier = std::make_shared<IdentifierNode>();
            identifier->name = previous().getLexeme();
            
            // Check for array access
            if (check(TokenType::T_LBRACKET)) {
                advance(); // consume '['
                auto index = parseExpression();
                if (!check(TokenType::T_RBRACKET)) {
                    error("Expected ']' after array index");
                    return nullptr;
                }
                advance(); // consume ']'
                
                auto arrayAccess = std::make_shared<ArrayAccessNode>();
                arrayAccess->arrayName = identifier->name;
                arrayAccess->index = index;
                return arrayAccess;
            }
            
            return identifier;
        }
    }
    
    if (match({TokenType::T_LPAREN})) {
        auto expr = parseExpression();
        if (!check(TokenType::T_RPAREN)) {
            error("Expected ')' after expression");
            return nullptr;
        }
        advance(); // consume ')'
        return expr;
    }
    
    error("Expected expression");
    return nullptr;
}

std::shared_ptr<IfStatementNode> Parser::parseIfStatement() {
    auto ifStmt = std::make_shared<IfStatementNode>();
    
    if (!check(TokenType::T_LPAREN)) {
        error("Expected '(' after if");
        return nullptr;
    }
    advance(); // consume '('
    
    ifStmt->condition = parseExpression();
    
    if (!check(TokenType::T_RPAREN)) {
        error("Expected ')' after condition");
        return nullptr;
    }
    advance(); // consume ')'
    
    ifStmt->thenBlock = parseBlock();
    
    if (match({TokenType::T_ELSE})) {
        ifStmt->elseBlock = parseBlock();
    }
    
    return ifStmt;
}

std::shared_ptr<WhileStatementNode> Parser::parseWhileStatement() {
    auto whileStmt = std::make_shared<WhileStatementNode>();
    
    if (!check(TokenType::T_LPAREN)) {
        error("Expected '(' after while");
        return nullptr;
    }
    advance(); // consume '('
    
    whileStmt->condition = parseExpression();
    
    if (!check(TokenType::T_RPAREN)) {
        error("Expected ')' after condition");
        return nullptr;
    }
    advance(); // consume ')'
    
    whileStmt->body = parseBlock();
    
    return whileStmt;
}

std::shared_ptr<ForStatementNode> Parser::parseForStatement() {
    auto forStmt = std::make_shared<ForStatementNode>();
    
    if (!check(TokenType::T_LPAREN)) {
        error("Expected '(' after for");
        return nullptr;
    }
    advance(); // consume '('
    
    // Parse initialization (optional)
    if (!check(TokenType::T_SEMICOLON)) {
        if (match({TokenType::T_INT, TokenType::T_FLOAT, TokenType::T_STRING, 
                   TokenType::T_CHAR, TokenType::T_BOOL})) {
            forStmt->initialization = parseVariableDeclaration();
        } else {
            forStmt->initialization = parseAssignmentStatement();
        }
    } else {
        advance(); // consume ';'
    }
    
    // Parse condition (optional)
    if (!check(TokenType::T_SEMICOLON)) {
        forStmt->condition = parseExpression();
    }
    if (!check(TokenType::T_SEMICOLON)) {
        error("Expected ';' after condition");
        return nullptr;
    }
    advance(); // consume ';'
    
    // Parse update (optional)
    if (!check(TokenType::T_RPAREN)) {
        forStmt->update = parseAssignmentStatement();
    }
    
    if (!check(TokenType::T_RPAREN)) {
        error("Expected ')' after for loop");
        return nullptr;
    }
    advance(); // consume ')'
    
    forStmt->body = parseBlock();
    
    return forStmt;
}

std::shared_ptr<DoWhileStatementNode> Parser::parseDoWhileStatement() {
    auto doWhileStmt = std::make_shared<DoWhileStatementNode>();
    
    // Parse the body first
    doWhileStmt->body = parseBlock();
    
    // Expect 'while' keyword
    if (!match({TokenType::T_WHILE})) {
        error("Expected 'while' after do block");
        return nullptr;
    }
    
    // Parse condition
    if (!check(TokenType::T_LPAREN)) {
        error("Expected '(' after while");
        return nullptr;
    }
    advance(); // consume '('
    
    doWhileStmt->condition = parseExpression();
    
    if (!check(TokenType::T_RPAREN)) {
        error("Expected ')' after condition");
        return nullptr;
    }
    advance(); // consume ')'
    
    return doWhileStmt;
}

std::shared_ptr<ReturnStatementNode> Parser::parseReturnStatement() {
    auto returnStmt = std::make_shared<ReturnStatementNode>();
    
    if (!check(TokenType::T_SEMICOLON)) {
        returnStmt->expression = parseExpression();
    }
    
    if (!check(TokenType::T_SEMICOLON)) {
        error("Expected ';' after return statement");
        return nullptr;
    }
    advance(); // consume ';'
    
    return returnStmt;
}

std::shared_ptr<BreakStatementNode> Parser::parseBreakStatement() {
    auto breakStmt = std::make_shared<BreakStatementNode>();
    
    if (!check(TokenType::T_SEMICOLON)) {
        error("Expected ';' after break statement");
        return nullptr;
    }
    advance(); // consume ';'
    
    return breakStmt;
}

std::shared_ptr<ContinueStatementNode> Parser::parseContinueStatement() {
    auto continueStmt = std::make_shared<ContinueStatementNode>();
    
    if (!check(TokenType::T_SEMICOLON)) {
        error("Expected ';' after continue statement");
        return nullptr;
    }
    advance(); // consume ';'
    
    return continueStmt;
}

std::shared_ptr<PrintStatementNode> Parser::parsePrintStatement() {
    auto printStmt = std::make_shared<PrintStatementNode>();
    
    // Parse the expression to print
    printStmt->expression = parseExpression();
    
    if (!check(TokenType::T_SEMICOLON)) {
        error("Expected ';' after print statement");
        return nullptr;
    }
    advance(); // consume ';'
    
    return printStmt;
}

std::shared_ptr<AssignmentNode> Parser::parseAssignmentStatement() {
    auto assignment = std::make_shared<AssignmentNode>();
    
    if (!check(TokenType::T_IDENTIFIER)) {
        error("Expected identifier");
        return nullptr;
    }
    assignment->variable = advance().getLexeme();
    
    if (!match({TokenType::T_ASSIGN})) {
        error("Expected '=' after identifier");
        return nullptr;
    }
    
    assignment->value = parseExpression();
    
    if (!check(TokenType::T_SEMICOLON)) {
        error("Expected ';' after assignment");
        return nullptr;
    }
    advance(); // consume ';'
    
    return assignment;
}

std::shared_ptr<VariableDeclarationNode> Parser::parseVariableDeclaration() {
    auto varDecl = std::make_shared<VariableDeclarationNode>();
    
    // Type is already consumed by match() in parseStatement()
    varDecl->type = previous().getLexeme();
    
    // Parse variable name
    if (!check(TokenType::T_IDENTIFIER)) {
        error("Expected variable name");
        return nullptr;
    }
    varDecl->name = advance().getLexeme();
    
    // Check for initialization
    if (match({TokenType::T_ASSIGN})) {
        varDecl->initializer = parseExpression();
    }
    
    if (!check(TokenType::T_SEMICOLON)) {
        error("Expected ';' after variable declaration");
        return nullptr;
    }
    advance(); // consume ';'
    
    return varDecl;
}

std::shared_ptr<VariableDeclarationNode> Parser::parseGlobalVariableDeclaration() {
    auto varDecl = std::make_shared<VariableDeclarationNode>();
    
    // Parse variable type
    if (!match({TokenType::T_INT, TokenType::T_FLOAT, TokenType::T_STRING, 
                TokenType::T_CHAR, TokenType::T_BOOL})) {
        error("Expected variable type");
        return nullptr;
    }
    varDecl->type = previous().getLexeme();
    
    // Parse variable name
    if (!check(TokenType::T_IDENTIFIER)) {
        error("Expected variable name");
        return nullptr;
    }
    varDecl->name = advance().getLexeme();
    
    // Check for initialization
    if (match({TokenType::T_ASSIGN})) {
        varDecl->initializer = parseExpression();
    }
    
    if (!check(TokenType::T_SEMICOLON)) {
        error("Expected ';' after global variable declaration");
        return nullptr;
    }
    advance(); // consume ';'
    
    return varDecl;
}

std::shared_ptr<BlockNode> Parser::parseBlock() {
    auto block = std::make_shared<BlockNode>();
    
    if (!check(TokenType::T_LBRACE)) {
        error("Expected '{'");
        return nullptr;
    }
    advance(); // consume '{'
    
    while (!check(TokenType::T_RBRACE) && !isAtEnd()) {
        block->statements.push_back(parseStatement());
    }
    
    if (!check(TokenType::T_RBRACE)) {
        error("Expected '}'");
        return nullptr;
    }
    advance(); // consume '}'
    
    return block;
}

std::shared_ptr<FunctionCallNode> Parser::parseFunctionCall() {
    auto call = std::make_shared<FunctionCallNode>();
    call->functionName = previous().getLexeme();
    
    if (!check(TokenType::T_LPAREN)) {
        error("Expected '(' after function name");
        return nullptr;
    }
    advance(); // consume '('
    
    if (!check(TokenType::T_RPAREN)) {
        do {
            call->arguments.push_back(parseExpression());
        } while (match({TokenType::T_COMMA}));
    }
    
    if (!check(TokenType::T_RPAREN)) {
        error("Expected ')' after arguments");
        return nullptr;
    }
    advance(); // consume ')'
    
    return call;
}

std::shared_ptr<SwitchStatementNode> Parser::parseSwitchStatement() {
    auto switchStmt = std::make_shared<SwitchStatementNode>();
    
    if (!check(TokenType::T_LPAREN)) {
        error("Expected '(' after switch");
        return nullptr;
    }
    advance(); // consume '('
    
    switchStmt->expression = parseExpression();
    
    if (!check(TokenType::T_RPAREN)) {
        error("Expected ')' after switch expression");
        return nullptr;
    }
    advance(); // consume ')'
    
    if (!check(TokenType::T_LBRACE)) {
        error("Expected '{' after switch");
        return nullptr;
    }
    advance(); // consume '{'
    
    // Parse cases and default
    while (!isAtEnd() && !check(TokenType::T_RBRACE)) {
        if (match({TokenType::T_CASE})) {
            switchStmt->cases.push_back(parseCaseStatement());
        } else if (match({TokenType::T_DEFAULT})) {
            if (switchStmt->defaultCase) {
                error("Multiple default cases in switch statement");
                return nullptr;
            }
            switchStmt->defaultCase = parseDefaultStatement();
        } else {
            error("Expected 'case' or 'default' in switch statement");
            return nullptr;
        }
    }
    
    if (!check(TokenType::T_RBRACE)) {
        error("Expected '}' after switch statement");
        return nullptr;
    }
    advance(); // consume '}'
    
    return switchStmt;
}

std::shared_ptr<CaseStatementNode> Parser::parseCaseStatement() {
    auto caseStmt = std::make_shared<CaseStatementNode>();
    
    caseStmt->value = parseExpression();
    
    if (!match({TokenType::T_COLON})) {
        error("Expected ':' after case value");
        return nullptr;
    }
    
    // Parse statements until next case/default/}
    while (!isAtEnd() && !check(TokenType::T_CASE) && 
           !check(TokenType::T_DEFAULT) && !check(TokenType::T_RBRACE)) {
        auto stmt = parseStatement();
        if (stmt) {
            caseStmt->statements.push_back(stmt);
        } else {
            // If parsing fails, try to synchronize
            if (synchronize() == ParseError::UnexpectedEOF) {
                break;
            }
        }
    }
    
    return caseStmt;
}

std::shared_ptr<DefaultStatementNode> Parser::parseDefaultStatement() {
    auto defaultStmt = std::make_shared<DefaultStatementNode>();
    
    if (!match({TokenType::T_COLON})) {
        error("Expected ':' after default");
        return nullptr;
    }
    
    // Parse statements until next case/default/}
    while (!isAtEnd() && !check(TokenType::T_CASE) && 
           !check(TokenType::T_DEFAULT) && !check(TokenType::T_RBRACE)) {
        auto stmt = parseStatement();
        if (stmt) {
            defaultStmt->statements.push_back(stmt);
        } else {
            // If parsing fails, try to synchronize
            if (synchronize() == ParseError::UnexpectedEOF) {
                break;
            }
        }
    }
    
    return defaultStmt;
}

std::shared_ptr<ExpressionNode> Parser::parseTernary() {
    auto expr = parseLogicalOr();
    
    if (match({TokenType::T_QUESTION})) {
        auto trueValue = parseExpression();
        if (!match({TokenType::T_COLON})) {
            error("Expected ':' in ternary operator");
            return nullptr;
        }
        auto falseValue = parseTernary();
        
        auto ternary = std::make_shared<TernaryNode>();
        ternary->condition = expr;
        ternary->trueValue = trueValue;
        ternary->falseValue = falseValue;
        return ternary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match({TokenType::T_OR})) {
        Token op = previous();
        auto right = parseLogicalAnd();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseLogicalAnd() {
    auto expr = parseBitwiseOr();
    
    while (match({TokenType::T_AND})) {
        Token op = previous();
        auto right = parseBitwiseOr();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseBitwiseOr() {
    auto expr = parseBitwiseXor();
    
    while (match({TokenType::T_BIT_OR})) {
        Token op = previous();
        auto right = parseBitwiseXor();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseBitwiseXor() {
    auto expr = parseBitwiseAnd();
    
    while (match({TokenType::T_BIT_XOR})) {
        Token op = previous();
        auto right = parseBitwiseAnd();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseBitwiseAnd() {
    auto expr = parseEquality();
    
    while (match({TokenType::T_BIT_AND})) {
        Token op = previous();
        auto right = parseEquality();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseShift() {
    auto expr = parseAdditive();
    
    while (match({TokenType::T_LEFT_SHIFT, TokenType::T_RIGHT_SHIFT})) {
        Token op = previous();
        auto right = parseAdditive();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->op = op.getLexeme();
        binary->left = expr;
        binary->right = right;
        expr = binary;
    }
    
    return expr;
}

void Parser::printAST(const std::shared_ptr<ProgramNode>& program) {
    if (program) {
        program->print();
    } else {
        std::cout << "No AST generated" << std::endl;
    }
}
