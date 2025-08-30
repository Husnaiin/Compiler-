#include "lexer.h"
#include "token.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
	std::string input = argc > 1 ? argv[1] : "int x = 42;";
	bool isFile = (argc > 1);

	Lexer lexer(input, isFile);
	std::vector<Token> tokens = lexer.tokenize();

	for (const auto& tok : tokens) {
		tok.print();
		std::cout << std::endl;
	}
	return 0;
}


