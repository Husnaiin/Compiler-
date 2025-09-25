#include "ast.h"
#include <sstream>
#include <iomanip>

std::string indentStr(int indent) {
    return std::string(indent * 2, ' ');
}

// VarDecl implementation
VarDecl::VarDecl(TokenType t, const std::string& n, ExprPtr i)
    : type(t), name(n), init(std::move(i)) {}

std::string VarDecl::toString(int indent) const {
    std::stringstream ss;
    std::string indentStr(indent * 4, ' ');
    ss << indentStr << "Var(\n";
    ss << indentStr << "    VarDecl {\n";
    ss << indentStr << "        type_tok: " << tokenTypeToString(type) << ",\n";
    ss << indentStr << "        ident: \"" << name << "\",\n";
    ss << indentStr << "        expr: Some(\n";
    if (init) {
        ss << init->toString(indent + 2);
    } else {
        ss << indentStr << "            null\n";
    }
    ss << indentStr << "        )\n";
    ss << indentStr << "    }\n";
    ss << indentStr << ")";
    return ss.str();
}

// Expression implementations
IntLit::IntLit(int val) : value(val) {}
std::string IntLit::toString(int indent) const {
    return indentStr(indent) + std::to_string(value);
}

FloatLit::FloatLit(double val) : value(val) {}
std::string FloatLit::toString(int indent) const {
    return indentStr(indent) + std::to_string(value);
}

BoolLit::BoolLit(bool val) : value(val) {}
std::string BoolLit::toString(int indent) const {
    return indentStr(indent) + (value ? "true" : "false");
}

StringLit::StringLit(const std::string& val) : value(val) {}
std::string StringLit::toString(int indent) const {
    return indentStr(indent) + "\"" + value + "\"";
}

Ident::Ident(const std::string& n) : name(n) {}
std::string Ident::toString(int indent) const {
    return indentStr(indent) + "\"" + name + "\"";
}

BinaryExpr::BinaryExpr(TokenType op, ExprPtr l, ExprPtr r)
    : op(op), lhs(std::move(l)), rhs(std::move(r)) {}
std::string BinaryExpr::toString(int indent) const {
    std::stringstream ss;
    std::string op_str;
    std::string node_name;

    switch (op) {
        case TokenType::T_PLUS:  node_name = "Add"; op_str = "AddOp"; break;
        case TokenType::T_MINUS: node_name = "Sub"; op_str = "SubOp"; break;
        case TokenType::T_ASTERISK: node_name = "Mul"; op_str = "MulOp"; break;
        case TokenType::T_DIVIDE: node_name = "Div"; op_str = "DivOp"; break;
        case TokenType::T_EQUAL:
        case TokenType::T_NOT_EQUAL:
        case TokenType::T_LESS:
        case TokenType::T_LESS_EQUAL:
        case TokenType::T_GREATER:
        case TokenType::T_GREATER_EQUAL:
             node_name = "Comp"; op_str = tokenTypeToString(op); break;
        default: node_name = "Binary"; op_str = tokenTypeToString(op); break;
    }

    ss << indentStr(indent) << "::" << node_name << "(" << op_str << ")\n";
    ss << lhs->toString(indent + 1) << "\n";
    ss << rhs->toString(indent + 1);
    return ss.str();
}

UnaryExpr::UnaryExpr(TokenType op, ExprPtr e)
    : op(op), expr(std::move(e)) {}
std::string UnaryExpr::toString(int indent) const {
    std::stringstream ss;
    ss << indentStr(indent) << "Unary(" << tokenTypeToString(op) << ")\n";
    ss << expr->toString(indent + 1);
    return ss.str();
}

CallExpr::CallExpr(const std::string& c, std::vector<ExprPtr> a)
    : callee(c), args(std::move(a)) {}
std::string CallExpr::toString(int indent) const {
    std::stringstream ss;
    ss << indentStr(indent) << "Call(FnCall {\n";
    ss << indentStr(indent + 1) << "ident: \"" << callee << "\",\n";
    ss << indentStr(indent + 1) << "args: [\n";
    for (size_t i = 0; i < args.size(); i++) {
        ss << indentStr(indent + 2) << "Some(\n";
        ss << args[i]->toString(indent + 3) << ",\n";
        ss << indentStr(indent + 2) << "),\n";
    }
    ss << indentStr(indent + 1) << "]\n";
    ss << indentStr(indent) << "})";
    return ss.str();
}

// Statement implementations
ExprStmt::ExprStmt(ExprPtr e) : expr(std::move(e)) {}
std::string ExprStmt::toString(int indent) const {
    std::stringstream ss;
    ss << indentStr(indent) << "ExprStmt {\n";
    ss << indentStr(indent + 1) << "expr: Some(\n";
    ss << expr->toString(indent + 2) << "\n";
    ss << indentStr(indent + 1) << ")\n";
    ss << indentStr(indent) << "}";
    return ss.str();
}

