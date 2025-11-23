#include "ir_generator.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// ===== TACOperand Implementation =====

std::string TACOperand::toString() const {
    switch (kind) {
        case Kind::TEMP:
        case Kind::VAR:
        case Kind::FUNC:
            return value;
        case Kind::CONST:
            return value;
        case Kind::LABEL:
            return value;
        default:
            return "?";
    }
}

// ===== TACInstruction Implementation =====

std::string TACInstruction::toString() const {
    std::ostringstream oss;
    
    switch (opcode) {
        case TACOpcode::LABEL:
            oss << dest.toString() << ":";
            break;
            
        case TACOpcode::GOTO:
            oss << "goto " << dest.toString();
            break;
            
        case TACOpcode::IF_FALSE:
            oss << "if (!" << src1.toString() << ") goto " << dest.toString();
            break;
            
        case TACOpcode::IF_TRUE:
            oss << "if (" << src1.toString() << ") goto " << dest.toString();
            break;
            
        case TACOpcode::RETURN:
            oss << "return " << src1.toString();
            break;
            
        case TACOpcode::RETURN_VOID:
            oss << "return";
            break;
            
        case TACOpcode::PARAM:
            oss << "param " << dest.toString();
            break;
            
        case TACOpcode::CALL:
            oss << dest.toString() << " = call " << src1.toString() << ", " << src2.toString();
            break;
            
        case TACOpcode::COPY:
            oss << dest.toString() << " = " << src1.toString();
            break;
            
        case TACOpcode::PRINT:
            oss << "print " << dest.toString();
            break;
            
        case TACOpcode::NEG:
        case TACOpcode::NOT:
        case TACOpcode::LNOT:
            {
                std::string op_str;
                if (opcode == TACOpcode::NEG) op_str = "-";
                else if (opcode == TACOpcode::NOT) op_str = "~";
                else op_str = "!";
                oss << dest.toString() << " = " << op_str << src1.toString();
            }
            break;
            
        case TACOpcode::ADDR:
            oss << dest.toString() << " = &" << src1.toString();
            break;
            
        case TACOpcode::DEREF:
            oss << dest.toString() << " = *" << src1.toString();
            break;
            
        case TACOpcode::STORE:
            oss << "*" << dest.toString() << " = " << src1.toString();
            break;
            
        case TACOpcode::ARRAY_LOAD:
            oss << dest.toString() << " = " << src1.toString() << "[" << src2.toString() << "]";
            break;
            
        case TACOpcode::ARRAY_STORE:
            oss << dest.toString() << "[" << src1.toString() << "] = " << src2.toString();
            break;
            
        case TACOpcode::ARRAY_ADDR:
            oss << dest.toString() << " = &" << src1.toString() << "[" << src2.toString() << "]";
            break;
            
        case TACOpcode::NOP:
            oss << "nop";
            break;
            
        default:
            // Binary operations
            {
                std::string op_str;
                switch (opcode) {
                    case TACOpcode::ADD: op_str = "+"; break;
                    case TACOpcode::SUB: op_str = "-"; break;
                    case TACOpcode::MUL: op_str = "*"; break;
                    case TACOpcode::DIV: op_str = "/"; break;
                    case TACOpcode::MOD: op_str = "%"; break;
                    case TACOpcode::AND: op_str = "&"; break;
                    case TACOpcode::OR: op_str = "|"; break;
                    case TACOpcode::XOR: op_str = "^"; break;
                    case TACOpcode::SHL: op_str = "<<"; break;
                    case TACOpcode::SHR: op_str = ">>"; break;
                    case TACOpcode::LAND: op_str = "&&"; break;
                    case TACOpcode::LOR: op_str = "||"; break;
                    case TACOpcode::EQ: op_str = "=="; break;
                    case TACOpcode::NE: op_str = "!="; break;
                    case TACOpcode::LT: op_str = "<"; break;
                    case TACOpcode::LE: op_str = "<="; break;
                    case TACOpcode::GT: op_str = ">"; break;
                    case TACOpcode::GE: op_str = ">="; break;
                    default: op_str = "?"; break;
                }
                oss << dest.toString() << " = " << src1.toString() << " " << op_str << " " << src2.toString();
            }
            break;
    }
    
    if (!comment.empty()) {
        oss << "  # " << comment;
    }
    
    return oss.str();
}

