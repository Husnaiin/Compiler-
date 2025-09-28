#include "lexer.h"
#include "parser.h"
#include "token.h"
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
	}
	
	std::cout << "\n=== AST ===" << std::endl;
	parser.printAST(ast);
	return parser.hasErrors() ? 1 : 0;
}


