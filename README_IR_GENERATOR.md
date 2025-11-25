# Compiler with IR Generator - Quick Start Guide

## Overview

A complete compiler implementation featuring:
- ✅ Lexical Analysis
- ✅ Syntax Analysis (Parser)
- ✅ Semantic Analysis (Scope + Type Checking)
- ✅ **IR Generation (Three-Address Code)** ← NEW!
- ✅ Proper break/continue validation
- ✅ Comprehensive error handling

## Quick Start

### Build
```bash
cd Non_Regex
make -f Makefile_IR
```

### Run
```bash
# Compile a program
./compiler ../tests/simple_valid.src

# Generate IR only
./ir_demo ../tests/ir_test_simple.src output.tac
```

## Project Structure

```
Compiler-/
├── Non_Regex/
│   ├── lexer.cpp/.h              # Tokenization
│   ├── parser.cpp/.h             # AST generation
│   ├── scope_analyzer.cpp/.h     # Scope checking
│   ├── type_checker.cpp/.h       # Type checking
│   ├── ir_generator.cpp/.h       # IR generation (NEW)
│   ├── main.cpp                  # Main compiler
│   ├── ir_demo.cpp               # IR demo (NEW)
│   ├── Makefile_IR               # Build system
│   └── *.md                      # Documentation
└── tests/
    ├── simple_valid.src          # Basic test
    ├── ir_test_simple.src        # IR test
    └── ir_test_comprehensive.src # Full IR test
```

## Build Targets

```bash
make -f Makefile_IR              # Build all
make -f Makefile_IR compiler     # Main compiler only
make -f Makefile_IR ir_demo      # IR demo only
make -f Makefile_IR clean        # Clean artifacts
make -f Makefile_IR test         # Run basic test
make -f Makefile_IR test-ir      # Run IR tests
make -f Makefile_IR test-all     # Run all tests
make -f Makefile_IR help         # Show help
```

## Example Usage

### Input Program (`test.src`)
```c
int x = 10;
int y = 20;

function int add(int a, int b) {
    return a + b;
}

function int main() {
    int result = add(x, y);
    
    if (result > 25) {
        print result;
    } else {
        print 0;
    }
    
    return 0;
}
```

### Compile
```bash
./compiler test.src
```

### Generated TAC (`test.tac`)
```
# Global Variables
.global int x
.global int y

# Function: add (returns int)
.function add
# Parameters: a, b
    t0 = a + b
    return t0
.end_function

# Function: main (returns int)
.function main
    param x
    param y
    t0 = call add, 2
    result = t0
    t1 = result > 25
    if (!t1) goto L0
    print result
    goto L1
L0:
    print 0
L1:
    return 0
.end_function
```

## TAC Instructions

### Arithmetic
```
t0 = a + b    # ADD
t1 = a - b    # SUB
t2 = a * b    # MUL
t3 = a / b    # DIV
t4 = a % b    # MOD
t5 = -a       # NEG
```

### Control Flow
```
L1:                    # LABEL
goto L2                # GOTO
if (!cond) goto L3     # IF_FALSE
if (cond) goto L4      # IF_TRUE
```

### Functions
```
param x                # PARAM
t0 = call func, 2      # CALL
return t0              # RETURN
return                 # RETURN_VOID
```

### Comparison
```
t0 = a == b    # EQ
t1 = a != b    # NE
t2 = a < b     # LT
t3 = a <= b    # LE
t4 = a > b     # GT
t5 = a >= b    # GE
```

## Language Features

### Data Types
- `int`, `float`, `char`, `string`, `bool`
- Pointers (`int*`, `float*`, etc.)
- Arrays

### Control Structures
- `if-else`
- `while`, `for`, `do-while`
- `switch-case-default`
- `break`, `continue` (properly validated!)
- Ternary operator (`? :`)

### Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Logical: `&&`, `||`, `!`
- Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>`

## Key Features

### Break/Continue Validation
```cpp
// Valid
while (i < 10) {
    if (i == 5) break;
    i++;
}

// Invalid - ERROR!
if (x > 0) {
    break;  // Not in a loop!
}
```

### Short-Circuit Evaluation
```c
if (a && b) { }  // If a is false, b is NOT evaluated