// ===== IRProgram Implementation =====

std::string IRProgram::toString() const {
    std::ostringstream oss;
    
    // Global variables
    if (!global_vars.empty()) {
        oss << "# Global Variables\n";
        for (const auto& [name, type] : global_vars) {
            oss << ".global " << type << " " << name << "\n";
        }
        oss << "\n";
    }
    
    // String literals
    if (!string_literals.empty()) {
        oss << "# String Literals\n";
        for (const auto& [label, value] : string_literals) {
            oss << label << ": .string " << value << "\n";
        }
        oss << "\n";
    }
    
    // Functions
    for (const auto& func : functions) {
        oss << "# Function: " << func.name << " (returns " << func.return_type << ")\n";
        oss << ".function " << func.name << "\n";
        
        // Parameters
        if (!func.parameters.empty()) {
            oss << "# Parameters: ";
            for (size_t i = 0; i < func.parameters.size(); i++) {
                if (i > 0) oss << ", ";
                oss << func.parameters[i];
            }
            oss << "\n";
        }
        
        // Instructions
        for (const auto& instr : func.instructions) {
            if (instr.opcode == TACOpcode::LABEL) {
                oss << instr.toString() << "\n";
            } else {
                oss << "    " << instr.toString() << "\n";
            }
        }
        
        oss << ".end_function\n\n";
    }
    
    return oss.str();
}

void IRProgram::print() const {
    std::cout << toString();
}

// ===== IRGenerator Implementation =====

IRGenerator::IRGenerator() 
    : current_function_(nullptr), string_literal_count_(0) {
}

// Helper: Generate new temporary
std::string IRGenerator::new_temp() {
    if (!current_function_) {
        report_error(IRGenError::InvalidExpression, "No current function context");
        return "t0";
    }
    return "t" + std::to_string(current_function_->temp_count++);
}

// Helper: Generate new label
std::string IRGenerator::new_label() {
    if (!current_function_) {
        report_error(IRGenError::InvalidExpression, "No current function context");
        return "L0";
    }
    return "L" + std::to_string(current_function_->label_count++);
}

// Helper: Generate string literal label
std::string IRGenerator::new_string_label() {
    return ".STR" + std::to_string(string_literal_count_++);
}

// Emit instructions
void IRGenerator::emit(const TACInstruction& instr) {
    if (current_function_) {
        current_function_->instructions.push_back(instr);
    }
}

void IRGenerator::emit(TACOpcode opcode) {
    emit(TACInstruction(opcode));
}

void IRGenerator::emit(TACOpcode opcode, const TACOperand& dest) {
    emit(TACInstruction(opcode, dest));
}

void IRGenerator::emit(TACOpcode opcode, const TACOperand& dest, const TACOperand& src1) {
    emit(TACInstruction(opcode, dest, src1));
}

void IRGenerator::emit(TACOpcode opcode, const TACOperand& dest, 
                      const TACOperand& src1, const TACOperand& src2) {
    emit(TACInstruction(opcode, dest, src1, src2));
}

// Error reporting
void IRGenerator::report_error(IRGenError type, const std::string& message,
                              const std::optional<std::string>& symbol) {
    errors_.emplace_back(type, message, symbol);
}

// Get variable type
std::string IRGenerator::get_variable_type(const std::string& name) {
    if (type_result_) {
        // Try to get type from type checker result
        // This is a simplified version - in a real implementation,
        // you'd look up the variable in the type environment
    }
    return "int";  // Default type
}

// Get expression type
std::string IRGenerator::get_expression_type(std::shared_ptr<ExpressionNode> expr) {
    if (!expr) return "void";
    
    if (type_result_) {
        // Try to get type from type checker result
        auto it = type_result_->type_map.find(expr.get());
        if (it != type_result_->type_map.end()) {
            return it->second.base_type;
        }
    }
    
    // Fallback: try to infer from literal
    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr)) {
        return lit->type;
    }
    
    return "int";  // Default type
}

// Loop context management
void IRGenerator::push_loop(const std::string& break_label, const std::string& continue_label) {
    loop_stack_.push_back({break_label, continue_label});
}

