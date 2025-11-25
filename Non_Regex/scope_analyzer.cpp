#include "scope_analyzer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>

// Constructor
ScopeAnalyzer::ScopeAnalyzer() 
    : next_symbol_id_(1), next_scope_id_(1), loop_depth_(0), switch_depth_(0) {
    // Create global scope
    push_scope(ScopeKind::Global);
}

// Get current scope ID
ScopeId ScopeAnalyzer::current_scope_id() const {
    if (scope_stack_.empty()) {
        return 0;  // Should never happen after initialization
    }
    return scope_stack_.back();
}

// Get current scope pointer
Scope* ScopeAnalyzer::current_scope() {
    ScopeId id = current_scope_id();
    auto it = scopes_.find(id);
    return (it != scopes_.end()) ? &it->second : nullptr;
}

// Push a new scope
void ScopeAnalyzer::push_scope(ScopeKind kind, const SourceLocation& start) {
    std::optional<ScopeId> parent = scope_stack_.empty() ? std::nullopt : std::optional<ScopeId>(scope_stack_.back());
    ScopeId new_id = next_scope_id_++;
    
    Scope scope(new_id, kind, parent);
    scope.start_location = start;
    scopes_.insert({new_id, scope});
    scope_stack_.push_back(new_id);
}

// Pop current scope
void ScopeAnalyzer::pop_scope(const SourceLocation& end) {
    if (!scope_stack_.empty()) {
        ScopeId id = scope_stack_.back();
        auto it = scopes_.find(id);
        if (it != scopes_.end()) {
            it->second.end_location = end;
        }
        scope_stack_.pop_back();
    }
}

// Create a new symbol
SymbolId ScopeAnalyzer::create_symbol(const std::string& name, SymbolKind kind,
                                     const SourceLocation& loc, const std::string& type) {
    ScopeId scope_id = current_scope_id();
    SymbolId id = next_symbol_id_++;
    
    SymbolInfo info(id, name, kind, scope_id, loc, type);
    symbols_.insert({id, info});
    
    return id;
}

// Insert symbol into a scope
bool ScopeAnalyzer::insert_symbol(const std::string& name, SymbolId symbol_id, ScopeId scope_id) {
    auto it = scopes_.find(scope_id);
    if (it == scopes_.end()) {
        return false;
    }
    
    // Check for redefinition in same scope
    if (it->second.symbols.find(name) != it->second.symbols.end()) {
        return false;  // Already exists in this scope
    }
    
    it->second.symbols[name] = symbol_id;
    return true;
}

// Lookup symbol starting from current scope and walking up
std::optional<SymbolInfo*> ScopeAnalyzer::lookup(const std::string& name) {
    // Walk up the scope stack
    for (auto it = scope_stack_.rbegin(); it != scope_stack_.rend(); ++it) {
        ScopeId scope_id = *it;
        auto scope_it = scopes_.find(scope_id);
        if (scope_it == scopes_.end()) continue;
        
        auto symbol_it = scope_it->second.symbols.find(name);
        if (symbol_it != scope_it->second.symbols.end()) {
            SymbolId symbol_id = symbol_it->second;
            auto sym_it = symbols_.find(symbol_id);
            if (sym_it != symbols_.end()) {
                return &sym_it->second;
            }
        }
    }
    
    return std::nullopt;
}

// Lookup in a specific scope (and its parents)
std::optional<SymbolInfo*> ScopeAnalyzer::lookup_in_scope(const std::string& name, ScopeId scope_id) {
    ScopeId current = scope_id;
    while (current != 0) {
        auto scope_it = scopes_.find(current);
        if (scope_it == scopes_.end()) break;
        
        auto symbol_it = scope_it->second.symbols.find(name);
        if (symbol_it != scope_it->second.symbols.end()) {
            SymbolId symbol_id = symbol_it->second;
            auto sym_it = symbols_.find(symbol_id);
            if (sym_it != symbols_.end()) {
                return &sym_it->second;
            }
        }
        
        // Move to parent
        if (scope_it->second.parent_id.has_value()) {
            current = scope_it->second.parent_id.value();
        } else {
            break;
        }
    }
    
    return std::nullopt;
}

