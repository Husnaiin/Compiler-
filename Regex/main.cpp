#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.h"

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

    bool hasError = false;
    cout << "Tokens:\n";
    for (const auto& token : tokens) {
        // Check for error tokens and print them to standard error.
        if (token.type == TokenType::T_ERROR) {
            cerr << "Lexical Error: " << token.value
                 << " at line " << token.line
                 << ", column " << token.column << endl;
            hasError = true;
        } else if (token.type != TokenType::T_EOF) {
            cout << token.toString() << endl;
        }
    }

    // Print the final End-Of-File token.
    if (!tokens.empty() && tokens.back().type == TokenType::T_EOF) {
        cout << tokens.back().toString() << endl;
    }

    return hasError ? 1 : 0;
}
