#include "lexer.h"
#include "parser.h"
#include "scope_analyzer.h"
#include "type_checker.h"
#include "ir_generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

void print_separator(const std::string& title) {
    std::cout << "\n========================================\n";
    std::cout << title << "\n";
    std::cout << "========================================\n";
}

void print_ir_errors(const std::vector<IRGenErrorEntry>& errors) {
    if (errors.empty()) {
        std::cout << "No IR generation errors.\n";
        return;
    }
    
    std::cout << "IR Generation Errors (" << errors.size() << "):\n";
    for (const auto& error : errors) {
        std::string error_type;
        switch (error.error_type) {
            case IRGenError::InvalidExpression: error_type = "InvalidExpression"; break;
            case IRGenError::InvalidStatement: error_type = "InvalidStatement"; break;
            case IRGenError::UndefinedVariable: error_type = "UndefinedVariable"; break;
            case IRGenError::UndefinedFunction: error_type = "UndefinedFunction"; break;
            case IRGenError::TypeMismatch: error_type = "TypeMismatch"; break;
            case IRGenError::InvalidOperand: error_type = "InvalidOperand"; break;
            case IRGenError::UnreachableCode: error_type = "UnreachableCode"; break;
            case IRGenError::InvalidArrayAccess: error_type = "InvalidArrayAccess"; break;
            case IRGenError::InvalidPointerOperation: error_type = "InvalidPointerOperation"; break;
            case IRGenError::MissingReturnValue: error_type = "MissingReturnValue"; break;
            case IRGenError::InvalidLValue: error_type = "InvalidLValue"; break;
            case IRGenError::DivisionByZero: error_type = "DivisionByZero"; break;
            default: error_type = "Unknown"; break;
        }
        
        std::cout << "  [" << error_type << "] " << error.message;
        if (error.related_symbol.has_value()) {
            std::cout << " (Symbol: " << error.related_symbol.value() << ")";
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file> [output_file]\n";
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argc >= 3 ? argv[2] : "";
    
    // Read source file
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << input_file << "'\n";
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source_code = buffer.str();
    file.close();
    
    print_separator("SOURCE CODE");
    std::cout << source_code << "\n";
    
    // ===== LEXICAL ANALYSIS =====
    print_separator("LEXICAL ANALYSIS");
    Lexer lexer(source_code, false);  // false means it's a string, not a file
    std::vector<Token> tokens = lexer.tokenize();
    
    // Check for lexical errors
    bool has_lex_errors = false;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::T_ERROR) {
            has_lex_errors = true;
            break;
        }
    }
    
    if (has_lex_errors) {
        std::cerr << "Lexical analysis failed with errors.\n";
        return 1;
    }
    
    std::cout << "Tokenization successful. Total tokens: " << tokens.size() << "\n";
    
    // ===== SYNTAX ANALYSIS =====
    print_separator("SYNTAX ANALYSIS");
    Parser parser(tokens);
    std::shared_ptr<ProgramNode> ast = parser.parse();
    
    if (parser.hasErrors()) {
        std::cerr << "Parsing failed with " << parser.getErrorCount() << " error(s).\n";
        return 1;
    }
    
    std::cout << "Parsing successful.\n";
    
    // ===== SCOPE ANALYSIS =====
    print_separator("SCOPE ANALYSIS");
    ScopeAnalyzer scope_analyzer;
    ScopeAnalysisResult scope_result = scope_analyzer.analyze_scopes(ast);
    
    if (scope_result.hasErrors()) {
        std::cerr << "Scope analysis found " << scope_result.errors.size() << " error(s):\n";
        for (const auto& error : scope_result.errors) {
            std::string error_type;
            switch (error.error_type) {
                case ScopeError::UndeclaredVariableAccessed: error_type = "UndeclaredVariable"; break;
                case ScopeError::UndefinedFunctionCalled: error_type = "UndefinedFunction"; break;
                case ScopeError::VariableRedefinition: error_type = "VariableRedefinition"; break;
                case ScopeError::FunctionPrototypeRedefinition: error_type = "FunctionRedefinition"; break;
                default: error_type = "Unknown"; break;
            }
            std::cout << "  [" << error_type << "] " << error.message;
            if (error.related_symbol.has_value()) {
                std::cout << " (Symbol: " << error.related_symbol.value() << ")";
            }
            std::cout << " at " << error.location.toString() << "\n";
        }
        std::cerr << "\nScope analysis failed. Cannot proceed to IR generation.\n";
        return 1;
    }
    
    std::cout << "Scope analysis successful.\n";
    std::cout << "Total symbols: " << scope_result.symbols.size() << "\n";
    std::cout << "Total scopes: " << scope_result.scopes.size() << "\n";
    
    // ===== TYPE CHECKING =====
    print_separator("TYPE CHECKING");
    TypeChecker type_checker(scope_result);
    auto type_result = std::make_shared<TypeCheckResult>(
        type_checker.check_types(ast)
    );
    
    if (type_result->hasErrors()) {
        std::cerr << "Type checking found " << type_result->errors.size() << " error(s):\n";
        for (const auto& error : type_result->errors) {
            std::cout << "  " << error.message << "\n";
        }
        std::cerr << "\nType checking failed. IR generation may produce warnings.\n";
        // Continue anyway to generate IR (with potential issues)
    } else {
        std::cout << "Type checking successful.\n";
    }
    
    // ===== IR GENERATION =====
    print_separator("IR GENERATION");
    IRGenerator ir_generator;
    IRGenResult ir_result = ir_generator.generate(
        ast,
        std::make_shared<ScopeAnalysisResult>(scope_result),
        type_result
    );
    
    if (ir_result.hasErrors()) {
        std::cerr << "IR generation completed with warnings/errors:\n";
        print_ir_errors(ir_result.errors);
        std::cout << "\n";
    } else {
        std::cout << "IR generation successful!\n\n";
    }
    
    // ===== OUTPUT IR =====
    print_separator("GENERATED THREE-ADDRESS CODE (TAC)");
    
    std::string ir_output = ir_result.program.toString();
    std::cout << ir_output;
    
    // Write to output file if specified
    if (!output_file.empty()) {
        std::ofstream out(output_file);
        if (out.is_open()) {
            out << ir_output;
            out.close();
            std::cout << "\nIR code written to: " << output_file << "\n";
        } else {
            std::cerr << "Warning: Could not write to output file '" << output_file << "'\n";
        }
    }
    
    // ===== STATISTICS =====
    print_separator("STATISTICS");
    std::cout << "Functions generated: " << ir_result.program.functions.size() << "\n";
    std::cout << "Global variables: " << ir_result.program.global_vars.size() << "\n";
    std::cout << "String literals: " << ir_result.program.string_literals.size() << "\n";
    
    int total_instructions = 0;
    for (const auto& func : ir_result.program.functions) {
        total_instructions += func.instructions.size();
    }
    std::cout << "Total TAC instructions: " << total_instructions << "\n";
    
    print_separator("COMPILATION COMPLETE");
    
    if (ir_result.success && !type_result->hasErrors()) {
        std::cout << "✓ All phases completed successfully!\n";
        return 0;
    } else {
        std::cout << "⚠ Compilation completed with warnings/errors.\n";
        return ir_result.hasErrors() ? 1 : 0;
    }
}
