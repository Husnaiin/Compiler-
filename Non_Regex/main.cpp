#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "scope_analyzer.h"
#include "type_checker.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
	// Use a file input by default; allow passing a source file path as argv[1]
	std::string input = (argc > 1) ? argv[1] : "sample_program.txt";
	bool isFile = true;

	// Step 1: Lexical Analysis (Tokenization)
	std::cout << "=== STEP 1: LEXICAL ANALYSIS ===" << std::endl;
	Lexer lexer(input, isFile);
	std::vector<Token> tokens = lexer.tokenize();
	std::cout << "✓ Tokenization completed. Total tokens: " << tokens.size() << std::endl;
	
	// Optional: Print first 20 and last 5 tokens for verification
	std::cout << "\nFirst 20 tokens:" << std::endl;
	for (size_t i = 0; i < std::min(size_t(20), tokens.size()); i++) {
		std::cout << "  ";
		tokens[i].print();
		std::cout << std::endl;
	}
	if (tokens.size() > 25) {
		std::cout << "  ... (" << (tokens.size() - 25) << " tokens omitted) ..." << std::endl;
		std::cout << "Last 5 tokens:" << std::endl;
		for (size_t i = tokens.size() - 5; i < tokens.size(); i++) {
			std::cout << "  ";
			tokens[i].print();
			std::cout << std::endl;
		}
	}

	// Step 2: Syntax Analysis (Parsing)
	std::cout << "\n=== STEP 2: SYNTAX ANALYSIS (PARSING) ===" << std::endl;
	Parser parser(tokens);
	auto ast = parser.parse();
	
	if (parser.hasErrors()) {
		std::cerr << "\n❌ Parsing completed with " << parser.getErrorCount() << " error(s)." << std::endl;
		std::cerr << "AST may be incomplete due to parsing errors." << std::endl;
		return 1;
	}
	std::cout << "✓ Parsing completed successfully." << std::endl;
	
	std::cout << "\n=== ABSTRACT SYNTAX TREE (AST) ===" << std::endl;
	parser.printAST(ast);
	
	// Step 3: Semantic Analysis - Scope Analysis
	std::cout << "\n=== STEP 3: SEMANTIC ANALYSIS - SCOPE CHECKING ===" << std::endl;
	ScopeAnalyzer analyzer;
	ScopeAnalysisResult result = analyzer.analyze_scopes(ast);
	
	std::cout << "\n--- Scope Tree ---" << std::endl;
	analyzer.dump_scope_tree();
	
	std::cout << "\n--- Symbol Table ---" << std::endl;
	analyzer.dump_symbol_table();
	
	// Check for scope errors
	if (result.hasErrors()) {
		std::cout << "\n❌ SCOPE ERRORS DETECTED:" << std::endl;
		for (const auto& error : result.errors) {
			std::string error_type;
			switch (error.error_type) {
				case ScopeError::UndeclaredVariableAccessed:
					error_type = "UndeclaredVariableAccessed";
					break;
				case ScopeError::UndefinedFunctionCalled:
					error_type = "UndefinedFunctionCalled";
					break;
				case ScopeError::VariableRedefinition:
					error_type = "VariableRedefinition";
					break;
				case ScopeError::FunctionPrototypeRedefinition:
					error_type = "FunctionPrototypeRedefinition";
					break;
			}
			std::cout << "  [" << error_type << "] " << error.message 
			          << " at " << error.location.toString();
			if (error.related_symbol.has_value()) {
				std::cout << " (symbol: " << error.related_symbol.value() << ")";
			}
			std::cout << std::endl;
		}
		std::cerr << "\n❌ Scope analysis completed with " << result.errors.size() 
		          << " error(s). Skipping type checking." << std::endl;
		return 1;
	} else {
		std::cout << "\n✓ Scope analysis completed successfully - No scope errors found." << std::endl;
	}
	
	// Step 4: Semantic Analysis - Type Checking
	std::cout << "\n=== STEP 4: SEMANTIC ANALYSIS - TYPE CHECKING ===" << std::endl;
	TypeChecker type_checker(result);
	TypeCheckResult type_result = type_checker.check_types(ast);
	
	// Print type checking results
	std::cout << "\n--- Type Checking Results ---" << std::endl;
	type_checker.print_type_errors();
	
	if (type_result.hasErrors()) {
		std::cerr << "\n❌ Type checking completed with " << type_result.errors.size() 
		          << " error(s)." << std::endl;
		return 1;
	} else {
		std::cout << "\n✓ Type checking completed successfully - No type errors found." << std::endl;
	}
	
	// Compilation Summary
	std::cout << "\n" << std::string(60, '=') << std::endl;
	std::cout << "✓✓✓ COMPILATION SUCCESSFUL ✓✓✓" << std::endl;
	std::cout << std::string(60, '=') << std::endl;
	std::cout << "\nAll compilation phases completed successfully:" << std::endl;
	std::cout << "  ✓ Lexical Analysis (Tokenization)" << std::endl;
	std::cout << "  ✓ Syntax Analysis (Parsing)" << std::endl;
	std::cout << "  ✓ Semantic Analysis (Scope Checking)" << std::endl;
	std::cout << "  ✓ Semantic Analysis (Type Checking)" << std::endl;
	std::cout << std::string(60, '=') << std::endl;
	
	return 0;
}


