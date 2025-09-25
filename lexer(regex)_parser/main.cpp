#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.h"
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <source_file>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open the file: " << filename << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string sourceCode = buffer.str();

    Lexer lexer(sourceCode);
    vector<Token> tokens = lexer.tokenize();

    // Check for lexical errors
    bool hasError = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::T_ERROR) {
            cerr << "Lexical Error: " << token.value
                 << " at line " << token.line
                 << ", column " << token.column << endl;
            hasError = true;
        }
    }

    if (hasError) {
        return 1;
    }

    // Print the token stream
    cout << "## Token Stream" << endl;
    cout << "```" << endl;
    cout << "[";
    for (size_t i = 0; i < tokens.size(); ++i) {
        cout << tokens[i].toString();
        if (i < tokens.size() - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
    cout << "```" << endl << endl;


    // Parse the tokens
    Parser parser(tokens);
    Program program = parser.parse();

    // Check for parsing errors
    const vector<ParseError>& errors = parser.getErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            cerr << "Parse Error: " << error.message
                 << " at line " << error.token.line
                 << ", column " << error.token.column << endl;
        }
        return 1;
    }

    // Print the AST
    cout << "## Abstract Syntax Tree" << endl;
    cout << "```" << endl;
    cout << "[" << endl;
    for (size_t i = 0; i < program.size(); i++) {
        cout << program[i]->toString(1);
        if (i < program.size() - 1) {
            cout << ",";
        }
        cout << endl;
    }
    cout << "]" << endl;
    cout << "```" << endl;

    return 0;
}