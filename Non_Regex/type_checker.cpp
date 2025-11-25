#include "type_checker.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

// Constructor
TypeChecker::TypeChecker(const ScopeAnalysisResult& scope_result)
    : scope_result_(scope_result) {
    build_symbol_type_table();
}

// Report a type error
void TypeChecker::report_error(TypeChkError type, const std::string& message,
                              const SourceLocation& loc,
                              const std::optional<std::string>& info) {
    errors_.emplace_back(type, message, loc, info);
}

// Get source location (placeholder implementation)
SourceLocation TypeChecker::get_node_location(const ASTNode* node) const {
    return SourceLocation(0, 0);
}

// Parse type string into TypeInfo
TypeInfo TypeChecker::parse_type_string(const std::string& type_str) {
    TypeInfo info;
    std::string cleaned = type_str;
    
    // Count pointer depth
    info.pointer_depth = 0;
    while (!cleaned.empty() && cleaned.back() == '*') {
        info.pointer_depth++;
        cleaned.pop_back();
    }
    
    // Check for array
    info.is_array = false;
    size_t bracket_pos = cleaned.find('[');
    if (bracket_pos != std::string::npos) {
        info.is_array = true;
        // Try to extract array size
        size_t end_bracket = cleaned.find(']', bracket_pos);
        if (end_bracket != std::string::npos && end_bracket > bracket_pos + 1) {
            std::string size_str = cleaned.substr(bracket_pos + 1, end_bracket - bracket_pos - 1);
            try {
                info.array_size = std::stoi(size_str);
            } catch (...) {
                info.array_size = -1;  // Unknown size
            }
        }
        cleaned = cleaned.substr(0, bracket_pos);
    }
    
    // Remove whitespace
    cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), ::isspace), cleaned.end());
    
    info.base_type = cleaned;
    return info;
}

// Build symbol type table from scope analysis
void TypeChecker::build_symbol_type_table() {
    for (const auto& [symbol_id, symbol_info] : scope_result_.symbols) {
        TypeInfo type = parse_type_string(symbol_info.type_name);
        
        // For functions, we need to extract parameter types
        if (symbol_info.kind == SymbolKind::Function || 
            symbol_info.kind == SymbolKind::FunctionPrototype) {
            type.is_function = true;
            
            // Get function node if available
            if (symbol_info.decl_node) {
                auto func_node = dynamic_cast<FunctionNode*>(symbol_info.decl_node);
                if (func_node) {
                    for (const auto& param : func_node->parameters) {
                        if (param) {
                            TypeInfo param_type = parse_type_string(param->type);
                            type.param_types.push_back(param_type);
                        }
                    }
                }
            }
        }
        
        symbol_types_[symbol_id] = type;
    }
}

// Get symbol type
TypeInfo TypeChecker::get_symbol_type(SymbolId symbol_id) {
    auto it = symbol_types_.find(symbol_id);
    if (it != symbol_types_.end()) {
        return it->second;
    }
    return TypeInfo();  // Return void type as default
}

// Lookup symbol
std::optional<SymbolInfo*> TypeChecker::lookup_symbol(const std::string& name) {
    // Search through symbols in scope result
    for (auto& [symbol_id, symbol_info] : scope_result_.symbols) {
        if (symbol_info.name == name) {
            return &symbol_info;
        }
    }
    return std::nullopt;
}

// Check if types are compatible
bool TypeChecker::are_types_compatible(const TypeInfo& t1, const TypeInfo& t2) {
    return t1.equals(t2) || t1.canImplicitlyCastTo(t2) || t2.canImplicitlyCastTo(t1);
}

// Get common type between two types
TypeInfo TypeChecker::get_common_type(const TypeInfo& t1, const TypeInfo& t2) {
    if (t1.equals(t2)) return t1;
    
    // Pointer types must match exactly
    if (t1.pointer_depth > 0 || t2.pointer_depth > 0) {
        if (t1.equals(t2)) return t1;
        return TypeInfo();  // Invalid
    }
    
    // Numeric types: prefer float over int
    if (t1.isNumeric() && t2.isNumeric()) {
        if (t1.base_type == "float" || t2.base_type == "float") {
            return TypeInfo("float");
        }
        return TypeInfo("int");
    }
    
    // Integral types: prefer int
    if (t1.isIntegral() && t2.isIntegral()) {
        if (t1.base_type == "int" || t2.base_type == "int") {
            return TypeInfo("int");
        }
        return t1;
    }
    
    return TypeInfo();  // No common type
}

// Check if type is valid for condition
bool TypeChecker::is_valid_condition(const TypeInfo& type) {
    // Allow boolean, integral types, and pointers in conditions
    return type.isBoolean() || type.isIntegral() || type.isPointer();
}

// Check arithmetic operations
bool TypeChecker::is_valid_arithmetic_op(const std::string& op, 
                                         const TypeInfo& left, 
                                         const TypeInfo& right) {
    // Both operands must be numeric
    return left.isNumeric() && right.isNumeric();
}