// Report an error
void ScopeAnalyzer::report_error(ScopeError type, const std::string& message,
                                const SourceLocation& loc, const std::optional<std::string>& symbol) {
    errors_.emplace_back(type, message, loc, symbol);
}

// Get approximate source location from AST node
// Since AST nodes don't store location directly, we'll use a default
SourceLocation ScopeAnalyzer::get_node_location(const ASTNode* node) const {
    // In a real implementation, you'd store location in AST nodes
    // For now, return a default location
    return SourceLocation(0, 0);
}

// Analyze program
void ScopeAnalyzer::analyze_program(std::shared_ptr<ProgramNode> program) {
    if (!program) return;
    
    // Analyze global variables first
    for (auto& global_var : program->globalVariables) {
        if (global_var) {
            analyze_variable_declaration(global_var);
        }
    }
    
    // First pass: register all function declarations (for forward reference support)
    ScopeId global_scope = scope_stack_[0];  // First scope is global
    for (auto& func : program->functions) {
        if (func) {
            SourceLocation loc = get_node_location(func.get());

            // Support simple overloading by arity: use a composite key "<name>#<param_count>"
            size_t arity = 0;
            for (auto& p : func->parameters) if (p) arity++;
            std::string func_key = func->name + "#" + std::to_string(arity);

            // Check if function with same signature already exists in global scope
            auto scope_it = scopes_.find(global_scope);
            if (scope_it != scopes_.end()) {
                if (scope_it->second.symbols.find(func_key) != scope_it->second.symbols.end()) {
                    report_error(ScopeError::FunctionPrototypeRedefinition,
                                "Function '" + func->name + "' with " + std::to_string(arity) + " parameter(s) already declared in this scope",
                                loc, func->name);
                    continue;  // Skip this function
                }
            }

            // Create new function symbol
            SymbolId func_id = create_symbol(func->name, SymbolKind::Function, loc, func->returnType);
            auto sym_it = symbols_.find(func_id);
            if (sym_it != symbols_.end()) {
                sym_it->second.is_defined = true;  // Mark as defined (has body)
                sym_it->second.decl_node = func.get();
            }
            
            // Insert into global scope under composite key
            if (!insert_symbol(func_key, func_id, global_scope)) {
                // Shouldn't happen if we checked above, but handle it
                report_error(ScopeError::FunctionPrototypeRedefinition,
                            "Function '" + func->name + "' redefinition for key " + func_key,
                            loc, func->name);
            }
        }
    }
    
    // Second pass: analyze function bodies
    for (auto& func : program->functions) {
        if (func) {
            analyze_function(func);
        }
    }
}

// Analyze function
void ScopeAnalyzer::analyze_function(std::shared_ptr<FunctionNode> func) {
    if (!func) return;
    
    SourceLocation func_loc = get_node_location(func.get());
    
    // Enter function scope
    push_scope(ScopeKind::Function, func_loc);
    
    // Analyze parameters (they're in function scope)
    for (auto& param : func->parameters) {
        if (param) {
            analyze_parameter(param, func_loc);
        }
    }
    
    // Analyze function body
    if (func->body) {
        analyze_block(func->body);
    }
    
    // Exit function scope
    pop_scope();
}

// Analyze parameter
void ScopeAnalyzer::analyze_parameter(std::shared_ptr<ParameterNode> param, const SourceLocation& loc) {
    if (!param) return;
    
    SourceLocation param_loc = loc;  // Use function location as approximation
    SymbolId param_id = create_symbol(param->name, SymbolKind::Parameter, param_loc, param->type);
    
    ScopeId func_scope = current_scope_id();
    if (!insert_symbol(param->name, param_id, func_scope)) {
        report_error(ScopeError::VariableRedefinition,
                    "Parameter '" + param->name + "' redefined",
                    param_loc, param->name);
    } else {
        auto sym_it = symbols_.find(param_id);
        if (sym_it != symbols_.end()) {
            sym_it->second.decl_node = param.get();
        }
    }
}

