#ifndef AST_H
#define AST_H

#include <vector>
#include <memory>
#include <string>
#include <sstream> // <-- Add this include
#include "token.h"

// Helper function to convert TokenType to string
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::T_INT: return "Int";
        case TokenType::T_PLUS: return "AddOp";
        case TokenType::T_MINUS: return "SubOp";
        case TokenType::T_ASTERISK: return "MulOp";
        case TokenType::T_DIVIDE: return "DivOp";
        case TokenType::T_EQUAL: return "EqualsOp";
        case TokenType::T_LESS: return "LessThan";
        // ... add all other token types you want to print
        default: return std::to_string(static_cast<int>(type));
    }
}


// Forward declarations
struct Expr;
struct Stmt;
struct Decl;
struct VarDecl;
struct FnDecl;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;
using DeclPtr = std::unique_ptr<Decl>;
using Program = std::vector<DeclPtr>;

// Expression types
struct Expr {
    virtual ~Expr() = default;
    virtual std::string toString(int indent = 0) const = 0;
};

struct IntLit : Expr {
    int value;
    IntLit(int val);
    std::string toString(int indent = 0) const override;
};

struct FloatLit : Expr {
    double value;
    FloatLit(double val);
    std::string toString(int indent = 0) const override;
};

struct BoolLit : Expr {
    bool value;
    BoolLit(bool val);
    std::string toString(int indent = 0) const override;
};

struct StringLit : Expr {
    std::string value;
    StringLit(const std::string& val);
    std::string toString(int indent = 0) const override;
};

struct Ident : Expr {
    std::string name;
    Ident(const std::string& n);
    std::string toString(int indent = 0) const override;
};

struct BinaryExpr : Expr {
    TokenType op;
    ExprPtr lhs;
    ExprPtr rhs;
    BinaryExpr(TokenType op, ExprPtr l, ExprPtr r);
    std::string toString(int indent = 0) const override;
};

struct UnaryExpr : Expr {
    TokenType op;
    ExprPtr expr;
    UnaryExpr(TokenType op, ExprPtr e);
    std::string toString(int indent = 0) const override;
};

struct CallExpr : Expr {
    std::string callee;
    std::vector<ExprPtr> args;
    CallExpr(const std::string& c, std::vector<ExprPtr> a);
    std::string toString(int indent = 0) const override;
};

// Statement types
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::string toString(int indent = 0) const = 0;
};

struct ExprStmt : Stmt {
    ExprPtr expr;
    ExprStmt(ExprPtr e);
    std::string toString(int indent = 0) const override;
};

struct ReturnStmt : Stmt {
    ExprPtr expr;
    ReturnStmt(ExprPtr e);
    std::string toString(int indent = 0) const override;
};

struct IfStmt : Stmt {
    ExprPtr cond;
    std::vector<StmtPtr> thenBlock;
    std::vector<StmtPtr> elseBlock;
    IfStmt(ExprPtr c, std::vector<StmtPtr> t, std::vector<StmtPtr> e);
    std::string toString(int indent = 0) const override;
};

struct ForStmt : Stmt {
    std::unique_ptr<VarDecl> init;
    ExprPtr cond;
    ExprPtr update;
    std::vector<StmtPtr> body;
    ForStmt(std::unique_ptr<VarDecl> i, ExprPtr c, ExprPtr u, std::vector<StmtPtr> b);
    std::string toString(int indent = 0) const override;
};

struct BreakStmt : Stmt {
    std::string toString(int indent = 0) const override;
};

// Declaration types
struct Decl {
    virtual ~Decl() = default;
    virtual std::string toString(int indent = 0) const = 0;
};

struct VarDecl : Decl {
    TokenType type;
    std::string name;
    ExprPtr init;
    VarDecl(TokenType t, const std::string& n, ExprPtr i);
    std::string toString(int indent = 0) const override;
};

struct FnDecl : Decl {
    TokenType returnType;
    std::string name;
    std::vector<VarDecl> params;
    std::vector<StmtPtr> body;
    FnDecl(TokenType rt, const std::string& n, std::vector<VarDecl> p, std::vector<StmtPtr> b);
    std::string toString(int indent = 0) const override;
};

#endif // AST_H