// Check logical operations
bool TypeChecker::is_valid_logical_op(const std::string& op,
                                      const TypeInfo& left,
                                      const TypeInfo& right) {
    // Operands should be boolean or convertible to boolean
    return is_valid_condition(left) && is_valid_condition(right);
}

// Check bitwise operations
bool TypeChecker::is_valid_bitwise_op(const std::string& op,
                                      const TypeInfo& left,
                                      const TypeInfo& right) {
    // Both operands must be integral
    return left.isIntegral() && right.isIntegral();
}

// Check comparison operations
bool TypeChecker::is_valid_comparison_op(const std::string& op,
                                         const TypeInfo& left,
                                         const TypeInfo& right) {
    // Can compare same types or compatible numeric types
    if (are_types_compatible(left, right)) return true;
    if (left.isNumeric() && right.isNumeric()) return true;
    return false;
}

// Check shift operations
bool TypeChecker::is_valid_shift_op(const std::string& op,
                                    const TypeInfo& left,
                                    const TypeInfo& right) {
    // Both operands must be integers
    return (left.base_type == "int" && right.base_type == "int");
}

// Main type checking entry point
TypeCheckResult TypeChecker::check_types(std::shared_ptr<ProgramNode> ast) {
    errors_.clear();
    type_map_.clear();
    
    check_program(ast);
    
    TypeCheckResult result;
    result.has_errors = !errors_.empty();
    result.errors = errors_;
    result.type_map = type_map_;
    
    return result;
}

// Check program
void TypeChecker::check_program(std::shared_ptr<ProgramNode> program) {
    if (!program) return;
    
    // Check global variables
    for (auto& global_var : program->globalVariables) {
        if (global_var) {
            check_variable_declaration(global_var);
        }
    }
    
    // Check functions
    for (auto& func : program->functions) {
        if (func) {
            check_function(func);
        }
    }
}

// Check function
void TypeChecker::check_function(std::shared_ptr<FunctionNode> func) {
    if (!func) return;
    
    SourceLocation loc = get_node_location(func.get());
    
    // Set up context
    context_.current_function = func->name;
    context_.current_function_return_type = parse_type_string(func->returnType);
    context_.loop_depth = 0;
    context_.in_switch = false;
    context_.function_has_return = false;
    
    // Check parameters for void type (not allowed)
    for (const auto& param : func->parameters) {
        if (param) {
            TypeInfo param_type = parse_type_string(param->type);
            if (param_type.isVoid()) {
                report_error(TypeChkError::ErroneousVarDecl,
                           "Parameter '" + param->name + "' cannot have void type",
                           loc, param->name);
            }
        }
    }
    
    // Check function body
    if (func->body) {
        check_block(func->body);
    }
    
    // Check if non-void function has return statement
    if (!context_.current_function_return_type.isVoid() && !context_.function_has_return) {
        report_error(TypeChkError::ReturnStmtNotFound,
                   "Non-void function '" + func->name + "' does not have a return statement",
                   loc, func->name);
    }
    
    // Reset context
    context_ = TypeCheckContext();
}

// Check block
void TypeChecker::check_block(std::shared_ptr<BlockNode> block) {
    if (!block) return;
    
    for (auto& stmt : block->statements) {
        if (stmt) {
            check_statement(stmt);
        }
    }
}

// Check statement
void TypeChecker::check_statement(std::shared_ptr<StatementNode> stmt) {
    if (!stmt) return;
    
    if (auto var_decl = std::dynamic_pointer_cast<VariableDeclarationNode>(stmt)) {
        check_variable_declaration(var_decl);
    } else if (auto array_decl = std::dynamic_pointer_cast<ArrayDeclarationNode>(stmt)) {
        check_array_declaration(array_decl);
    } else if (auto ptr_decl = std::dynamic_pointer_cast<PointerDeclarationNode>(stmt)) {
        check_pointer_declaration(ptr_decl);
    } else if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(stmt)) {
        check_assignment(assign);
    } else if (auto if_stmt = std::dynamic_pointer_cast<IfStatementNode>(stmt)) {
        check_if_statement(if_stmt);
    } else if (auto while_stmt = std::dynamic_pointer_cast<WhileStatementNode>(stmt)) {
        check_while_statement(while_stmt);
    } else if (auto for_stmt = std::dynamic_pointer_cast<ForStatementNode>(stmt)) {
        check_for_statement(for_stmt);
    } else if (auto do_while = std::dynamic_pointer_cast<DoWhileStatementNode>(stmt)) {
        check_do_while_statement(do_while);
    } else if (auto switch_stmt = std::dynamic_pointer_cast<SwitchStatementNode>(stmt)) {
        check_switch_statement(switch_stmt);
    } else if (auto return_stmt = std::dynamic_pointer_cast<ReturnStatementNode>(stmt)) {
        check_return_statement(return_stmt);
    } else if (auto break_stmt = std::dynamic_pointer_cast<BreakStatementNode>(stmt)) {
        check_break_statement(break_stmt);
    } else if (auto continue_stmt = std::dynamic_pointer_cast<ContinueStatementNode>(stmt)) {
        check_continue_statement(continue_stmt);
    } else if (auto print_stmt = std::dynamic_pointer_cast<PrintStatementNode>(stmt)) {
        check_print_statement(print_stmt);
    } else if (auto block = std::dynamic_pointer_cast<BlockNode>(stmt)) {
        check_block(block);
    }
}