void IRGenerator::pop_loop() {
    if (!loop_stack_.empty()) {
        loop_stack_.pop_back();
    }
}

std::optional<IRGenerator::LoopContext> IRGenerator::current_loop() {
    if (loop_stack_.empty()) {
        return std::nullopt;
    }
    return loop_stack_.back();
}

// Convert binary operator to TAC opcode
TACOpcode IRGenerator::binary_op_to_opcode(const std::string& op) {
    if (op == "+") return TACOpcode::ADD;
    if (op == "-") return TACOpcode::SUB;
    if (op == "*") return TACOpcode::MUL;
    if (op == "/") return TACOpcode::DIV;
    if (op == "%") return TACOpcode::MOD;
    if (op == "&") return TACOpcode::AND;
    if (op == "|") return TACOpcode::OR;
    if (op == "^") return TACOpcode::XOR;
    if (op == "<<") return TACOpcode::SHL;
    if (op == ">>") return TACOpcode::SHR;
    if (op == "&&") return TACOpcode::LAND;
    if (op == "||") return TACOpcode::LOR;
    if (op == "==") return TACOpcode::EQ;
    if (op == "!=") return TACOpcode::NE;
    if (op == "<") return TACOpcode::LT;
    if (op == "<=") return TACOpcode::LE;
    if (op == ">") return TACOpcode::GT;
    if (op == ">=") return TACOpcode::GE;
    
    report_error(IRGenError::InvalidOperand, "Unknown binary operator: " + op);
    return TACOpcode::NOP;
}

// ===== Code Generation Methods =====

void IRGenerator::generate_program(std::shared_ptr<ProgramNode> program) {
    if (!program) return;
    
    // Generate global variables
    for (auto& global_var : program->globalVariables) {
        if (global_var) {
            program_.global_vars.push_back({global_var->name, global_var->type});
        }
    }
    
    // Generate functions
    for (auto& func : program->functions) {
        if (func) {
            generate_function(func);
        }
    }
}

void IRGenerator::generate_function(std::shared_ptr<FunctionNode> func) {
    if (!func) return;
    
    // Create new function
    IRFunction ir_func(func->name, func->returnType);
    
    // Set current function
    current_function_ = &ir_func;
    
    // Add parameters
    for (auto& param : func->parameters) {
        if (param) {
            ir_func.parameters.push_back(param->name);
        }
    }
    
    // Generate function body
    if (func->body) {
        generate_block(func->body);
    }
    
    // Add implicit return for void functions
    if (func->returnType == "void") {
        emit(TACOpcode::RETURN_VOID);
    }
    
    // Add function to program
    program_.functions.push_back(ir_func);
    
    // Clear current function
    current_function_ = nullptr;
}

void IRGenerator::generate_block(std::shared_ptr<BlockNode> block) {
    if (!block) return;
    
    for (auto& stmt : block->statements) {
        if (stmt) {
            generate_statement(stmt);
        }
    }
}

void IRGenerator::generate_statement(std::shared_ptr<StatementNode> stmt) {
    if (!stmt) return;
    
    // Variable declaration
    if (auto var_decl = std::dynamic_pointer_cast<VariableDeclarationNode>(stmt)) {
        generate_variable_decl(var_decl);
    }
    // Assignment
    else if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(stmt)) {
        generate_assignment(assign);
    }
    // If statement
    else if (auto if_stmt = std::dynamic_pointer_cast<IfStatementNode>(stmt)) {
        generate_if(if_stmt);
    }
    // While loop
    else if (auto while_stmt = std::dynamic_pointer_cast<WhileStatementNode>(stmt)) {
        generate_while(while_stmt);
    }
    // For loop
    else if (auto for_stmt = std::dynamic_pointer_cast<ForStatementNode>(stmt)) {
        generate_for(for_stmt);
    }
    // Do-while loop
    else if (auto do_while = std::dynamic_pointer_cast<DoWhileStatementNode>(stmt)) {
        generate_do_while(do_while);
    }
    // Switch statement
    else if (auto switch_stmt = std::dynamic_pointer_cast<SwitchStatementNode>(stmt)) {
        generate_switch(switch_stmt);
    }
    // Return statement
    else if (auto return_stmt = std::dynamic_pointer_cast<ReturnStatementNode>(stmt)) {
        generate_return(return_stmt);
    }
    // Break statement
    else if (auto break_stmt = std::dynamic_pointer_cast<BreakStatementNode>(stmt)) {
        generate_break(break_stmt);
    }
    // Continue statement
    else if (auto continue_stmt = std::dynamic_pointer_cast<ContinueStatementNode>(stmt)) {
        generate_continue(continue_stmt);
    }
    // Print statement
    else if (auto print_stmt = std::dynamic_pointer_cast<PrintStatementNode>(stmt)) {
        generate_print(print_stmt);
    }
    // Block
    else if (auto block = std::dynamic_pointer_cast<BlockNode>(stmt)) {
        generate_block(block);
    }
}