// Generated TAC:
t0 = a
if (!t0) goto L1
    t0 = b
L1:
```

### Type Checking
```c
int x = 10;
float y = 3.14;
int z = x + y;  // OK - implicit cast float→int

bool b = x + y;  // ERROR - can't assign numeric to bool
```

## Documentation

- `COMPILER_SUMMARY.md` - Complete overview
- `IR_GENERATOR_README.md` - TAC format details
- `SCOPE_ANALYZER_README.md` - Scope checking
- `TYPE_CHECKER_README.md` - Type system
- `BNF_GRAMMAR.md` - Language grammar

## Testing

### Test Files
- `tests/simple_valid.src` - Basic valid program
- `tests/valid_types.src` - Type checking
- `tests/ir_test_simple.src` - Simple IR test
- `tests/comprehensive_test.src` - Full test

### Run Tests
```bash
# Single test
./compiler ../tests/simple_valid.src

# IR generation test
./ir_demo ../tests/ir_test_simple.src

# All tests
make -f Makefile_IR test-all
```

## Error Detection

The compiler detects and reports:

### Lexical Errors
- Invalid characters
- Unterminated strings/comments

### Syntax Errors
- Missing semicolons
- Mismatched braces
- Invalid syntax

### Semantic Errors
- Undeclared variables
- Type mismatches
- **Break/continue outside loops** ✅
- Function signature mismatches

### IR Generation Errors
- Invalid expressions
- Type incompatibilities

## Next Steps (Bonus Features)

### Bonus A: Assembler
Generate native code for:
- **MIPS** - Classic RISC
- **ARM** - Modern embedded
- **RISC-V** - Open-source ISA
- **x86** - Desktop/server

### Bonus B: LLVM/QBE Backend
- Convert TAC → LLVM IR
- Generate executable binaries
- Use QBE lightweight backend

## Compilation Pipeline

```
Source Code
    ↓
[Lexer] → Tokens
    ↓
[Parser] → AST
    ↓
[Scope Analyzer] → Symbol Table
    ↓
[Type Checker] → Type-checked AST
    ↓
[IR Generator] → TAC (YOU ARE HERE)
    ↓
[Optimizer] → Optimized TAC (future)
    ↓
[Code Generator] → Assembly (future)
    ↓
Executable
```

## System Requirements

- C++17 or later
- g++ or clang++
- Make
- Linux/macOS/WSL

## Learning Resources

Read the documentation in order:
1. `COMPILER_SUMMARY.md` - Start here
2. `BNF_GRAMMAR.md` - Language syntax
3. `SCOPE_ANALYZER_README.md` - Scope rules
4. `TYPE_CHECKER_README.md` - Type system
5. `IR_GENERATOR_README.md` - TAC format

## Contributing

Potential improvements:
- More optimization passes
- Better error messages
- Additional language features
- Backend code generators
- Standard library

## Example Session

```bash
$ cd Non_Regex
$ make -f Makefile_IR
g++ -std=c++17 -Wall -Wextra -g -c lexer.cpp -o lexer.o
...
g++ -std=c++17 -Wall -Wextra -g -o compiler ...

$ ./compiler ../tests/simple_valid.src
=== STEP 1: LEXICAL ANALYSIS ===
Tokenization completed. Total tokens: 124

=== STEP 2: SYNTAX ANALYSIS ===
Parsing completed successfully.

=== STEP 3: SEMANTIC ANALYSIS - SCOPE CHECKING ===
Scope analysis completed successfully.

=== STEP 4: SEMANTIC ANALYSIS - TYPE CHECKING ===
Type checking completed successfully.

=== STEP 5: IR GENERATION ===
IR generation completed successfully.

--- Three-Address Code (TAC) ---
[... TAC output ...]

IR code saved to: ../tests/simple_valid.tac

COMPILATION SUCCESSFUL
```

## Success!

Your compiler now generates Three-Address Code (TAC) - a clean, platform-independent intermediate representation ready for optimization and code generation!

---

**Status**: IR Generation Complete  
**Date**: November 2025  
**Next**: Optimization & Backend
