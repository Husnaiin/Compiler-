#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "parser.h"
#include "scope_analyzer.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <set>

// Type Checking Error Types
enum class TypeChkError {
    ErroneousVarDecl,
    FnCallParamCount,
    FnCallParamType,
    ErroneousReturnType,
    ExpressionTypeMismatch,
    ExpectedBooleanExpression,
    ErroneousBreak,
    NonBooleanCondStmt,
    EmptyExpression,
    AttemptedBoolOpOnNonBools,
    AttemptedBitOpOnNonNumeric,
    AttemptedShiftOnNonInt,
    AttemptedAddOpOnNonNumeric,
    AttemptedExponentiationOfNonNumeric,
    ReturnStmtNotFound,
    ArrayIndexNotInteger,
    InvalidArraySize,
    PointerTypeMismatch,
    DereferenceNonPointer,
    AddressOfNonLValue,
    InvalidOperandTypes,
    UndefinedType,
    VoidVariableDeclaration,
    InvalidBreakContinue,
    SwitchExpressionNotIntegral,
    CaseValueNotConstant,
    DuplicateCaseValue,
    InvalidTernaryTypes,
    DivisionByZero,
    ModuloByZero,
    AssignmentToNonLValue,
    FunctionNotReturningValue
};

// Type Information
struct TypeInfo {
    std::string base_type;      // int, float, string, char, bool, void
    int pointer_depth;          // 0 for regular, 1 for *, 2 for **, etc.
    bool is_array;              // true if it's an array type
    int array_size;             // -1 if unknown/dynamic
    bool is_function;           // true if function type
    std::vector<TypeInfo> param_types;  // for function types
    
    TypeInfo() : base_type("void"), pointer_depth(0), is_array(false), 
                 array_size(-1), is_function(false) {}
    
    TypeInfo(const std::string& type) 
        : base_type(type), pointer_depth(0), is_array(false), 
          array_size(-1), is_function(false) {}
    
    bool isNumeric() const {
        return base_type == "int" || base_type == "float";
    }
    
    bool isIntegral() const {
        return base_type == "int" || base_type == "char" || base_type == "bool";
    }
    
    bool isBoolean() const {
        return base_type == "bool";
    }
    
    bool isVoid() const {
        return base_type == "void" && pointer_depth == 0 && !is_array;
    }
    
    bool isPointer() const {
        return pointer_depth > 0 || is_array;
    }
    
    bool equals(const TypeInfo& other) const {
        if (base_type != other.base_type) return false;
        if (pointer_depth != other.pointer_depth) return false;
        if (is_array != other.is_array) return false;
        if (is_function != other.is_function) return false;
        return true;
    }
    
    bool canImplicitlyCastTo(const TypeInfo& target) const {
        // Same type
        if (equals(target)) return true;
        
        // Pointer types must match exactly
        if (pointer_depth > 0 || target.pointer_depth > 0) {
            return equals(target);
        }
        
        // Array to pointer decay
        if (is_array && target.pointer_depth > 0) {
            return base_type == target.base_type;
        }
        
        // Numeric conversions
        if (isNumeric() && target.isNumeric()) {
            return true;  // Allow int <-> float
        }
        
        // Integral conversions
        if (isIntegral() && target.isIntegral()) {
            return true;  // Allow int <-> char <-> bool
        }
        
        return false;
    }
    
    std::string toString() const {
        std::string result = base_type;
        for (int i = 0; i < pointer_depth; i++) {
            result += "*";
        }
        if (is_array) {
            result += "[]";
        }
        return result;
    }
};

// Type Error Entry
struct TypeErrorEntry {
    TypeChkError error_type;
    std::string message;
    SourceLocation location;
    std::optional<std::string> related_info;
    
    TypeErrorEntry(TypeChkError type, const std::string& msg, const SourceLocation& loc,
                   const std::optional<std::string>& info = std::nullopt)
        : error_type(type), message(msg), location(loc), related_info(info) {}
};

// Type Checking Result
struct TypeCheckResult {
    bool has_errors;
    std::vector<TypeErrorEntry> errors;
    std::unordered_map<const ASTNode*, TypeInfo> type_map;  // Maps AST nodes to their types
    
    TypeCheckResult() : has_errors(false) {}
    
    bool hasErrors() const { return has_errors || !errors.empty(); }
};

// Context for tracking function and loop information
struct TypeCheckContext {
    std::string current_function;
    TypeInfo current_function_return_type;
    int loop_depth;  // Track nested loop depth for break/continue
    bool in_switch;  // Track if we're inside a switch statement
    std::set<int> switch_case_values;  // Track case values in current switch
    bool function_has_return;  // Track if function has return statement
    
    TypeCheckContext() : loop_depth(0), in_switch(false), function_has_return(false) {}
};