void IRGenerator::generate_variable_decl(std::shared_ptr<VariableDeclarationNode> decl) {
    if (!decl) return;
    
    // If there's an initializer, generate assignment
    if (decl->initializer) {
        TACOperand value = generate_expression(decl->initializer);
        TACOperand var(TACOperand::Kind::VAR, decl->name, decl->type);
        emit(TACOpcode::COPY, var, value);
    }
}

void IRGenerator::generate_assignment(std::shared_ptr<AssignmentNode> assign) {
    if (!assign) return;
    
    TACOperand value = generate_expression(assign->value);
    TACOperand var(TACOperand::Kind::VAR, assign->variable);
    emit(TACOpcode::COPY, var, value);
}

void IRGenerator::generate_if(std::shared_ptr<IfStatementNode> if_stmt) {
    if (!if_stmt) return;
    
    std::string else_label = new_label();
    std::string end_label = new_label();
    
    // Evaluate condition
    TACOperand cond = generate_expression(if_stmt->condition);
    
    // If condition is false, jump to else/end
    if (if_stmt->elseBlock) {
        emit(TACOpcode::IF_FALSE, TACOperand(TACOperand::Kind::LABEL, else_label), cond);
    } else {
        emit(TACOpcode::IF_FALSE, TACOperand(TACOperand::Kind::LABEL, end_label), cond);
    }
    
    // Generate then block
    if (if_stmt->thenBlock) {
        generate_block(if_stmt->thenBlock);
    }
    
    // Jump to end (skip else)
    if (if_stmt->elseBlock) {
        emit(TACOpcode::GOTO, TACOperand(TACOperand::Kind::LABEL, end_label));
        
        // Else label
        emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, else_label));
        
        // Generate else block
        generate_block(if_stmt->elseBlock);
    }
    
    // End label
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, end_label));
}

void IRGenerator::generate_while(std::shared_ptr<WhileStatementNode> while_stmt) {
    if (!while_stmt) return;
    
    std::string start_label = new_label();
    std::string end_label = new_label();
    
    // Push loop context
    push_loop(end_label, start_label);
    
    // Start label
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, start_label));
    
    // Evaluate condition
    TACOperand cond = generate_expression(while_stmt->condition);
    
    // If false, jump to end
    emit(TACOpcode::IF_FALSE, TACOperand(TACOperand::Kind::LABEL, end_label), cond);
    
    // Generate body
    if (while_stmt->body) {
        generate_block(while_stmt->body);
    }
    
    // Jump back to start
    emit(TACOpcode::GOTO, TACOperand(TACOperand::Kind::LABEL, start_label));
    
    // End label
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, end_label));
    
    // Pop loop context
    pop_loop();
}

void IRGenerator::generate_for(std::shared_ptr<ForStatementNode> for_stmt) {
    if (!for_stmt) return;
    
    std::string start_label = new_label();
    std::string continue_label = new_label();
    std::string end_label = new_label();
    
    // Push loop context
    push_loop(end_label, continue_label);
    
    // Generate initialization
    if (for_stmt->initialization) {
        generate_statement(for_stmt->initialization);
    }
    
    // Start label
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, start_label));
    
    // Evaluate condition
    if (for_stmt->condition) {
        TACOperand cond = generate_expression(for_stmt->condition);
        emit(TACOpcode::IF_FALSE, TACOperand(TACOperand::Kind::LABEL, end_label), cond);
    }
    
    // Generate body
    if (for_stmt->body) {
        generate_block(for_stmt->body);
    }
    
    // Continue label (for continue statements)
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, continue_label));
    
    // Generate update
    if (for_stmt->update) {
        generate_statement(for_stmt->update);
    }
    
    // Jump back to start
    emit(TACOpcode::GOTO, TACOperand(TACOperand::Kind::LABEL, start_label));
    
    // End label
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, end_label));
    
    // Pop loop context
    pop_loop();
}

