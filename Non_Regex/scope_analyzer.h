#ifndef SCOPE_ANALYZER_H
#define SCOPE_ANALYZER_H

#include "parser.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <optional>

// Scope Analysis Error Types
enum class ScopeError {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionPrototypeRedefinition
};

// Source Location for error reporting
struct SourceLocation {
    int line;
    int column;
    
    SourceLocation(int l = 0, int c = 0) : line(l), column(c) {}
    std::string toString() const {
        return "line " + std::to_string(line) + ", column " + std::to_string(column);
    }
};

// Symbol Kind
enum class SymbolKind {
    GlobalVar,
    LocalVar,
    Parameter,
    Function,
    FunctionPrototype
};

// Scope Kind
enum class ScopeKind {
    Global,
    Function,
    Block
};

// Forward declarations
struct SymbolInfo;
struct Scope;
struct ScopeErrorEntry;

// Symbol ID type
using SymbolId = size_t;
using ScopeId = size_t;

// Symbol Information
struct SymbolInfo {
    SymbolId id;
    std::string name;
    SymbolKind kind;
    ScopeId scope_id;
    SourceLocation location;
    std::string type_name;  // Placeholder for type checker
    bool is_defined;        // For functions: whether full definition exists
    ASTNode* decl_node;     // Pointer to declaration node (for later phases)
    
    SymbolInfo(SymbolId id, const std::string& name, SymbolKind kind, 
               ScopeId scope_id, const SourceLocation& loc, const std::string& type = "")
        : id(id), name(name), kind(kind), scope_id(scope_id), 
          location(loc), type_name(type), is_defined(false), decl_node(nullptr) {}
};

// Scope Information
struct Scope {
    ScopeId id;
    std::optional<ScopeId> parent_id;
    ScopeKind kind;
    std::unordered_map<std::string, SymbolId> symbols;  // name -> symbol_id
    SourceLocation start_location;
    SourceLocation end_location;
    
    Scope(ScopeId id, ScopeKind kind, std::optional<ScopeId> parent = std::nullopt)
        : id(id), parent_id(parent), kind(kind) {}
};

// Scope Error Entry
struct ScopeErrorEntry {
    ScopeError error_type;
    std::string message;
    SourceLocation location;
    std::optional<std::string> related_symbol;
    
    ScopeErrorEntry(ScopeError type, const std::string& msg, const SourceLocation& loc,
                   const std::optional<std::string>& symbol = std::nullopt)
        : error_type(type), message(msg), location(loc), related_symbol(symbol) {}
};

// Scope Analysis Result
struct ScopeAnalysisResult {
    std::shared_ptr<ProgramNode> annotated_ast;
    std::unordered_map<ScopeId, Scope> scopes;
    std::unordered_map<SymbolId, SymbolInfo> symbols;
    std::vector<ScopeErrorEntry> errors;
    
    bool hasErrors() const { return !errors.empty(); }
};

// AST Node Annotation (stored separately to avoid modifying AST nodes)
struct ASTAnnotation {
    SymbolId symbol_id;
    bool is_resolved;
    
    ASTAnnotation() : symbol_id(0), is_resolved(false) {}
    ASTAnnotation(SymbolId id) : symbol_id(id), is_resolved(true) {}
};

// Scope Analyzer Class
class ScopeAnalyzer {
private:
    // Core data structures
    std::unordered_map<ScopeId, Scope> scopes_;
    std::unordered_map<SymbolId, SymbolInfo> symbols_;
    std::vector<ScopeId> scope_stack_;  // Current scope chain
    std::vector<ScopeErrorEntry> errors_;
    
    // Annotation map: maps AST node pointers to their resolved symbols
    std::unordered_map<const ASTNode*, ASTAnnotation> annotations_;
    
    // ID counters
    SymbolId next_symbol_id_;
    ScopeId next_scope_id_;
    
    // Helper methods
    ScopeId current_scope_id() const;
    Scope* current_scope();
    void push_scope(ScopeKind kind, const SourceLocation& start = SourceLocation());
    void pop_scope(const SourceLocation& end = SourceLocation());
    
    // Symbol management
    SymbolId create_symbol(const std::string& name, SymbolKind kind, 
                          const SourceLocation& loc, const std::string& type = "");
    bool insert_symbol(const std::string& name, SymbolId symbol_id, ScopeId scope_id);
    std::optional<SymbolInfo*> lookup(const std::string& name);
    std::optional<SymbolInfo*> lookup_in_scope(const std::string& name, ScopeId scope_id);
    
    // Error reporting
    void report_error(ScopeError type, const std::string& message, 
                     const SourceLocation& loc, const std::optional<std::string>& symbol = std::nullopt);
    
    // AST Visitor methods
    void analyze_program(std::shared_ptr<ProgramNode> program);
    void analyze_function(std::shared_ptr<FunctionNode> func);
    void analyze_parameter(std::shared_ptr<ParameterNode> param, const SourceLocation& loc);
    void analyze_statement(std::shared_ptr<StatementNode> stmt);
    void analyze_expression(std::shared_ptr<ExpressionNode> expr);
    void analyze_block(std::shared_ptr<BlockNode> block);
    void analyze_variable_declaration(std::shared_ptr<VariableDeclarationNode> decl);
    void analyze_identifier(std::shared_ptr<IdentifierNode> ident);
    void analyze_function_call(std::shared_ptr<FunctionCallNode> call);
    void analyze_assignment(std::shared_ptr<AssignmentNode> assign);
    
    // Helper to get source location from AST node (approximate)
    SourceLocation get_node_location(const ASTNode* node) const;
    
public:
    ScopeAnalyzer();
    
    // Main API
    ScopeAnalysisResult analyze_scopes(std::shared_ptr<ProgramNode> ast);
    
    // Lookup API for other modules
    std::optional<SymbolInfo*> lookup_in_env(const std::string& name, ScopeId scope_id);
    std::optional<SymbolInfo*> resolve_ident(const std::string& name, const SourceLocation& loc);
    
    // Debug/Visualization
    void dump_scope_tree() const;
    void dump_symbol_table() const;
    void print_scope_tree() const;
    
    // Get annotations
    std::optional<ASTAnnotation> get_annotation(const ASTNode* node) const;
};

#endif // SCOPE_ANALYZER_H