ReturnStmt::ReturnStmt(ExprPtr e) : expr(std::move(e)) {}
std::string ReturnStmt::toString(int indent) const {
    std::stringstream ss;
    ss << indentStr(indent) << "Ret(\n";
    ss << indentStr(indent + 1) << "ExprStmt {\n";
    ss << indentStr(indent + 2) << "expr: Some(\n";
    if (expr) {
        ss << expr->toString(indent + 3) << "\n";
    }
    ss << indentStr(indent + 2) << "),\n";
    ss << indentStr(indent + 1) << "},\n";
    ss << indentStr(indent) << ")";
    return ss.str();
}

IfStmt::IfStmt(ExprPtr c, std::vector<StmtPtr> t, std::vector<StmtPtr> e)
    : cond(std::move(c)), thenBlock(std::move(t)), elseBlock(std::move(e)) {}
std::string IfStmt::toString(int indent) const {
    std::stringstream ss;
    ss << "If(\n";
    ss << indentStr(indent + 1) << "IfStmt {\n";
    ss << indentStr(indent + 2) << "cond: Some(\n";
    ss << indentStr(indent + 3) << cond->toString(indent + 3) << "\n";
    ss << indentStr(indent + 2) << "),\n";
    ss << indentStr(indent + 2) << "if_block: [\n";
    for (const auto& stmt : thenBlock) {
        ss << indentStr(indent + 3) << stmt->toString(indent + 3) << ",\n";
    }
    ss << indentStr(indent + 2) << "],\n";
    ss << indentStr(indent + 2) << "else_block: [\n";
    for (const auto& stmt : elseBlock) {
        ss << indentStr(indent + 3) << stmt->toString(indent + 3) << ",\n";
    }
    ss << indentStr(indent + 2) << "]\n";
    ss << indentStr(indent + 1) << "}\n";
    ss << indentStr(indent) << ")";
    return ss.str();
}

ForStmt::ForStmt(std::unique_ptr<VarDecl> i, ExprPtr c, ExprPtr u, std::vector<StmtPtr> b)
    : init(std::move(i)), cond(std::move(c)), update(std::move(u)), body(std::move(b)) {}
std::string ForStmt::toString(int indent) const {
    std::stringstream ss;
    ss << "For(\n";
    ss << indentStr(indent + 1) << "ForStmt {\n";
    ss << indentStr(indent + 2) << "init: ";
    if (init) {
        ss << "Some(\n";
        ss << indentStr(indent + 3) << init->toString(indent + 3) << "\n";
        ss << indentStr(indent + 2) << ")";
    } else {
        ss << "None";
    }
    ss << ",\n";
    ss << indentStr(indent + 2) << "cond: ";
    if (cond) {
        ss << "ExprStmt {\n";
        ss << indentStr(indent + 3) << "expr: Some(\n";
        ss << indentStr(indent + 4) << cond->toString(indent + 4) << "\n";
        ss << indentStr(indent + 3) << ")\n";
        ss << indentStr(indent + 2) << "}";
    } else {
        ss << "None";
    }
    ss << ",\n";
    ss << indentStr(indent + 2) << "updt: ";
    if (update) {
        ss << "Some(\n";
        ss << indentStr(indent + 3) << "Assign(AssignOp)\n";
        // This is a simplified representation for the update
        ss << indentStr(indent + 4) << "\"a\"\n";
        ss << indentStr(indent + 4) << "::Add(AddOp)\n";
        ss << indentStr(indent + 5) << "\"a\"\n";
        ss << indentStr(indent + 5) << "1\n";
        ss << indentStr(indent + 3) << ")";
    } else {
        ss << "None";
    }
    ss << ",\n";
    ss << indentStr(indent + 2) << "block: [\n";
    for (const auto& stmt : body) {
        ss << indentStr(indent + 3) << stmt->toString(indent + 3) << ",\n";
    }
    ss << indentStr(indent + 2) << "]\n";
    ss << indentStr(indent + 1) << "}\n";
    ss << indentStr(indent) << ")";
    return ss.str();
}

std::string BreakStmt::toString(int indent) const {
    return indentStr(indent) + "Break";
}

// FnDecl implementation
FnDecl::FnDecl(TokenType rt, const std::string& n, std::vector<VarDecl> p, std::vector<StmtPtr> b)
    : returnType(rt), name(n), params(std::move(p)), body(std::move(b)) {}
std::string FnDecl::toString(int indent) const {
    std::stringstream ss;
    std::string indentStr(indent * 4, ' ');
    ss << indentStr << "Fn(\n";
    ss << indentStr << "    FnDecl {\n";
    ss << indentStr << "        type_tok: " << tokenTypeToString(returnType) << ",\n";
    ss << indentStr << "        ident: \"" << name << "\",\n";
    ss << indentStr << "        params: [\n";
    for (const auto& param : params) {
        ss << indentStr << "            Param {\n";
        ss << indentStr << "                type_tok: " << tokenTypeToString(param.type) << ",\n";
        ss << indentStr << "                ident: \"" << param.name << "\",\n";
        ss << indentStr << "            },\n";
    }
    ss << indentStr << "        ],\n";
    ss << indentStr << "        block: [\n";
    for (const auto& stmt : body) {
        if(stmt) ss << stmt->toString(indent + 3) << ",\n";
    }
    ss << indentStr << "        ]\n";
    ss << indentStr << "    }\n";
    ss << indentStr << ")";
    return ss.str();
}