// Analyze statement
void ScopeAnalyzer::analyze_statement(std::shared_ptr<StatementNode> stmt) {
    if (!stmt) return;
    
    // Use dynamic_cast to determine statement type
    if (auto block = std::dynamic_pointer_cast<BlockNode>(stmt)) {
        analyze_block(block);
    } else if (auto var_decl = std::dynamic_pointer_cast<VariableDeclarationNode>(stmt)) {
        analyze_variable_declaration(var_decl);
    } else if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(stmt)) {
        analyze_assignment(assign);
    } else if (auto if_stmt = std::dynamic_pointer_cast<IfStatementNode>(stmt)) {
        if (if_stmt->condition) analyze_expression(if_stmt->condition);
        if (if_stmt->thenBlock) analyze_block(if_stmt->thenBlock);
        if (if_stmt->elseBlock) analyze_block(if_stmt->elseBlock);
    } else if (auto while_stmt = std::dynamic_pointer_cast<WhileStatementNode>(stmt)) {
        if (while_stmt->condition) analyze_expression(while_stmt->condition);
        loop_depth_++;  // Enter loop
        if (while_stmt->body) analyze_block(while_stmt->body);
        loop_depth_--;  // Exit loop
    } else if (auto for_stmt = std::dynamic_pointer_cast<ForStatementNode>(stmt)) {
        if (for_stmt->initialization) analyze_statement(for_stmt->initialization);
        if (for_stmt->condition) analyze_expression(for_stmt->condition);
        if (for_stmt->update) analyze_statement(for_stmt->update);
        loop_depth_++;  // Enter loop
        if (for_stmt->body) analyze_block(for_stmt->body);
        loop_depth_--;  // Exit loop
    } else if (auto do_while = std::dynamic_pointer_cast<DoWhileStatementNode>(stmt)) {
        loop_depth_++;  // Enter loop
        if (do_while->body) analyze_block(do_while->body);
        loop_depth_--;  // Exit loop
        if (do_while->condition) analyze_expression(do_while->condition);
    } else if (auto switch_stmt = std::dynamic_pointer_cast<SwitchStatementNode>(stmt)) {
        if (switch_stmt->expression) analyze_expression(switch_stmt->expression);
        switch_depth_++;  // Enter switch (allows break)
        for (auto& case_stmt : switch_stmt->cases) {
            if (case_stmt) {
                if (case_stmt->value) analyze_expression(case_stmt->value);
                for (auto& case_statement : case_stmt->statements) {
                    analyze_statement(case_statement);
                }
            }
        }
        if (switch_stmt->defaultCase) {
            for (auto& default_stmt : switch_stmt->defaultCase->statements) {
                analyze_statement(default_stmt);
            }
        }
        switch_depth_--;  // Exit switch
    } else if (auto break_stmt = std::dynamic_pointer_cast<BreakStatementNode>(stmt)) {
        // Validate break is inside a loop or switch
        if (loop_depth_ <= 0 && switch_depth_ <= 0) {
            SourceLocation loc = get_node_location(break_stmt.get());
            report_error(ScopeError::InvalidBreak,
                         "Break statement not within loop or switch",
                         loc, std::nullopt);
        }
    } else if (auto continue_stmt = std::dynamic_pointer_cast<ContinueStatementNode>(stmt)) {
        // Validate continue is inside a loop (not switch)
        if (loop_depth_ <= 0) {
            SourceLocation loc = get_node_location(continue_stmt.get());
            report_error(ScopeError::InvalidContinue,
                         "Continue statement not within loop",
                         loc, std::nullopt);
        }
    } else if (auto return_stmt = std::dynamic_pointer_cast<ReturnStatementNode>(stmt)) {
        if (return_stmt->expression) analyze_expression(return_stmt->expression);
    } else if (auto print_stmt = std::dynamic_pointer_cast<PrintStatementNode>(stmt)) {
        if (print_stmt->expression) analyze_expression(print_stmt->expression);
    }
}