// Check variable declaration
void TypeChecker::check_variable_declaration(std::shared_ptr<VariableDeclarationNode> decl) {
    if (!decl) return;
    
    SourceLocation loc = get_node_location(decl.get());
    TypeInfo var_type = parse_type_string(decl->type);
    
    // Check for void variable
    if (var_type.isVoid()) {
        report_error(TypeChkError::VoidVariableDeclaration,
                   "Variable '" + decl->name + "' cannot have void type",
                   loc, decl->name);
        return;
    }
    
    // Check initializer if present
    if (decl->initializer) {
        TypeInfo init_type = check_expression(decl->initializer);
        
        if (!are_types_compatible(var_type, init_type)) {
            report_error(TypeChkError::ExpressionTypeMismatch,
                       "Cannot initialize variable '" + decl->name + "' of type '" + 
                       var_type.toString() + "' with expression of type '" + 
                       init_type.toString() + "'",
                       loc, decl->name);
        }
    }
}

// Check array declaration
void TypeChecker::check_array_declaration(std::shared_ptr<ArrayDeclarationNode> decl) {
    if (!decl) return;
    
    SourceLocation loc = get_node_location(decl.get());
    TypeInfo elem_type = parse_type_string(decl->type);
    
    // Check for void array
    if (elem_type.isVoid()) {
        report_error(TypeChkError::VoidVariableDeclaration,
                   "Array '" + decl->name + "' cannot have void element type",
                   loc, decl->name);
        return;
    }
    
    // Check array size if present
    if (decl->size) {
        TypeInfo size_type = check_expression(decl->size);
        if (!size_type.isIntegral()) {
            report_error(TypeChkError::ArrayIndexNotInteger,
                       "Array size must be an integer expression",
                       loc, decl->name);
        }
    }
    
    // Check initializer if present
    if (decl->initializer) {
        for (const auto& elem : decl->initializer->elements) {
            if (elem) {
                TypeInfo elem_init_type = check_expression(elem);
                if (!are_types_compatible(elem_type, elem_init_type)) {
                    report_error(TypeChkError::ExpressionTypeMismatch,
                               "Array element type mismatch: expected '" + 
                               elem_type.toString() + "' but got '" + 
                               elem_init_type.toString() + "'",
                               loc, decl->name);
                }
            }
        }
    }
}

// Check pointer declaration
void TypeChecker::check_pointer_declaration(std::shared_ptr<PointerDeclarationNode> decl) {
    if (!decl) return;
    
    SourceLocation loc = get_node_location(decl.get());
    TypeInfo base_type = parse_type_string(decl->baseType);
    base_type.pointer_depth = 1;  // It's a pointer
    
    // Check initializer if present
    if (decl->initializer) {
        TypeInfo init_type = check_expression(decl->initializer);
        
        // Allow address-of expression or compatible pointer type
        if (!are_types_compatible(base_type, init_type)) {
            report_error(TypeChkError::PointerTypeMismatch,
                       "Cannot initialize pointer '" + decl->name + "' of type '" + 
                       base_type.toString() + "' with expression of type '" + 
                       init_type.toString() + "'",
                       loc, decl->name);
        }
    }
}

// Check assignment
void TypeChecker::check_assignment(std::shared_ptr<AssignmentNode> assign) {
    if (!assign) return;
    
    SourceLocation loc = get_node_location(assign.get());
    
    // Lookup variable type
    auto symbol = lookup_symbol(assign->variable);
    if (!symbol.has_value()) {
        // Already reported by scope analyzer
        return;
    }
    
    TypeInfo var_type = get_symbol_type(symbol.value()->id);
    TypeInfo value_type = check_expression(assign->value);
    
    if (!are_types_compatible(var_type, value_type)) {
        report_error(TypeChkError::ExpressionTypeMismatch,
                   "Cannot assign expression of type '" + value_type.toString() + 
                   "' to variable '" + assign->variable + "' of type '" + 
                   var_type.toString() + "'",
                   loc, assign->variable);
    }
}

// Check if statement
void TypeChecker::check_if_statement(std::shared_ptr<IfStatementNode> stmt) {
    if (!stmt) return;
    
    SourceLocation loc = get_node_location(stmt.get());
    
    if (stmt->condition) {
        TypeInfo cond_type = check_expression(stmt->condition);
        if (!is_valid_condition(cond_type)) {
            report_error(TypeChkError::NonBooleanCondStmt,
                       "If statement condition must be boolean or convertible to boolean, got '" + 
                       cond_type.toString() + "'",
                       loc);
        }
    }
    
    if (stmt->thenBlock) {
        check_block(stmt->thenBlock);
    }
    
    if (stmt->elseBlock) {
        check_block(stmt->elseBlock);
    }
}

