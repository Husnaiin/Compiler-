# Compiler Implementation Summary

## Project Overview

This is a comprehensive compiler implementation with full support for lexical analysis, parsing, semantic analysis, type checking, and **intermediate representation (IR) generation** using Three-Address Code (TAC).

## Compilation Phases

### 1. Lexical Analysis (Tokenization)
- **Module**: `lexer.cpp`, `lexer.h`, `token.cpp`, `token.h`
- **Purpose**: Converts source code into a stream of tokens
- **Features**:
  - Integer, float, string, char, and boolean literals
  - Identifiers and keywords
  - Operators (arithmetic, logical, bitwise, comparison)
  - Comments (single-line and block)
  - Error handling for invalid characters

### 2. Syntax Analysis (Parsing)
- **Module**: `parser.cpp`, `parser.h`
- **Purpose**: Builds an Abstract Syntax Tree (AST) from tokens
- **Features**:
  - Recursive descent parsing
  - Support for expressions with proper precedence
  - Control flow statements (if, while, for, do-while, switch)
  - Functions with parameters and return values
  - Arrays and pointers
  - Ternary operator
  - Error recovery and synchronization

### 3. Semantic Analysis - Scope Checking
- **Module**: `scope_analyzer.cpp`, `scope_analyzer.h`
- **Purpose**: Validates variable and function declarations and usages
- **Features**:
  - Symbol table management with scopes
  - Nested scope support (global, function, block)
  - Variable redefinition detection
  - Undeclared variable/function detection
  - **Break/continue statement validation** (must be inside loops)
  - Function forward reference support

**Recent Improvements:**
- ✅ Added loop depth tracking to validate break/continue statements
- ✅ Switch statements properly allow break but not continue
- ✅ Error reporting for misplaced break/continue statements

### 4. Semantic Analysis - Type Checking
- **Module**: `type_checker.cpp`, `type_checker.h`
- **Purpose**: Ensures type safety and valid operations
- **Features**:
  - Type inference for expressions
  - Type compatibility checking
  - Implicit type conversion (int ↔ float)
  - Function return type validation
  - Array index type checking
  - Pointer type validation
  - Operator type compatibility
  - Ternary operator type checking

### 5. **Intermediate Representation (IR) Generation** ✨ NEW
- **Module**: `ir_generator.cpp`, `ir_generator.h`
- **Purpose**: Translates AST to Three-Address Code (TAC)
- **Format**: TAC - platform-independent intermediate representation
- **Features**:
  - 50+ TAC instruction types
  - Temporary variable generation
  - Label generation for control flow
  - Short-circuit evaluation for logical operators
  - Function call handling with parameters
  - Array and pointer operations
  - Comprehensive error handling

## Three-Address Code (TAC) Instructions

### Categories

1. **Assignment**: `COPY`
2. **Arithmetic**: `ADD`, `SUB`, `MUL`, `DIV`, `MOD`, `NEG`
3. **Bitwise**: `AND`, `OR`, `XOR`, `NOT`, `SHL`, `SHR`
4. **Logical**: `LAND`, `LOR`, `LNOT`
5. **Comparison**: `EQ`, `NE`, `LT`, `LE`, `GT`, `GE`
6. **Control Flow**: `LABEL`, `GOTO`, `IF_FALSE`, `IF_TRUE`
7. **Functions**: `PARAM`, `CALL`, `RETURN`, `RETURN_VOID`
8. **Arrays**: `ARRAY_LOAD`, `ARRAY_STORE`, `ARRAY_ADDR`
9. **Pointers**: `ADDR`, `DEREF`, `STORE`
10. **Special**: `PRINT`, `NOP`

### Example TAC Output

**Source Code:**
```c
int x = 10;
int y = 20;
int sum = x + y;
if (sum > 25) {
    print sum;
}
```

**Generated TAC:**
```
x = 10
y = 20
t0 = x + y
sum = t0
t1 = sum > 25
if (!t1) goto L0
    print sum
L0:
```