void IRGenerator::generate_do_while(std::shared_ptr<DoWhileStatementNode> do_while) {
    if (!do_while) return;
    
    std::string start_label = new_label();
    std::string end_label = new_label();
    
    // Push loop context
    push_loop(end_label, start_label);
    
    // Start label
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, start_label));
    
    // Generate body
    if (do_while->body) {
        generate_block(do_while->body);
    }
    
    // Evaluate condition
    TACOperand cond = generate_expression(do_while->condition);
    
    // If true, jump back to start
    emit(TACOpcode::IF_TRUE, TACOperand(TACOperand::Kind::LABEL, start_label), cond);
    
    // End label
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, end_label));
    
    // Pop loop context
    pop_loop();
}

void IRGenerator::generate_switch(std::shared_ptr<SwitchStatementNode> switch_stmt) {
    if (!switch_stmt) return;
    
    std::string end_label = new_label();
    
    // Push loop context (switch allows break)
    push_loop(end_label, "");
    
    // Evaluate switch expression
    TACOperand switch_val = generate_expression(switch_stmt->expression);
    
    // Generate labels for each case
    std::vector<std::string> case_labels;
    for (size_t i = 0; i < switch_stmt->cases.size(); i++) {
        case_labels.push_back(new_label());
    }
    std::string default_label = new_label();
    
    // Generate comparisons for each case
    for (size_t i = 0; i < switch_stmt->cases.size(); i++) {
        auto& case_stmt = switch_stmt->cases[i];
        if (case_stmt && case_stmt->value) {
            TACOperand case_val = generate_expression(case_stmt->value);
            std::string temp = new_temp();
            TACOperand temp_op(TACOperand::Kind::TEMP, temp);
            emit(TACOpcode::EQ, temp_op, switch_val, case_val);
            emit(TACOpcode::IF_TRUE, TACOperand(TACOperand::Kind::LABEL, case_labels[i]), temp_op);
        }
    }
    
    // Jump to default or end
    if (switch_stmt->defaultCase) {
        emit(TACOpcode::GOTO, TACOperand(TACOperand::Kind::LABEL, default_label));
    } else {
        emit(TACOpcode::GOTO, TACOperand(TACOperand::Kind::LABEL, end_label));
    }
    
    // Generate case bodies
    for (size_t i = 0; i < switch_stmt->cases.size(); i++) {
        emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, case_labels[i]));
        auto& case_stmt = switch_stmt->cases[i];
        if (case_stmt) {
            for (auto& stmt : case_stmt->statements) {
                generate_statement(stmt);
            }
        }
    }
    
    // Generate default case
    if (switch_stmt->defaultCase) {
        emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, default_label));
        for (auto& stmt : switch_stmt->defaultCase->statements) {
            generate_statement(stmt);
        }
    }
    
    // End label
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, end_label));
    
    // Pop loop context
    pop_loop();
}

void IRGenerator::generate_return(std::shared_ptr<ReturnStatementNode> return_stmt) {
    if (!return_stmt) return;
    
    if (return_stmt->expression) {
        TACOperand value = generate_expression(return_stmt->expression);
        emit(TACOpcode::RETURN, TACOperand(), value);
    } else {
        emit(TACOpcode::RETURN_VOID);
    }
}

void IRGenerator::generate_break(std::shared_ptr<BreakStatementNode> break_stmt) {
    auto loop = current_loop();
    if (!loop.has_value()) {
        report_error(IRGenError::InvalidStatement, "Break statement outside of loop/switch");
        return;
    }
    
    emit(TACOpcode::GOTO, TACOperand(TACOperand::Kind::LABEL, loop->break_label));
}