// Check while statement
void TypeChecker::check_while_statement(std::shared_ptr<WhileStatementNode> stmt) {
    if (!stmt) return;
    
    SourceLocation loc = get_node_location(stmt.get());
    
    if (stmt->condition) {
        TypeInfo cond_type = check_expression(stmt->condition);
        if (!is_valid_condition(cond_type)) {
            report_error(TypeChkError::NonBooleanCondStmt,
                       "While statement condition must be boolean or convertible to boolean, got '" + 
                       cond_type.toString() + "'",
                       loc);
        }
    }
    
    context_.loop_depth++;
    if (stmt->body) {
        check_block(stmt->body);
    }
    context_.loop_depth--;
}

// Check for statement
void TypeChecker::check_for_statement(std::shared_ptr<ForStatementNode> stmt) {
    if (!stmt) return;
    
    SourceLocation loc = get_node_location(stmt.get());
    
    if (stmt->initialization) {
        check_statement(stmt->initialization);
    }
    
    if (stmt->condition) {
        TypeInfo cond_type = check_expression(stmt->condition);
        if (!is_valid_condition(cond_type)) {
            report_error(TypeChkError::NonBooleanCondStmt,
                       "For statement condition must be boolean or convertible to boolean, got '" + 
                       cond_type.toString() + "'",
                       loc);
        }
    }
    
    if (stmt->update) {
        check_statement(stmt->update);
    }
    
    context_.loop_depth++;
    if (stmt->body) {
        check_block(stmt->body);
    }
    context_.loop_depth--;
}

// Check do-while statement
void TypeChecker::check_do_while_statement(std::shared_ptr<DoWhileStatementNode> stmt) {
    if (!stmt) return;
    
    SourceLocation loc = get_node_location(stmt.get());
    
    context_.loop_depth++;
    if (stmt->body) {
        check_block(stmt->body);
    }
    context_.loop_depth--;
    
    if (stmt->condition) {
        TypeInfo cond_type = check_expression(stmt->condition);
        if (!is_valid_condition(cond_type)) {
            report_error(TypeChkError::NonBooleanCondStmt,
                       "Do-while statement condition must be boolean or convertible to boolean, got '" + 
                       cond_type.toString() + "'",
                       loc);
        }
    }
}

// Check switch statement
void TypeChecker::check_switch_statement(std::shared_ptr<SwitchStatementNode> stmt) {
    if (!stmt) return;
    
    SourceLocation loc = get_node_location(stmt.get());
    
    // Check switch expression
    TypeInfo switch_type;
    if (stmt->expression) {
        switch_type = check_expression(stmt->expression);
        if (!switch_type.isIntegral()) {
            report_error(TypeChkError::SwitchExpressionNotIntegral,
                       "Switch expression must be of integral type, got '" + 
                       switch_type.toString() + "'",
                       loc);
        }
    }
    
    // Save old switch state
    bool old_in_switch = context_.in_switch;
    std::set<int> old_case_values = context_.switch_case_values;
    
    context_.in_switch = true;
    context_.switch_case_values.clear();
    context_.loop_depth++;  // Switch acts like a loop for break purposes
    
    // Check case statements
    for (auto& case_stmt : stmt->cases) {
        if (case_stmt && case_stmt->value) {
            TypeInfo case_type = check_expression(case_stmt->value);
            
            if (!are_types_compatible(switch_type, case_type)) {
                report_error(TypeChkError::ExpressionTypeMismatch,
                           "Case value type '" + case_type.toString() + 
                           "' does not match switch expression type '" + 
                           switch_type.toString() + "'",
                           loc);
            }
            
            // Check for constant value (simplified - just check if it's a literal)
            if (auto lit = std::dynamic_pointer_cast<LiteralNode>(case_stmt->value)) {
                try {
                    int case_val = std::stoi(lit->value);
                    if (context_.switch_case_values.count(case_val)) {
                        report_error(TypeChkError::DuplicateCaseValue,
                                   "Duplicate case value: " + std::to_string(case_val),
                                   loc);
                    }
                    context_.switch_case_values.insert(case_val);
                } catch (...) {
                    // Not a valid integer literal
                }
            }
            
            // Check case statements
            for (auto& case_statement : case_stmt->statements) {
                check_statement(case_statement);
            }
        }
    }
    
    // Check default case
    if (stmt->defaultCase) {
        for (auto& default_stmt : stmt->defaultCase->statements) {
            check_statement(default_stmt);
        }
    }
    
    context_.loop_depth--;
    context_.in_switch = old_in_switch;
    context_.switch_case_values = old_case_values;
}