## Project Structure

```
Non_Regex/
├── lexer.cpp, lexer.h           # Lexical analyzer
├── token.cpp, token.h           # Token definitions
├── parser.cpp, parser.h         # Parser and AST
├── scope_analyzer.cpp, .h       # Scope checking
├── type_checker.cpp, .h         # Type checking
├── ir_generator.cpp, .h         # IR generation ✨ NEW
├── main.cpp                     # Main compiler
├── ir_demo.cpp                  # IR generation demo ✨ NEW
├── Makefile_IR                  # Build system ✨ NEW
├── BNF_GRAMMAR.md              # Language grammar
├── SCOPE_ANALYZER_README.md    # Scope docs
├── TYPE_CHECKER_README.md      # Type checking docs
└── IR_GENERATOR_README.md      # IR generation docs ✨ NEW
```

## Building the Compiler

### Prerequisites
- C++17 compatible compiler (g++, clang++)
- Make build system

### Build Commands

```bash
# Build all components
make -f Makefile_IR

# Build specific targets
make -f Makefile_IR compiler      # Main compiler
make -f Makefile_IR ir_demo       # IR demo

# Clean build artifacts
make -f Makefile_IR clean

# Run tests
make -f Makefile_IR test
make -f Makefile_IR test-ir
make -f Makefile_IR test-all
```

## Usage

### Main Compiler
```bash
./compiler <source_file>

# Example
./compiler ../tests/simple_valid.src

# Output: Generates <source_file>.tac with IR code
```

### IR Demo
```bash
./ir_demo <source_file> [output_file]

# Example
./ir_demo ../tests/ir_test_simple.src output.tac
```

## Test Files

- `tests/simple_valid.src` - Basic valid program
- `tests/valid_types.src` - Type checking tests
- `tests/ir_test_simple.src` - Simple IR generation test
- `tests/ir_test_comprehensive.src` - Comprehensive IR test
- `tests/*_errors.src` - Various error test cases

## Language Features

### Data Types
- `int` - Integer numbers
- `float` - Floating-point numbers
- `char` - Single characters
- `string` - String literals
- `bool` - Boolean values (true/false)
- Pointers (int*, float*, etc.)
- Arrays

### Control Structures
- `if-else` statements
- `while` loops
- `for` loops
- `do-while` loops
- `switch-case` statements with default
- `break` and `continue` statements ✅ (properly validated)
- Ternary operator (`? :`)

### Functions
- Function declarations with return types
- Parameters with types
- Return statements
- Recursive functions
- Forward references

### Operators

**Arithmetic:** `+`, `-`, `*`, `/`, `%`, unary `-`
**Comparison:** `==`, `!=`, `<`, `<=`, `>`, `>=`
**Logical:** `&&`, `||`, `!`
**Bitwise:** `&`, `|`, `^`, `~`, `<<`, `>>`
**Assignment:** `=`
**Other:** `? :` (ternary)

## Error Handling

### Lexical Errors
- Invalid characters
- Unterminated strings
- Unterminated comments
- Invalid number formats

### Syntax Errors
- Unexpected tokens
- Missing semicolons
- Mismatched parentheses/braces
- Invalid function declarations

### Semantic Errors - Scope
- Undeclared variables
- Undefined functions
- Variable redefinition
- Function redefinition
- **Invalid break/continue placement** ✅

### Semantic Errors - Type
- Type mismatches in operations
- Invalid function arguments
- Invalid return types
- Array index not integer
- Invalid pointer operations
- Non-boolean conditions

### IR Generation Errors
- Invalid expressions
- Undefined variables/functions
- Type incompatibilities
- Invalid operands

## Compilation Pipeline

