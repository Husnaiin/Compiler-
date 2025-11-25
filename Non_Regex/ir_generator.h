#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "parser.h"
#include "scope_analyzer.h"
#include "type_checker.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <optional>
#include <sstream>

// IR Generation Error Types
enum class IRGenError {
    InvalidExpression,
    InvalidStatement,
    UndefinedVariable,
    UndefinedFunction,
    TypeMismatch,
    InvalidOperand,
    UnreachableCode,
    InvalidArrayAccess,
    InvalidPointerOperation,
    MissingReturnValue,
    InvalidLValue,
    DivisionByZero
};

// TAC Opcode Types
enum class TACOpcode {
    // Assignment and copy
    COPY,           // dest = src
    
    // Arithmetic operations
    ADD,            // dest = src1 + src2
    SUB,            // dest = src1 - src2
    MUL,            // dest = src1 * src2
    DIV,            // dest = src1 / src2
    MOD,            // dest = src1 % src2
    NEG,            // dest = -src1
    
    // Bitwise operations
    AND,            // dest = src1 & src2
    OR,             // dest = src1 | src2
    XOR,            // dest = src1 ^ src2
    NOT,            // dest = ~src1
    SHL,            // dest = src1 << src2
    SHR,            // dest = src1 >> src2
    
    // Logical operations
    LAND,           // dest = src1 && src2
    LOR,            // dest = src1 || src2
    LNOT,           // dest = !src1
    
    // Comparison operations
    EQ,             // dest = src1 == src2
    NE,             // dest = src1 != src2
    LT,             // dest = src1 < src2
    LE,             // dest = src1 <= src2
    GT,             // dest = src1 > src2
    GE,             // dest = src1 >= src2
    
    // Control flow
    LABEL,          // label:
    GOTO,           // goto label
    IF_FALSE,       // if (!condition) goto label
    IF_TRUE,        // if (condition) goto label
    
    // Function operations
    PARAM,          // param arg
    CALL,           // dest = call func, num_args
    RETURN,         // return src
    RETURN_VOID,    // return
    
    // Memory operations
    ADDR,           // dest = &src
    DEREF,          // dest = *src
    STORE,          // *dest = src
    LOAD_ADDR,      // dest = [src]
    
    // Array operations
    ARRAY_LOAD,     // dest = arr[index]
    ARRAY_STORE,    // arr[index] = src
    ARRAY_ADDR,     // dest = &arr[index]
    
    // Special operations
    PRINT,          // print src
    NOP             // no operation
};

// TAC Operand (can be variable, temporary, constant, or label)
struct TACOperand {
    enum class Kind {
        TEMP,       // Temporary variable (t0, t1, ...)
        VAR,        // Named variable
        CONST,      // Constant value
        LABEL,      // Label
        FUNC        // Function name
    };
    
    Kind kind;
    std::string value;
    std::string type;  // Type information for type checking
    
    TACOperand() : kind(Kind::TEMP), value(""), type("void") {}
    TACOperand(Kind k, const std::string& v, const std::string& t = "int") 
        : kind(k), value(v), type(t) {}
    
    std::string toString() const;
    bool isValid() const { return !value.empty(); }
};

// TAC Instruction
struct TACInstruction {
    TACOpcode opcode;
    TACOperand dest;
    TACOperand src1;
    TACOperand src2;
    std::string comment;  // Optional comment for readability
    
    TACInstruction(TACOpcode op) : opcode(op) {}
    
    TACInstruction(TACOpcode op, const TACOperand& d)
        : opcode(op), dest(d) {}
    
    TACInstruction(TACOpcode op, const TACOperand& d, const TACOperand& s1)
        : opcode(op), dest(d), src1(s1) {}
    
    TACInstruction(TACOpcode op, const TACOperand& d, const TACOperand& s1, const TACOperand& s2)
        : opcode(op), dest(d), src1(s1), src2(s2) {}
    
    std::string toString() const;
};

// IR Generation Error Entry
struct IRGenErrorEntry {
    IRGenError error_type;
    std::string message;
    std::optional<std::string> related_symbol;
    
    IRGenErrorEntry(IRGenError type, const std::string& msg,
                   const std::optional<std::string>& symbol = std::nullopt)
        : error_type(type), message(msg), related_symbol(symbol) {}
};

// IR Function (collection of TAC instructions with metadata)
struct IRFunction {
    std::string name;
    std::string return_type;
    std::vector<std::string> parameters;
    std::vector<TACInstruction> instructions;
    int temp_count;  // Number of temporaries used
    int label_count; // Number of labels used
    