// Analyze expression
void ScopeAnalyzer::analyze_expression(std::shared_ptr<ExpressionNode> expr) {
    if (!expr) return;
    
    if (auto ident = std::dynamic_pointer_cast<IdentifierNode>(expr)) {
        analyze_identifier(ident);
    } else if (auto call = std::dynamic_pointer_cast<FunctionCallNode>(expr)) {
        analyze_function_call(call);
    } else if (auto binary = std::dynamic_pointer_cast<BinaryOpNode>(expr)) {
        if (binary->left) analyze_expression(binary->left);
        if (binary->right) analyze_expression(binary->right);
    } else if (auto unary = std::dynamic_pointer_cast<UnaryOpNode>(expr)) {
        if (unary->operand) analyze_expression(unary->operand);
    } else if (auto ternary = std::dynamic_pointer_cast<TernaryNode>(expr)) {
        if (ternary->condition) analyze_expression(ternary->condition);
        if (ternary->trueValue) analyze_expression(ternary->trueValue);
        if (ternary->falseValue) analyze_expression(ternary->falseValue);
    } else if (auto array_access = std::dynamic_pointer_cast<ArrayAccessNode>(expr)) {
        // Array access: analyze the array name and index
        if (array_access->index) analyze_expression(array_access->index);
        // Note: arrayName is a string, not an IdentifierNode, so we'd need to handle it specially
        // For now, treat it as an identifier lookup
        SourceLocation loc = get_node_location(array_access.get());
        auto symbol = lookup(array_access->arrayName);
        if (!symbol.has_value()) {
            report_error(ScopeError::UndeclaredVariableAccessed,
                        "Undeclared variable '" + array_access->arrayName + "'",
                        loc, array_access->arrayName);
        } else {
            annotations_[array_access.get()] = ASTAnnotation(symbol.value()->id);
        }
    } else if (auto deref = std::dynamic_pointer_cast<DereferenceNode>(expr)) {
        if (deref->operand) analyze_expression(deref->operand);
    } else if (auto addr = std::dynamic_pointer_cast<AddressOfNode>(expr)) {
        if (addr->operand) analyze_expression(addr->operand);
    }
    // Literal nodes don't need analysis
}

// Analyze block
void ScopeAnalyzer::analyze_block(std::shared_ptr<BlockNode> block) {
    if (!block) return;
    
    SourceLocation block_loc = get_node_location(block.get());
    push_scope(ScopeKind::Block, block_loc);
    
    for (auto& stmt : block->statements) {
        if (stmt) {
            analyze_statement(stmt);
        }
    }
    
    pop_scope();
}

// Analyze variable declaration
void ScopeAnalyzer::analyze_variable_declaration(std::shared_ptr<VariableDeclarationNode> decl) {
    if (!decl) return;
    
    SourceLocation decl_loc = get_node_location(decl.get());
    ScopeId current = current_scope_id();
    
    // Determine symbol kind based on scope
    SymbolKind kind = (current == scope_stack_[0]) ? SymbolKind::GlobalVar : SymbolKind::LocalVar;
    
    // Check if already declared in current scope
    auto existing = lookup_in_scope(decl->name, current);
    if (existing.has_value()) {
        // Check if it's in the same scope
        if (existing.value()->scope_id == current) {
            report_error(ScopeError::VariableRedefinition,
                        "Variable '" + decl->name + "' already declared in this scope",
                        decl_loc, decl->name);
            return;
        }
    }
    
    // Create symbol
    SymbolId var_id = create_symbol(decl->name, kind, decl_loc, decl->type);
    auto sym_it = symbols_.find(var_id);
    if (sym_it != symbols_.end()) {
        sym_it->second.decl_node = decl.get();
    }
    
    // Insert into current scope
    if (!insert_symbol(decl->name, var_id, current)) {
        // This shouldn't happen if we checked above, but handle it
        report_error(ScopeError::VariableRedefinition,
                    "Variable '" + decl->name + "' redefinition",
                    decl_loc, decl->name);
    }
    
    // Analyze initializer expression if present
    if (decl->initializer) {
        analyze_expression(decl->initializer);
    }
}