```
Source Code
    ↓
[Lexer] → Tokens
    ↓
[Parser] → Abstract Syntax Tree (AST)
    ↓
[Scope Analyzer] → Annotated AST + Symbol Table
    ↓
[Type Checker] → Type-checked AST
    ↓
[IR Generator] → Three-Address Code (TAC) ✨
    ↓
[Optimizer] → Optimized TAC (future)
    ↓
[Code Generator] → Assembly/Machine Code (future)
    ↓
[Assembler/Linker] → Executable (future)
```

## Key Implementation Highlights

### Break/Continue Validation ✅
The scope analyzer now properly tracks loop context:
```cpp
int loop_depth_;  // Tracks nesting level

// Increment when entering loops
loop_depth_++;
// ... analyze loop body
loop_depth_--;

// Validate break/continue
if (loop_depth_ <= 0) {
    report_error("Break/continue outside of loop");
}
```

### TAC Generation
Clean separation of concerns:
```cpp
TACOperand generate_expression(ExpressionNode* expr);
void generate_statement(StatementNode* stmt);
void emit(TACOpcode opcode, TACOperand dest, TACOperand src1, TACOperand src2);
```

### Short-Circuit Evaluation
Logical operators use proper short-circuit evaluation:
```cpp
// For a && b
evaluate(a);
if (!a) goto end;
evaluate(b);
end:
```

## Future Enhancements

### Immediate Next Steps
1. ✅ IR Generator (COMPLETED)
2. IR Optimization passes
3. Backend code generation

### Bonus Features (As Requested)

#### Bonus A: Assembler Implementation
Target options:
- **MIPS**: Classic RISC architecture
- **ARM**: Modern embedded/mobile
- **RISC-V**: Open-source ISA
- **x86**: Desktop/server

#### Bonus B: LLVM/QBE Backend
- **LLVM**: Convert TAC → LLVM IR → Machine code
- **QBE**: Lightweight compiler backend
- Generate executable binaries

### Advanced Features
- Function pointers
- Structs/Records
- Dynamic memory allocation
- Standard library functions
- Preprocessor directives
- Module system

## Performance Considerations

### Current Implementation
- **Time Complexity**: O(n) for most phases
- **Space Complexity**: O(n) for AST and symbol tables
- **TAC Generation**: Linear with AST size

### Optimization Opportunities
1. Constant folding
2. Dead code elimination
3. Common subexpression elimination
4. Copy propagation
5. Register allocation
6. Instruction scheduling

## Testing

### Test Coverage
- ✅ Lexical analysis
- ✅ Syntax analysis
- ✅ Scope checking
- ✅ Type checking
- ✅ IR generation
- ✅ Break/continue validation
- ✅ Control flow structures
- ✅ Function calls
- ✅ Expressions with operators

### Running Tests
```bash
# Run all tests
make -f Makefile_IR test-all

# Run specific tests
./compiler ../tests/simple_valid.src
./ir_demo ../tests/ir_test_simple.src
```

## Documentation

Comprehensive documentation available:
- `BNF_GRAMMAR.md` - Language grammar specification
- `SCOPE_ANALYZER_README.md` - Scope analysis details
- `TYPE_CHECKER_README.md` - Type checking guide
- `IR_GENERATOR_README.md` - IR generation guide ✨

## Conclusion

This compiler implementation provides a solid foundation for language processing with:
- ✅ Complete lexical and syntax analysis
- ✅ Robust semantic analysis with proper scope and type checking
- ✅ **Break/continue statement validation in scope analyzer**
- ✅ **Three-Address Code (TAC) intermediate representation**
- ✅ Comprehensive error handling
- ✅ Clean, modular architecture
- ✅ Extensive documentation

The IR generation module enables:
- Platform-independent code representation
- Easy optimization and transformation
- Multiple backend targets (LLVM, QBE, native assemblers)
- Clear separation between frontend and backend

## Credits

Developed as a comprehensive compiler implementation project demonstrating modern compiler design principles and techniques.

---

**Last Updated**: November 2025  
**Status**: IR Generation Complete ✅  
**Next Phase**: Optimization & Code Generation