void IRGenerator::generate_continue(std::shared_ptr<ContinueStatementNode> continue_stmt) {
    auto loop = current_loop();
    if (!loop.has_value()) {
        report_error(IRGenError::InvalidStatement, "Continue statement outside of loop");
        return;
    }
    
    if (loop->continue_label.empty()) {
        report_error(IRGenError::InvalidStatement, "Continue not allowed in switch statement");
        return;
    }
    
    emit(TACOpcode::GOTO, TACOperand(TACOperand::Kind::LABEL, loop->continue_label));
}

void IRGenerator::generate_print(std::shared_ptr<PrintStatementNode> print_stmt) {
    if (!print_stmt || !print_stmt->expression) return;
    
    TACOperand value = generate_expression(print_stmt->expression);
    emit(TACOpcode::PRINT, value);
}

// ===== Expression Generation =====

TACOperand IRGenerator::generate_expression(std::shared_ptr<ExpressionNode> expr) {
    if (!expr) {
        return TACOperand(TACOperand::Kind::CONST, "0");
    }
    
    // Literal
    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr)) {
        return TACOperand(TACOperand::Kind::CONST, lit->value, lit->type);
    }
    
    // Identifier
    if (auto ident = std::dynamic_pointer_cast<IdentifierNode>(expr)) {
        return TACOperand(TACOperand::Kind::VAR, ident->name);
    }
    
    // Binary operation
    if (auto binary = std::dynamic_pointer_cast<BinaryOpNode>(expr)) {
        return generate_binary_op(binary);
    }
    
    // Unary operation
    if (auto unary = std::dynamic_pointer_cast<UnaryOpNode>(expr)) {
        return generate_unary_op(unary);
    }
    
    // Function call
    if (auto call = std::dynamic_pointer_cast<FunctionCallNode>(expr)) {
        return generate_function_call(call);
    }
    
    // Array access
    if (auto array_access = std::dynamic_pointer_cast<ArrayAccessNode>(expr)) {
        return generate_array_access(array_access);
    }
    
    // Ternary operator
    if (auto ternary = std::dynamic_pointer_cast<TernaryNode>(expr)) {
        return generate_ternary(ternary);
    }
    
    // Dereference
    if (auto deref = std::dynamic_pointer_cast<DereferenceNode>(expr)) {
        TACOperand ptr = generate_expression(deref->operand);
        std::string temp = new_temp();
        TACOperand result(TACOperand::Kind::TEMP, temp);
        emit(TACOpcode::DEREF, result, ptr);
        return result;
    }
    
    // Address-of
    if (auto addr = std::dynamic_pointer_cast<AddressOfNode>(expr)) {
        TACOperand operand = generate_expression(addr->operand);
        std::string temp = new_temp();
        TACOperand result(TACOperand::Kind::TEMP, temp);
        emit(TACOpcode::ADDR, result, operand);
        return result;
    }
    
    report_error(IRGenError::InvalidExpression, "Unknown expression type");
    return TACOperand(TACOperand::Kind::CONST, "0");
}

TACOperand IRGenerator::generate_binary_op(std::shared_ptr<BinaryOpNode> binary) {
    if (!binary) {
        return TACOperand(TACOperand::Kind::CONST, "0");
    }
    
    // Short-circuit evaluation for && and ||
    if (binary->op == "&&" || binary->op == "||") {
        std::string result_temp = new_temp();
        TACOperand result(TACOperand::Kind::TEMP, result_temp);
        
        TACOperand left = generate_expression(binary->left);
        
        std::string short_circuit_label = new_label();
        std::string end_label = new_label();
        
        if (binary->op == "&&") {
            // If left is false, result is false
            emit(TACOpcode::COPY, result, left);
            emit(TACOpcode::IF_FALSE, TACOperand(TACOperand::Kind::LABEL, end_label), left);
            
            // Evaluate right
            TACOperand right = generate_expression(binary->right);
            emit(TACOpcode::COPY, result, right);
        } else {  // ||
            // If left is true, result is true
            emit(TACOpcode::COPY, result, left);
            emit(TACOpcode::IF_TRUE, TACOperand(TACOperand::Kind::LABEL, end_label), left);
            
            // Evaluate right
            TACOperand right = generate_expression(binary->right);
            emit(TACOpcode::COPY, result, right);
        }
        
        emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, end_label));
        return result;
    }
    
    // Regular binary operations
    TACOperand left = generate_expression(binary->left);
    TACOperand right = generate_expression(binary->right);
    
    std::string temp = new_temp();
    TACOperand result(TACOperand::Kind::TEMP, temp);
    
    TACOpcode opcode = binary_op_to_opcode(binary->op);
    emit(opcode, result, left, right);
    
    return result;
}