// Check return statement
void TypeChecker::check_return_statement(std::shared_ptr<ReturnStatementNode> stmt) {
    if (!stmt) return;
    
    SourceLocation loc = get_node_location(stmt.get());
    context_.function_has_return = true;
    
    if (stmt->expression) {
        TypeInfo return_type = check_expression(stmt->expression);
        
        if (context_.current_function_return_type.isVoid()) {
            report_error(TypeChkError::ErroneousReturnType,
                       "Void function '" + context_.current_function + 
                       "' cannot return a value",
                       loc, context_.current_function);
        } else if (!are_types_compatible(context_.current_function_return_type, return_type)) {
            report_error(TypeChkError::ErroneousReturnType,
                       "Return type '" + return_type.toString() + 
                       "' does not match function return type '" + 
                       context_.current_function_return_type.toString() + "'",
                       loc, context_.current_function);
        }
    } else {
        if (!context_.current_function_return_type.isVoid()) {
            report_error(TypeChkError::ErroneousReturnType,
                       "Non-void function '" + context_.current_function + 
                       "' must return a value",
                       loc, context_.current_function);
        }
    }
}

// Check break statement
void TypeChecker::check_break_statement(std::shared_ptr<BreakStatementNode> stmt) {
    if (!stmt) return;
    
    SourceLocation loc = get_node_location(stmt.get());
    
    if (context_.loop_depth == 0 && !context_.in_switch) {
        report_error(TypeChkError::ErroneousBreak,
                   "Break statement outside of loop or switch",
                   loc);
    }
}

// Check continue statement
void TypeChecker::check_continue_statement(std::shared_ptr<ContinueStatementNode> stmt) {
    if (!stmt) return;
    
    SourceLocation loc = get_node_location(stmt.get());
    
    if (context_.loop_depth == 0) {
        report_error(TypeChkError::InvalidBreakContinue,
                   "Continue statement outside of loop",
                   loc);
    }
}

// Check print statement
void TypeChecker::check_print_statement(std::shared_ptr<PrintStatementNode> stmt) {
    if (!stmt) return;
    
    if (stmt->expression) {
        check_expression(stmt->expression);
    }
}

// Check expression and return its type
TypeInfo TypeChecker::check_expression(std::shared_ptr<ExpressionNode> expr) {
    if (!expr) {
        SourceLocation loc = get_node_location(expr.get());
        report_error(TypeChkError::EmptyExpression,
                   "Empty expression",
                   loc);
        return TypeInfo();
    }
    
    TypeInfo result;
    
    if (auto binary = std::dynamic_pointer_cast<BinaryOpNode>(expr)) {
        result = check_binary_op(binary);
    } else if (auto unary = std::dynamic_pointer_cast<UnaryOpNode>(expr)) {
        result = check_unary_op(unary);
    } else if (auto literal = std::dynamic_pointer_cast<LiteralNode>(expr)) {
        result = check_literal(literal);
    } else if (auto ident = std::dynamic_pointer_cast<IdentifierNode>(expr)) {
        result = check_identifier(ident);
    } else if (auto call = std::dynamic_pointer_cast<FunctionCallNode>(expr)) {
        result = check_function_call(call);
    } else if (auto ternary = std::dynamic_pointer_cast<TernaryNode>(expr)) {
        result = check_ternary(ternary);
    } else if (auto array_access = std::dynamic_pointer_cast<ArrayAccessNode>(expr)) {
        result = check_array_access(array_access);
    } else if (auto deref = std::dynamic_pointer_cast<DereferenceNode>(expr)) {
        result = check_dereference(deref);
    } else if (auto addr = std::dynamic_pointer_cast<AddressOfNode>(expr)) {
        result = check_address_of(addr);
    } else {
        result = TypeInfo();
    }
    
    // Store type in map
    type_map_[expr.get()] = result;
    return result;
}

// Check binary operation
TypeInfo TypeChecker::check_binary_op(std::shared_ptr<BinaryOpNode> expr) {
    if (!expr) return TypeInfo();
    
    SourceLocation loc = get_node_location(expr.get());
    TypeInfo left_type = check_expression(expr->left);
    TypeInfo right_type = check_expression(expr->right);
    
    std::string op = expr->op;
    
    // Arithmetic operators: +, -, *, /, %
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        if (op == "%" && (!left_type.isIntegral() || !right_type.isIntegral())) {
            report_error(TypeChkError::AttemptedAddOpOnNonNumeric,
                       "Modulo operator requires integral operands",
                       loc);
            return TypeInfo();
        }
        
        if (!is_valid_arithmetic_op(op, left_type, right_type)) {
            report_error(TypeChkError::AttemptedAddOpOnNonNumeric,
                       "Arithmetic operator '" + op + "' requires numeric operands, got '" + 
                       left_type.toString() + "' and '" + right_type.toString() + "'",
                       loc);
            return TypeInfo();
        }
        
        return get_common_type(left_type, right_type);
    }
    
    // Logical operators: &&, ||
    if (op == "&&" || op == "||") {
        if (!is_valid_condition(left_type) || !is_valid_condition(right_type)) {
            report_error(TypeChkError::AttemptedBoolOpOnNonBools,
                       "Logical operator '" + op + "' requires boolean-compatible operands, got '" + 
                       left_type.toString() + "' and '" + right_type.toString() + "'",
                       loc);
        }
        return TypeInfo("bool");
    }
    
    // Bitwise operators: &, |, ^
    if (op == "&" || op == "|" || op == "^") {
        if (!is_valid_bitwise_op(op, left_type, right_type)) {
            report_error(TypeChkError::AttemptedBitOpOnNonNumeric,
                       "Bitwise operator '" + op + "' requires integral operands, got '" + 
                       left_type.toString() + "' and '" + right_type.toString() + "'",
                       loc);
            return TypeInfo();
        }
        return get_common_type(left_type, right_type);
    }
    
    // Shift operators: <<, >>
    if (op == "<<" || op == ">>") {
        if (!is_valid_shift_op(op, left_type, right_type)) {
            report_error(TypeChkError::AttemptedShiftOnNonInt,
                       "Shift operator '" + op + "' requires integer operands, got '" + 
                       left_type.toString() + "' and '" + right_type.toString() + "'",
                       loc);
            return TypeInfo();
        }
        return TypeInfo("int");
    }
    
    // Comparison operators: ==, !=, <, >, <=, >=
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
        if (!is_valid_comparison_op(op, left_type, right_type)) {
            report_error(TypeChkError::ExpressionTypeMismatch,
                       "Comparison operator '" + op + "' operands type mismatch: '" + 
                       left_type.toString() + "' and '" + right_type.toString() + "'",
                       loc);
        }
        return TypeInfo("bool");
    }
    
    // Assignment operators (if parsed as binary ops)
    if (op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "%=") {
        if (!are_types_compatible(left_type, right_type)) {
            report_error(TypeChkError::ExpressionTypeMismatch,
                       "Assignment operator '" + op + "' type mismatch: '" + 
                       left_type.toString() + "' and '" + right_type.toString() + "'",
                       loc);
        }
        return left_type;
    }
    
    return TypeInfo();
}