    IRFunction(const std::string& n, const std::string& ret_type)
        : name(n), return_type(ret_type), temp_count(0), label_count(0) {}
};

// IR Program (collection of functions and global variables)
struct IRProgram {
    std::vector<IRFunction> functions;
    std::vector<std::pair<std::string, std::string>> global_vars;  // (name, type)
    std::unordered_map<std::string, std::string> string_literals;  // label -> string value
    
    std::string toString() const;
    void print() const;
};

// IR Generation Result
struct IRGenResult {
    IRProgram program;
    std::vector<IRGenErrorEntry> errors;
    bool success;
    
    IRGenResult() : success(true) {}
    bool hasErrors() const { return !errors.empty(); }
};

// IR Generator Class
class IRGenerator {
private:
    // Current function being processed
    IRFunction* current_function_;
    
    // Scope and type information
    std::shared_ptr<ScopeAnalysisResult> scope_result_;
    std::shared_ptr<TypeCheckResult> type_result_;
    
    // Generated program
    IRProgram program_;
    
    // Error tracking
    std::vector<IRGenErrorEntry> errors_;
    
    // Control flow tracking
    struct LoopContext {
        std::string break_label;
        std::string continue_label;
    };
    std::vector<LoopContext> loop_stack_;
    
    // String literal management
    int string_literal_count_;
    
    // Helper methods
    std::string new_temp();
    std::string new_label();
    std::string new_string_label();
    void emit(const TACInstruction& instr);
    void emit(TACOpcode opcode);
    void emit(TACOpcode opcode, const TACOperand& dest);
    void emit(TACOpcode opcode, const TACOperand& dest, const TACOperand& src1);
    void emit(TACOpcode opcode, const TACOperand& dest, const TACOperand& src1, const TACOperand& src2);
    
    // Error reporting
    void report_error(IRGenError type, const std::string& message,
                     const std::optional<std::string>& symbol = std::nullopt);
    
    // Type information retrieval
    std::string get_variable_type(const std::string& name);
    std::string get_expression_type(std::shared_ptr<ExpressionNode> expr);
    
    // Code generation methods
    void generate_program(std::shared_ptr<ProgramNode> program);
    void generate_function(std::shared_ptr<FunctionNode> func);
    void generate_statement(std::shared_ptr<StatementNode> stmt);
    TACOperand generate_expression(std::shared_ptr<ExpressionNode> expr);
    TACOperand generate_binary_op(std::shared_ptr<BinaryOpNode> binary);
    TACOperand generate_unary_op(std::shared_ptr<UnaryOpNode> unary);
    TACOperand generate_function_call(std::shared_ptr<FunctionCallNode> call);
    TACOperand generate_array_access(std::shared_ptr<ArrayAccessNode> access);
    TACOperand generate_ternary(std::shared_ptr<TernaryNode> ternary);
    
    // Statement generation
    void generate_block(std::shared_ptr<BlockNode> block);
    void generate_if(std::shared_ptr<IfStatementNode> if_stmt);
    void generate_while(std::shared_ptr<WhileStatementNode> while_stmt);
    void generate_for(std::shared_ptr<ForStatementNode> for_stmt);
    void generate_do_while(std::shared_ptr<DoWhileStatementNode> do_while);
    void generate_switch(std::shared_ptr<SwitchStatementNode> switch_stmt);
    void generate_return(std::shared_ptr<ReturnStatementNode> return_stmt);
    void generate_break(std::shared_ptr<BreakStatementNode> break_stmt);
    void generate_continue(std::shared_ptr<ContinueStatementNode> continue_stmt);
    void generate_assignment(std::shared_ptr<AssignmentNode> assign);
    void generate_variable_decl(std::shared_ptr<VariableDeclarationNode> decl);
    void generate_print(std::shared_ptr<PrintStatementNode> print_stmt);
    
    // Helper for control flow
    void push_loop(const std::string& break_label, const std::string& continue_label);
    void pop_loop();
    std::optional<LoopContext> current_loop();
    
    // Opcode helper
    TACOpcode binary_op_to_opcode(const std::string& op);
    
public:
    IRGenerator();
    
    // Main API
    IRGenResult generate(std::shared_ptr<ProgramNode> ast,
                        std::shared_ptr<ScopeAnalysisResult> scope_result,
                        std::shared_ptr<TypeCheckResult> type_result);
    
    // Get generated program
    const IRProgram& get_program() const { return program_; }
    
    // Error checking
    bool has_errors() const { return !errors_.empty(); }
    const std::vector<IRGenErrorEntry>& get_errors() const { return errors_; }
};

#endif // IR_GENERATOR_H