TACOperand IRGenerator::generate_unary_op(std::shared_ptr<UnaryOpNode> unary) {
    if (!unary) {
        return TACOperand(TACOperand::Kind::CONST, "0");
    }
    
    TACOperand operand = generate_expression(unary->operand);
    std::string temp = new_temp();
    TACOperand result(TACOperand::Kind::TEMP, temp);
    
    if (unary->op == "-") {
        emit(TACOpcode::NEG, result, operand);
    } else if (unary->op == "!") {
        emit(TACOpcode::LNOT, result, operand);
    } else if (unary->op == "~") {
        emit(TACOpcode::NOT, result, operand);
    } else {
        report_error(IRGenError::InvalidOperand, "Unknown unary operator: " + unary->op);
    }
    
    return result;
}

TACOperand IRGenerator::generate_function_call(std::shared_ptr<FunctionCallNode> call) {
    if (!call) {
        return TACOperand(TACOperand::Kind::CONST, "0");
    }
    
    // Generate arguments and emit PARAM instructions
    for (auto& arg : call->arguments) {
        TACOperand arg_val = generate_expression(arg);
        emit(TACOpcode::PARAM, arg_val);
    }
    
    // Emit CALL instruction
    std::string temp = new_temp();
    TACOperand result(TACOperand::Kind::TEMP, temp);
    TACOperand func(TACOperand::Kind::FUNC, call->functionName);
    TACOperand num_args(TACOperand::Kind::CONST, std::to_string(call->arguments.size()));
    
    emit(TACOpcode::CALL, result, func, num_args);
    
    return result;
}

TACOperand IRGenerator::generate_array_access(std::shared_ptr<ArrayAccessNode> access) {
    if (!access) {
        return TACOperand(TACOperand::Kind::CONST, "0");
    }
    
    TACOperand index = generate_expression(access->index);
    TACOperand array(TACOperand::Kind::VAR, access->arrayName);
    
    std::string temp = new_temp();
    TACOperand result(TACOperand::Kind::TEMP, temp);
    
    emit(TACOpcode::ARRAY_LOAD, result, array, index);
    
    return result;
}

TACOperand IRGenerator::generate_ternary(std::shared_ptr<TernaryNode> ternary) {
    if (!ternary) {
        return TACOperand(TACOperand::Kind::CONST, "0");
    }
    
    std::string result_temp = new_temp();
    TACOperand result(TACOperand::Kind::TEMP, result_temp);
    
    std::string false_label = new_label();
    std::string end_label = new_label();
    
    // Evaluate condition
    TACOperand cond = generate_expression(ternary->condition);
    emit(TACOpcode::IF_FALSE, TACOperand(TACOperand::Kind::LABEL, false_label), cond);
    
    // True branch
    TACOperand true_val = generate_expression(ternary->trueValue);
    emit(TACOpcode::COPY, result, true_val);
    emit(TACOpcode::GOTO, TACOperand(TACOperand::Kind::LABEL, end_label));
    
    // False branch
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, false_label));
    TACOperand false_val = generate_expression(ternary->falseValue);
    emit(TACOpcode::COPY, result, false_val);
    
    // End
    emit(TACOpcode::LABEL, TACOperand(TACOperand::Kind::LABEL, end_label));
    
    return result;
}

// ===== Main API =====

IRGenResult IRGenerator::generate(std::shared_ptr<ProgramNode> ast,
                                 std::shared_ptr<ScopeAnalysisResult> scope_result,
                                 std::shared_ptr<TypeCheckResult> type_result) {
    // Store references
    scope_result_ = scope_result;
    type_result_ = type_result;
    
    // Reset state
    program_ = IRProgram();
    errors_.clear();
    string_literal_count_ = 0;
    loop_stack_.clear();
    
    // Generate IR
    generate_program(ast);
    
    // Build result
    IRGenResult result;
    result.program = program_;
    result.errors = errors_;
    result.success = errors_.empty();
    
    return result;
}