// Check unary operation
TypeInfo TypeChecker::check_unary_op(std::shared_ptr<UnaryOpNode> expr) {
    if (!expr) return TypeInfo();
    
    SourceLocation loc = get_node_location(expr.get());
    TypeInfo operand_type = check_expression(expr->operand);
    
    std::string op = expr->op;
    
    // Logical NOT: !
    if (op == "!") {
        if (!is_valid_condition(operand_type)) {
            report_error(TypeChkError::AttemptedBoolOpOnNonBools,
                       "Logical NOT operator requires boolean-compatible operand, got '" + 
                       operand_type.toString() + "'",
                       loc);
        }
        return TypeInfo("bool");
    }
    
    // Bitwise NOT: ~
    if (op == "~") {
        if (!operand_type.isIntegral()) {
            report_error(TypeChkError::AttemptedBitOpOnNonNumeric,
                       "Bitwise NOT operator requires integral operand, got '" + 
                       operand_type.toString() + "'",
                       loc);
            return TypeInfo();
        }
        return operand_type;
    }
    
    // Unary minus/plus: -, +
    if (op == "-" || op == "+") {
        if (!operand_type.isNumeric()) {
            report_error(TypeChkError::AttemptedAddOpOnNonNumeric,
                       "Unary '" + op + "' operator requires numeric operand, got '" + 
                       operand_type.toString() + "'",
                       loc);
            return TypeInfo();
        }
        return operand_type;
    }
    
    // Increment/Decrement: ++, --
    if (op == "++" || op == "--") {
        if (!operand_type.isNumeric() && !operand_type.isPointer()) {
            report_error(TypeChkError::InvalidOperandTypes,
                       "Increment/decrement operator requires numeric or pointer operand, got '" + 
                       operand_type.toString() + "'",
                       loc);
            return TypeInfo();
        }
        return operand_type;
    }
    
    return TypeInfo();
}

// Check literal
TypeInfo TypeChecker::check_literal(std::shared_ptr<LiteralNode> expr) {
    if (!expr) return TypeInfo();
    
    return parse_type_string(expr->type);
}

// Check identifier
TypeInfo TypeChecker::check_identifier(std::shared_ptr<IdentifierNode> expr) {
    if (!expr) return TypeInfo();
    
    SourceLocation loc = get_node_location(expr.get());
    auto symbol = lookup_symbol(expr->name);
    
    if (!symbol.has_value()) {
        // Already reported by scope analyzer
        return TypeInfo();
    }
    
    return get_symbol_type(symbol.value()->id);
}

