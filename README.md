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
git clone https://github.com/Husnaiin/Compiler-.git
cd Compiler-
```

---
## Directory layout (high-level)
```
Compiler-/
├─ Non_Regex/           # Lexer, Parser, Scope Analyzer, Type Checker, IR (C++17)
│  ├─ lexer.{h,cpp}
│  ├─ token.{h,cpp}
│  ├─ parser.{h,cpp}
│  ├─ scope_analyzer.{h,cpp}
│  ├─ type_checker.{h,cpp}
│  ├─ ir_generator.{h,cpp}
│  ├─ llvm_backend.{h,cpp}     # LLVM IR (.ll) emission from TAC
│  ├─ riscv_backend.{h,cpp}    # RISC-V assembly (.s) emission from TAC
│  └─ main.cpp          # Entry: tokens → AST → scope → types → IR → LLVM/RISCV
├─ Regex/               # Regex-based lexer variant
│  ├─ lexer.{h,cpp}
│  ├─ token.{h,cpp}
│  └─ main.cpp
├─ tests/               # Test programs (including scope analysis tests)
└─ README.md
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
Comprehensive tests and error cases are provided under the shared `tests/` directory (used by both lexers).

- Run the comprehensive valid test:
```bash
./Non_Regex/non_regex_demo tests/heavy_valid.src
```

- Error-focused tests (each should throw a clear error):
```bash
./Non_Regex/non_regex_demo tests/err_invalid_identifier.src
./Non_Regex/non_regex_demo tests/err_unterminated_string.src
./Non_Regex/non_regex_demo tests/err_unterminated_char.src
./Non_Regex/non_regex_demo tests/err_unterminated_block_comment.src
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
Examples:
```bash
./Regex/regex_demo Non_Regex/sample.src
./Regex/regex_demo tests/heavy_valid.src
```

The Regex version prints tokens via `toString()` (includes type/value and may include position info depending on implementation).

---
## Scope Analyzer (Non_Regex)

Scope analysis is implemented only in the `Non_Regex/` directory. It consumes the AST built by the `Non_Regex` parser and produces:
- a scope tree (global/function/block scopes),
- a symbol table (variables, parameters, functions),
- annotations on AST nodes for later passes,
- and scope-related errors (undeclared use, redefinitions, undefined function call).

### Build
Build a single binary that runs Lexer → Parser → Scope Analyzer → Type Checker → IR → LLVM IR/RISC-V:
```bash
mkdir -p build
g++ -std=c++17 -O2 \
  Non_Regex/lexer.cpp \
  Non_Regex/token.cpp \
  Non_Regex/parser.cpp \
  Non_Regex/scope_analyzer.cpp \
  Non_Regex/type_checker.cpp \
  Non_Regex/ir_generator.cpp \
  Non_Regex/llvm_backend.cpp \
  Non_Regex/riscv_backend.cpp \
  Non_Regex/main.cpp \
  -o build/scope_analyzer
```

### Run on a single test
Tests now live under the top-level `tests/` folder:
```bash
./build/scope_analyzer tests/test_scope_valid.txt
```

### Run all scope tests
```bash
for f in tests/test_scope_*.txt; do
  echo "=== $f ==="
  ./build/scope_analyzer "$f"
  echo
done
```

Notes:
- If you run without an argument, the demo may default to a sample file. Prefer passing an explicit test file path under `tests/`.
- The scope analyzer currently targets the `Non_Regex` pipeline (there is no scope analysis under `Regex/`).

---
## Type Checker (Non_Regex)

The type checker runs immediately after scope analysis within the same executable. It validates:
- expression and assignment types,
- function call argument types vs parameter types,
- return types vs function return type,
- operator operand types and compatibility.

If scope errors are found, type checking is skipped.

### Build (included in unified build)
Use the same build command shown above (includes `type_checker.cpp`).

### Run
Same as scope analysis: pass a test file under `tests/`. Type checking runs automatically if scope analysis succeeds.

---
## IR Generator (Non_Regex)

After successful type checking, the IR generator emits Three-Address Code (TAC).

Artifacts:
- TAC printed to stdout.
- TAC saved next to the input as `<basename>.tac` (e.g., `tests/test_scope_valid.tac`).
- IR statistics (functions, globals, instruction count).

### Build (included in unified build)
Use the same build command (includes `ir_generator.cpp`).

### Run (generates TAC)
```bash
./build/scope_analyzer tests/test_scope_valid.txt
cat tests/test_scope_valid.tac
```

Notes:
- If scope/type errors are found, IR generation is skipped.
- The `.tac` file is overwritten on each run for the same input.

---
## LLVM IR Emission (Non_Regex)

After IR generation, the tool emits a textual LLVM IR module (.ll) next to your input file.

### Build (included in unified build)
Use the unified build above (includes `llvm_backend.cpp`).

### Run (produces .ll)
```bash
./build/scope_analyzer tests/test_scope_valid.txt
cat tests/test_scope_valid.ll
```

### Make an executable from .ll
- Method A (llvm-as + llc + cc):
```bash
llvm-as tests/test_scope_valid.ll -o tests/out.bc
llc tests/out.bc -filetype=obj -o tests/out.o
cc tests/out.o -o tests/a.out
tests/a.out
```
- Method B (clang directly):
```bash
clang tests/test_scope_valid.ll -o tests/a.out
tests/a.out
```

Notes:
- PRINT lowering requires a runtime stub or mapping to printf (planned enhancement).

---
## RISC-V Assembly Emission (Non_Regex)

The tool also emits RISC-V RV32I/M GNU assembly (.s) from TAC. This is useful for testing on a RISC-V toolchain or emulator.

### Build (included in unified build)
Use the unified build above (includes `riscv_backend.cpp`).

### Run (produces .s)
```bash
./build/scope_analyzer tests/test_scope_valid.txt
sed -n '1,120p' tests/test_scope_valid.s
```

### Assemble and link (requires RISC-V toolchain)
- With clang’s RISC-V target:
```bash
clang -target riscv32 -march=rv32im tests/test_scope_valid.s -o tests/a_rv32
```
- Or with a cross GCC:
```bash
riscv32-unknown-elf-gcc -march=rv32im tests/test_scope_valid.s -o tests/a_rv32
```

Notes:
- Current CALL lowering passes placeholder zeros in a0.. for args (argument threading from PARAM planned next).
- Array/pointer ops are stubbed; core arithmetic, comparisons, branches, returns, and copies are implemented.

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
./Non_Regex/non_regex_demo tests/heavy_valid.src

# Run Regex
./Regex/regex_demo Non_Regex/sample.src
./Regex/regex_demo tests/heavy_valid.src

# Build & run Scope Analyzer (Non_Regex)
mkdir -p build
g++ -std=c++17 -O2 Non_Regex/lexer.cpp Non_Regex/token.cpp Non_Regex/parser.cpp Non_Regex/scope_analyzer.cpp Non_Regex/type_checker.cpp Non_Regex/ir_generator.cpp Non_Regex/llvm_backend.cpp Non_Regex/riscv_backend.cpp Non_Regex/main.cpp -o build/scope_analyzer
./build/scope_analyzer tests/test_scope_valid.txt
for f in tests/test_scope_*.txt; do ./build/scope_analyzer "$f"; done
```
