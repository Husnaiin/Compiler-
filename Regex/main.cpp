#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
using namespace std;
int main(int argc, char* argv[]) {
    if(argc < 2) {
        cerr << "Usage: " << argv[0] << " <source_file>" << endl;
        return 1;
    }
    string filename = argv[1];
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open the file: " << filename << endl;
        return 1; }
    stringstream buffer;
    buffer << file.rdbuf();
    string sourceCode = buffer.str();
    try{
        Lexer lexer(sourceCode);
        vector<Token> tokens = lexer.tokenize();
        cout << "Tokens:\n";
        for (const auto& token : tokens) {
            cout << token.toString() << endl;  // Show ALL tokens, not just EOF
        }
    } catch (const exception& e) {
        cerr << "Lexical error: " << e.what() << endl;
        return 1;
    }
    return 0;
    
}