// Check function call
TypeInfo TypeChecker::check_function_call(std::shared_ptr<FunctionCallNode> expr) {
    if (!expr) return TypeInfo();
    
    SourceLocation loc = get_node_location(expr.get());
    
    // Resolve overloaded function by name + arity first, then by parameter types
    std::vector<SymbolInfo*> candidates;
    size_t arity = expr->arguments.size();
    for (auto& [symbol_id, symbol_info] : scope_result_.symbols) {
        if ((symbol_info.kind == SymbolKind::Function || symbol_info.kind == SymbolKind::FunctionPrototype) &&
            symbol_info.name == expr->functionName) {
            // Count parameters from decl_node if available
            size_t param_count = 0;
            if (symbol_info.decl_node) {
                if (auto fn = dynamic_cast<FunctionNode*>(symbol_info.decl_node)) {
                    for (auto& p : fn->parameters) if (p) param_count++;
                }
            }
            if (param_count == arity) {
                candidates.push_back(const_cast<SymbolInfo*>(&symbol_info));
            }
        }
    }
    
    if (candidates.empty()) {
        // No overload with matching arity; surface param count error
        // Try to find any function with this name to get expected count (first one)
        for (auto& [symbol_id, symbol_info] : scope_result_.symbols) {
            if ((symbol_info.kind == SymbolKind::Function || symbol_info.kind == SymbolKind::FunctionPrototype) &&
                symbol_info.name == expr->functionName) {
                TypeInfo any_func_type = get_symbol_type(symbol_id);
                report_error(TypeChkError::FnCallParamCount,
                           "Function '" + expr->functionName + "' expects " + 
                           std::to_string(any_func_type.param_types.size()) + " arguments, got " + 
                           std::to_string(arity),
                           loc, expr->functionName);
                return any_func_type;
            }
        }
        // Function not found at all (already reported by scope analyzer)
        return TypeInfo();
    }
    
    // Pick first candidate then validate parameter types
    SymbolInfo* chosen = candidates.front();
    TypeInfo func_type = get_symbol_type(chosen->id);
    
    if (!func_type.is_function) {
        report_error(TypeChkError::FnCallParamCount,
                   "'" + expr->functionName + "' is not a function",
                   loc, expr->functionName);
        return TypeInfo();
    }
    
    // Parameter count already matched by selection
    
    // Check parameter types
    for (size_t i = 0; i < expr->arguments.size(); i++) {
        TypeInfo arg_type = check_expression(expr->arguments[i]);
        TypeInfo param_type = func_type.param_types[i];
        
        if (!are_types_compatible(param_type, arg_type)) {
            report_error(TypeChkError::FnCallParamType,
                       "Argument " + std::to_string(i + 1) + " of function '" + 
                       expr->functionName + "' has wrong type: expected '" + 
                       param_type.toString() + "', got '" + arg_type.toString() + "'",
                       loc, expr->functionName);
        }
    }
    
    return func_type;  // Return function return type
}

// Check ternary operation
TypeInfo TypeChecker::check_ternary(std::shared_ptr<TernaryNode> expr) {
    if (!expr) return TypeInfo();
    
    SourceLocation loc = get_node_location(expr.get());
    
    TypeInfo cond_type = check_expression(expr->condition);
    if (!is_valid_condition(cond_type)) {
        report_error(TypeChkError::ExpectedBooleanExpression,
                   "Ternary operator condition must be boolean-compatible, got '" + 
                   cond_type.toString() + "'",
                   loc);
    }
    
    TypeInfo true_type = check_expression(expr->trueValue);
    TypeInfo false_type = check_expression(expr->falseValue);
    
    if (!are_types_compatible(true_type, false_type)) {
        report_error(TypeChkError::InvalidTernaryTypes,
                   "Ternary operator branches have incompatible types: '" + 
                   true_type.toString() + "' and '" + false_type.toString() + "'",
                   loc);
        return true_type;
    }
    
    return get_common_type(true_type, false_type);
}

// Check array access
TypeInfo TypeChecker::check_array_access(std::shared_ptr<ArrayAccessNode> expr) {
    if (!expr) return TypeInfo();
    
    SourceLocation loc = get_node_location(expr.get());
    
    // Check array variable
    auto symbol = lookup_symbol(expr->arrayName);
    if (!symbol.has_value()) {
        // Already reported by scope analyzer
        return TypeInfo();
    }
    
    TypeInfo array_type = get_symbol_type(symbol.value()->id);
    
    if (!array_type.is_array && array_type.pointer_depth == 0) {
        report_error(TypeChkError::InvalidOperandTypes,
                   "Subscript operator applied to non-array/non-pointer type '" + 
                   array_type.toString() + "'",
                   loc, expr->arrayName);
        return TypeInfo();
    }
    
    // Check index type
    TypeInfo index_type = check_expression(expr->index);
    if (!index_type.isIntegral()) {
        report_error(TypeChkError::ArrayIndexNotInteger,
                   "Array index must be of integral type, got '" + 
                   index_type.toString() + "'",
                   loc, expr->arrayName);
    }
    
    // Return element type (dereference pointer or array)
    TypeInfo elem_type = array_type;
    if (elem_type.is_array) {
        elem_type.is_array = false;
    } else if (elem_type.pointer_depth > 0) {
        elem_type.pointer_depth--;
    }
    
    return elem_type;
}

// Check dereference
TypeInfo TypeChecker::check_dereference(std::shared_ptr<DereferenceNode> expr) {
    if (!expr) return TypeInfo();
    
    SourceLocation loc = get_node_location(expr.get());
    TypeInfo operand_type = check_expression(expr->operand);
    
    if (!operand_type.isPointer()) {
        report_error(TypeChkError::DereferenceNonPointer,
                   "Cannot dereference non-pointer type '" + 
                   operand_type.toString() + "'",
                   loc);
        return TypeInfo();
    }
    
    // Return dereferenced type
    TypeInfo result = operand_type;
    if (result.is_array) {
        result.is_array = false;
    } else if (result.pointer_depth > 0) {
        result.pointer_depth--;
    }
    
    return result;
}