// Type Checker Class
class TypeChecker {
private:
    // Reference to scope analysis result
    ScopeAnalysisResult scope_result_;
    
    // Type checking state
    std::vector<TypeErrorEntry> errors_;
    std::unordered_map<const ASTNode*, TypeInfo> type_map_;
    TypeCheckContext context_;
    
    // Type environment (maps symbols to types)
    std::unordered_map<SymbolId, TypeInfo> symbol_types_;
    
    // Helper methods
    void report_error(TypeChkError type, const std::string& message,
                     const SourceLocation& loc, 
                     const std::optional<std::string>& info = std::nullopt);
    
    TypeInfo parse_type_string(const std::string& type_str);
    TypeInfo get_symbol_type(SymbolId symbol_id);
    TypeInfo get_expression_type(std::shared_ptr<ExpressionNode> expr);
    
    // Type checking visitor methods
    void check_program(std::shared_ptr<ProgramNode> program);
    void check_function(std::shared_ptr<FunctionNode> func);
    void check_statement(std::shared_ptr<StatementNode> stmt);
    TypeInfo check_expression(std::shared_ptr<ExpressionNode> expr);
    void check_block(std::shared_ptr<BlockNode> block);
    
    // Statement type checking
    void check_variable_declaration(std::shared_ptr<VariableDeclarationNode> decl);
    void check_array_declaration(std::shared_ptr<ArrayDeclarationNode> decl);
    void check_pointer_declaration(std::shared_ptr<PointerDeclarationNode> decl);
    void check_assignment(std::shared_ptr<AssignmentNode> assign);
    void check_if_statement(std::shared_ptr<IfStatementNode> stmt);
    void check_while_statement(std::shared_ptr<WhileStatementNode> stmt);
    void check_for_statement(std::shared_ptr<ForStatementNode> stmt);
    void check_do_while_statement(std::shared_ptr<DoWhileStatementNode> stmt);
    void check_switch_statement(std::shared_ptr<SwitchStatementNode> stmt);
    void check_return_statement(std::shared_ptr<ReturnStatementNode> stmt);
    void check_break_statement(std::shared_ptr<BreakStatementNode> stmt);
    void check_continue_statement(std::shared_ptr<ContinueStatementNode> stmt);
    void check_print_statement(std::shared_ptr<PrintStatementNode> stmt);
    
    // Expression type checking
    TypeInfo check_binary_op(std::shared_ptr<BinaryOpNode> expr);
    TypeInfo check_unary_op(std::shared_ptr<UnaryOpNode> expr);
    TypeInfo check_literal(std::shared_ptr<LiteralNode> expr);
    TypeInfo check_identifier(std::shared_ptr<IdentifierNode> expr);
    TypeInfo check_function_call(std::shared_ptr<FunctionCallNode> expr);
    TypeInfo check_ternary(std::shared_ptr<TernaryNode> expr);
    TypeInfo check_array_access(std::shared_ptr<ArrayAccessNode> expr);
    TypeInfo check_dereference(std::shared_ptr<DereferenceNode> expr);
    TypeInfo check_address_of(std::shared_ptr<AddressOfNode> expr);
    
    // Type compatibility checking
    bool are_types_compatible(const TypeInfo& t1, const TypeInfo& t2);
    TypeInfo get_common_type(const TypeInfo& t1, const TypeInfo& t2);
    bool is_valid_condition(const TypeInfo& type);
    
    // Operator-specific type checking
    bool is_valid_arithmetic_op(const std::string& op, const TypeInfo& left, const TypeInfo& right);
    bool is_valid_logical_op(const std::string& op, const TypeInfo& left, const TypeInfo& right);
    bool is_valid_bitwise_op(const std::string& op, const TypeInfo& left, const TypeInfo& right);
    bool is_valid_comparison_op(const std::string& op, const TypeInfo& left, const TypeInfo& right);
    bool is_valid_shift_op(const std::string& op, const TypeInfo& left, const TypeInfo& right);
    
    // Helper to get source location
    SourceLocation get_node_location(const ASTNode* node) const;
    
    // Helper to lookup symbols
    std::optional<SymbolInfo*> lookup_symbol(const std::string& name);
    
    // Build symbol type table from scope analysis
    void build_symbol_type_table();
    
    // Check if function has all required return statements
    bool check_all_paths_return(std::shared_ptr<BlockNode> block);
    
public:
    TypeChecker(const ScopeAnalysisResult& scope_result);
    
    // Main API
    TypeCheckResult check_types(std::shared_ptr<ProgramNode> ast);
    
    // Get type of AST node
    std::optional<TypeInfo> get_type(const ASTNode* node) const;
    
    // Debug
    void print_type_errors() const;
};

#endif // TYPE_CHECKER_H
