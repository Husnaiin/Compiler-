## Compiler – Lexers (Non_Regex and Regex)

This repository contains two lexer implementations for a toy language:
- Non_Regex: manual, character-by-character scanner with escape handling, comments, and positions stored in tokens
- Regex: pattern-driven lexer using std::regex matching

Both versions build to small CLI programs you can run against source files.

### Prerequisites
- A C++17-capable compiler (g++ or clang++)
- Linux/macOS shell (Windows works with MSYS/WSL)

### Clone
```bash
git clone <your-repo-url> Compiler-
cd Compiler-
```

---
## Non_Regex lexer

### Build
```bash
g++ -std=c++17 Non_Regex/main.cpp Non_Regex/lexer.cpp Non_Regex/token.cpp -o Non_Regex/non_regex_demo
```

### Run
- With the built-in demo input (no args):
```bash
./Non_Regex/non_regex_demo
```

- With a source file (recommended):
```bash
./Non_Regex/non_regex_demo Non_Regex/sample.src
```

Tokens are printed without line/column in the console, but the lexer stores positions internally for error messages.

### Test cases
Comprehensive tests and error cases are provided under `Non_Regex/tests`.

- Run the comprehensive valid test:
```bash
./Non_Regex/non_regex_demo Non_Regex/tests/heavy_valid.src
```

- Error-focused tests (each should throw a clear error):
```bash
./Non_Regex/non_regex_demo Non_Regex/tests/err_invalid_identifier.src
./Non_Regex/non_regex_demo Non_Regex/tests/err_unterminated_string.src
./Non_Regex/non_regex_demo Non_Regex/tests/err_unterminated_char.src
./Non_Regex/non_regex_demo Non_Regex/tests/err_unterminated_block_comment.src
```

Notes:
- The lexer supports: keywords (fn, int, float, string, bool, char, if/elif/else, for/while/do, switch/case/default, return/break/continue, print/read, true/false/null/void/include), delimiters `()[]{}`, punctuation `, ; : . ?`, operators `+ - * / %`, assignments and compound assigns, comparisons, logical ops, bitwise ops, shifts, and string/char escapes. Single-line `//` and block `/* */` comments are skipped.
- Invalid identifiers that start with a digit (e.g., `123abc`) raise an error. Unterminated strings/chars/comments also raise errors.

---
## Regex lexer

Files: `Regex/main.cpp`, `Regex/lexer.{h,cpp}`, `Regex/token.{h,cpp}`

### Build
```bash
g++ -std=c++17 Regex/main.cpp Regex/lexer.cpp Regex/token.cpp -o Regex/regex_demo
```

### Run
The Regex lexer expects a source file path:
```bash
./Regex/regex_demo <path/to/source_file>
```
Example:
```bash
./Regex/regex_demo Non_Regex/sample.src
```

The Regex version prints tokens via `toString()` (includes type/value and may include position info depending on implementation).

---
## Troubleshooting
- “Could not open file”: Ensure you pass a valid path. Avoid leading `/` unless it’s an absolute path that exists.
- Multiple binaries: If you’ve built to both repo root and subfolders previously, prefer the ones under `Non_Regex/` and `Regex/` per the commands above.
- Locale/encoding: Files should be UTF-8 without BOM for predictable behavior.

---
## Quick commands recap
```bash
# Build both
g++ -std=c++17 Non_Regex/main.cpp Non_Regex/lexer.cpp Non_Regex/token.cpp -o Non_Regex/non_regex_demo
g++ -std=c++17 Regex/main.cpp Regex/lexer.cpp Regex/token.cpp -o Regex/regex_demo

# Run Non_Regex
./Non_Regex/non_regex_demo                    # demo input
./Non_Regex/non_regex_demo Non_Regex/sample.src

# Run Regex
./Regex/regex_demo Non_Regex/sample.src
```