// Check address-of
TypeInfo TypeChecker::check_address_of(std::shared_ptr<AddressOfNode> expr) {
    if (!expr) return TypeInfo();
    
    SourceLocation loc = get_node_location(expr.get());
    TypeInfo operand_type = check_expression(expr->operand);
    
    // Check if operand is an lvalue (simplified check - just verify it's an identifier or array access)
    if (!std::dynamic_pointer_cast<IdentifierNode>(expr->operand) &&
        !std::dynamic_pointer_cast<ArrayAccessNode>(expr->operand)) {
        report_error(TypeChkError::AddressOfNonLValue,
                   "Address-of operator requires an lvalue",
                   loc);
    }
    
    // Return pointer to operand type
    TypeInfo result = operand_type;
    result.pointer_depth++;
    
    return result;
}

// Get type of AST node
std::optional<TypeInfo> TypeChecker::get_type(const ASTNode* node) const {
    auto it = type_map_.find(node);
    if (it != type_map_.end()) {
        return it->second;
    }
    return std::nullopt;
}

// Print type errors
void TypeChecker::print_type_errors() const {
    if (errors_.empty()) {
        std::cout << "=== NO TYPE ERRORS ===" << std::endl;
        return;
    }
    
    std::cout << "=== TYPE CHECKING ERRORS ===" << std::endl;
    for (const auto& error : errors_) {
        std::string error_type;
        switch (error.error_type) {
            case TypeChkError::ErroneousVarDecl: error_type = "ErroneousVarDecl"; break;
            case TypeChkError::FnCallParamCount: error_type = "FnCallParamCount"; break;
            case TypeChkError::FnCallParamType: error_type = "FnCallParamType"; break;
            case TypeChkError::ErroneousReturnType: error_type = "ErroneousReturnType"; break;
            case TypeChkError::ExpressionTypeMismatch: error_type = "ExpressionTypeMismatch"; break;
            case TypeChkError::ExpectedBooleanExpression: error_type = "ExpectedBooleanExpression"; break;
            case TypeChkError::ErroneousBreak: error_type = "ErroneousBreak"; break;
            case TypeChkError::NonBooleanCondStmt: error_type = "NonBooleanCondStmt"; break;
            case TypeChkError::EmptyExpression: error_type = "EmptyExpression"; break;
            case TypeChkError::AttemptedBoolOpOnNonBools: error_type = "AttemptedBoolOpOnNonBools"; break;
            case TypeChkError::AttemptedBitOpOnNonNumeric: error_type = "AttemptedBitOpOnNonNumeric"; break;
            case TypeChkError::AttemptedShiftOnNonInt: error_type = "AttemptedShiftOnNonInt"; break;
            case TypeChkError::AttemptedAddOpOnNonNumeric: error_type = "AttemptedAddOpOnNonNumeric"; break;
            case TypeChkError::AttemptedExponentiationOfNonNumeric: error_type = "AttemptedExponentiationOfNonNumeric"; break;
            case TypeChkError::ReturnStmtNotFound: error_type = "ReturnStmtNotFound"; break;
            case TypeChkError::ArrayIndexNotInteger: error_type = "ArrayIndexNotInteger"; break;
            case TypeChkError::InvalidArraySize: error_type = "InvalidArraySize"; break;
            case TypeChkError::PointerTypeMismatch: error_type = "PointerTypeMismatch"; break;
            case TypeChkError::DereferenceNonPointer: error_type = "DereferenceNonPointer"; break;
            case TypeChkError::AddressOfNonLValue: error_type = "AddressOfNonLValue"; break;
            case TypeChkError::InvalidOperandTypes: error_type = "InvalidOperandTypes"; break;
            case TypeChkError::UndefinedType: error_type = "UndefinedType"; break;
            case TypeChkError::VoidVariableDeclaration: error_type = "VoidVariableDeclaration"; break;
            case TypeChkError::InvalidBreakContinue: error_type = "InvalidBreakContinue"; break;
            case TypeChkError::SwitchExpressionNotIntegral: error_type = "SwitchExpressionNotIntegral"; break;
            case TypeChkError::CaseValueNotConstant: error_type = "CaseValueNotConstant"; break;
            case TypeChkError::DuplicateCaseValue: error_type = "DuplicateCaseValue"; break;
            case TypeChkError::InvalidTernaryTypes: error_type = "InvalidTernaryTypes"; break;
            case TypeChkError::DivisionByZero: error_type = "DivisionByZero"; break;
            case TypeChkError::ModuloByZero: error_type = "ModuloByZero"; break;
            case TypeChkError::AssignmentToNonLValue: error_type = "AssignmentToNonLValue"; break;
            case TypeChkError::FunctionNotReturningValue: error_type = "FunctionNotReturningValue"; break;
        }
        
        std::cout << "[" << error_type << "] " << error.message 
                  << " at " << error.location.toString();
        if (error.related_info.has_value()) {
            std::cout << " (" << error.related_info.value() << ")";
        }
        std::cout << std::endl;
    }
}