// Analyze identifier
void ScopeAnalyzer::analyze_identifier(std::shared_ptr<IdentifierNode> ident) {
    if (!ident) return;
    
    SourceLocation ident_loc = get_node_location(ident.get());
    auto symbol = lookup(ident->name);
    
    if (!symbol.has_value()) {
        report_error(ScopeError::UndeclaredVariableAccessed,
                    "Undeclared variable '" + ident->name + "'",
                    ident_loc, ident->name);
        annotations_[ident.get()] = ASTAnnotation();  // Mark as unresolved
    } else {
        annotations_[ident.get()] = ASTAnnotation(symbol.value()->id);
    }
}

// Analyze function call
void ScopeAnalyzer::analyze_function_call(std::shared_ptr<FunctionCallNode> call) {
    if (!call) return;
    
    SourceLocation call_loc = get_node_location(call.get());
    // Resolve function by name + arity
    size_t arity = call->arguments.size();
    std::string func_key = call->functionName + "#" + std::to_string(arity);

    std::optional<SymbolInfo*> symbol;
    // Walk up the scope chain to find matching key
    for (auto it = scope_stack_.rbegin(); it != scope_stack_.rend(); ++it) {
        ScopeId scope_id = *it;
        auto scope_it = scopes_.find(scope_id);
        if (scope_it == scopes_.end()) continue;
        auto k = scope_it->second.symbols.find(func_key);
        if (k != scope_it->second.symbols.end()) {
            auto sym_it = symbols_.find(k->second);
            if (sym_it != symbols_.end()) {
                symbol = &sym_it->second;
                break;
            }
        }
    }

    if (!symbol.has_value()) {
        report_error(ScopeError::UndefinedFunctionCalled,
                     "Undefined function overload '" + call->functionName + "' with " + std::to_string(arity) + " argument(s)",
                     call_loc, call->functionName);
        annotations_[call.get()] = ASTAnnotation();  // Mark as unresolved
    } else {
        SymbolInfo* sym = symbol.value();
        if (sym->kind != SymbolKind::Function && sym->kind != SymbolKind::FunctionPrototype) {
            report_error(ScopeError::UndefinedFunctionCalled,
                        "'" + call->functionName + "' is not a function",
                        call_loc, call->functionName);
            annotations_[call.get()] = ASTAnnotation();
        } else {
            annotations_[call.get()] = ASTAnnotation(sym->id);
        }
    }
    
    // Analyze arguments
    for (auto& arg : call->arguments) {
        if (arg) {
            analyze_expression(arg);
        }
    }
}

// Analyze assignment
void ScopeAnalyzer::analyze_assignment(std::shared_ptr<AssignmentNode> assign) {
    if (!assign) return;
    
    SourceLocation assign_loc = get_node_location(assign.get());
    
    // Check if variable exists
    auto symbol = lookup(assign->variable);
    if (!symbol.has_value()) {
        report_error(ScopeError::UndeclaredVariableAccessed,
                    "Undeclared variable '" + assign->variable + "'",
                    assign_loc, assign->variable);
    } else {
        annotations_[assign.get()] = ASTAnnotation(symbol.value()->id);
    }
    
    // Analyze value expression
    if (assign->value) {
        analyze_expression(assign->value);
    }
}

// Main API: Analyze scopes
ScopeAnalysisResult ScopeAnalyzer::analyze_scopes(std::shared_ptr<ProgramNode> ast) {
    // Reset state (except keep global scope)
    scope_stack_.clear();
    scopes_.clear();
    symbols_.clear();
    errors_.clear();
    annotations_.clear();
    next_symbol_id_ = 1;
    next_scope_id_ = 1;
    loop_depth_ = 0;
    switch_depth_ = 0;
    
    // Recreate global scope
    push_scope(ScopeKind::Global);
    
    // Analyze the AST
    analyze_program(ast);
    
    // Build result
    ScopeAnalysisResult result;
    result.annotated_ast = ast;
    result.scopes = scopes_;
    result.symbols = symbols_;
    result.errors = errors_;
    
    return result;
}

// Lookup in environment
std::optional<SymbolInfo*> ScopeAnalyzer::lookup_in_env(const std::string& name, ScopeId scope_id) {
    return lookup_in_scope(name, scope_id);
}

