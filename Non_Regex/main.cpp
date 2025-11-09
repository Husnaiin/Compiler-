#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "scope_analyzer.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
	// Use a file input by default; allow passing a source file path as argv[1]
	std::string input = (argc > 1) ? argv[1] : "sample_program.txt";
	bool isFile = true;

	Lexer lexer(input, isFile);
	std::vector<Token> tokens = lexer.tokenize();

	std::cout << "=== TOKENS ===" << std::endl;
	for (const auto& tok : tokens) {
		tok.print();
		std::cout << std::endl;
	}

	Parser parser(tokens);
	auto ast = parser.parse();
	
	if (parser.hasErrors()) {
		std::cerr << "\nParsing completed with " << parser.getErrorCount() << " error(s)." << std::endl;
		std::cerr << "AST may be incomplete due to parsing errors." << std::endl;
		return 1;
	}
	
	std::cout << "\n=== AST ===" << std::endl;
	parser.printAST(ast);
	
	// Scope Analysis
	std::cout << "\n=== SCOPE ANALYSIS ===" << std::endl;
	ScopeAnalyzer analyzer;
	ScopeAnalysisResult result = analyzer.analyze_scopes(ast);
	
	// Print scope tree
	analyzer.dump_scope_tree();
	
	// Print symbol table
	std::cout << std::endl;
	analyzer.dump_symbol_table();
	
	// Print errors
	if (result.hasErrors()) {
		std::cout << "\n=== SCOPE ERRORS ===" << std::endl;
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
			std::cout << "[" << error_type << "] " << error.message 
			          << " at " << error.location.toString();
			if (error.related_symbol.has_value()) {
				std::cout << " (symbol: " << error.related_symbol.value() << ")";
			}
			std::cout << std::endl;
		}
		return 1;
	} else {
		std::cout << "\n=== NO SCOPE ERRORS ===" << std::endl;
	}
	
	return 0;
}