// Resolve identifier
std::optional<SymbolInfo*> ScopeAnalyzer::resolve_ident(const std::string& name, const SourceLocation& loc) {
    return lookup(name);
}

// Get annotation
std::optional<ASTAnnotation> ScopeAnalyzer::get_annotation(const ASTNode* node) const {
    auto it = annotations_.find(node);
    if (it != annotations_.end()) {
        return it->second;
    }
    return std::nullopt;
}

// Debug: Dump scope tree
void ScopeAnalyzer::dump_scope_tree() const {
    std::cout << "=== SCOPE TREE ===" << std::endl;
    
    // Helper function to print scope recursively
    std::function<void(ScopeId, int)> print_scope = [&](ScopeId id, int indent) {
        auto it = scopes_.find(id);
        if (it == scopes_.end()) return;
        
        const Scope& scope = it->second;
        std::string indent_str(indent * 2, ' ');
        
        std::string kind_str;
        switch (scope.kind) {
            case ScopeKind::Global: kind_str = "Global"; break;
            case ScopeKind::Function: kind_str = "Function"; break;
            case ScopeKind::Block: kind_str = "Block"; break;
        }
        
        std::cout << indent_str << "Scope #" << id << " (" << kind_str << ")";
        if (scope.parent_id.has_value()) {
            std::cout << " [parent: #" << scope.parent_id.value() << "]";
        }
        std::cout << std::endl;
        
        if (!scope.symbols.empty()) {
            std::cout << indent_str << "  Symbols:" << std::endl;
            for (const auto& [name, symbol_id] : scope.symbols) {
                auto sym_it = symbols_.find(symbol_id);
                if (sym_it != symbols_.end()) {
                    const SymbolInfo& sym = sym_it->second;
                    std::string sym_kind;
                    switch (sym.kind) {
                        case SymbolKind::GlobalVar: sym_kind = "GlobalVar"; break;
                        case SymbolKind::LocalVar: sym_kind = "LocalVar"; break;
                        case SymbolKind::Parameter: sym_kind = "Parameter"; break;
                        case SymbolKind::Function: sym_kind = "Function"; break;
                        case SymbolKind::FunctionPrototype: sym_kind = "FunctionPrototype"; break;
                    }
                    std::cout << indent_str << "    " << name << " -> #" << symbol_id 
                              << " (" << sym_kind << ", type: " << sym.type_name << ")" << std::endl;
                }
            }
        }
        
        // Print child scopes (scopes with this as parent)
        for (const auto& [child_id, child_scope] : scopes_) {
            if (child_scope.parent_id.has_value() && child_scope.parent_id.value() == id) {
                print_scope(child_id, indent + 1);
            }
        }
    };
    
    // Find root scopes (no parent)
    for (const auto& [id, scope] : scopes_) {
        if (!scope.parent_id.has_value()) {
            print_scope(id, 0);
        }
    }
}

// Debug: Dump symbol table
void ScopeAnalyzer::dump_symbol_table() const {
    std::cout << "=== SYMBOL TABLE ===" << std::endl;
    
    for (const auto& [id, symbol] : symbols_) {
        std::string kind_str;
        switch (symbol.kind) {
            case SymbolKind::GlobalVar: kind_str = "GlobalVar"; break;
            case SymbolKind::LocalVar: kind_str = "LocalVar"; break;
            case SymbolKind::Parameter: kind_str = "Parameter"; break;
            case SymbolKind::Function: kind_str = "Function"; break;
            case SymbolKind::FunctionPrototype: kind_str = "FunctionPrototype"; break;
        }
        
        std::cout << "Symbol #" << id << ": " << symbol.name 
                  << " (" << kind_str << ")" << std::endl;
        std::cout << "  Scope: #" << symbol.scope_id << std::endl;
        std::cout << "  Type: " << symbol.type_name << std::endl;
        std::cout << "  Location: " << symbol.location.toString() << std::endl;
        std::cout << "  Defined: " << (symbol.is_defined ? "yes" : "no") << std::endl;
        std::cout << std::endl;
    }
}

// Debug: Print scope tree (alias)
void ScopeAnalyzer::print_scope_tree() const {
    dump_scope_tree();
}

