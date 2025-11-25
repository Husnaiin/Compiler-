# Intermediate Representation (IR) Generator Documentation

## Overview

The IR Generator is a compiler phase that translates the Abstract Syntax Tree (AST) into Three-Address Code (TAC), a low-level intermediate representation that is easy to optimize and translate to machine code.

## Architecture

### Components

1. **TACOperand**: Represents operands (variables, temporaries, constants, labels)
2. **TACInstruction**: Represents a single TAC instruction
3. **IRFunction**: Collection of TAC instructions for a function
4. **IRProgram**: Complete IR program with functions and global data
5. **IRGenerator**: Main class that performs the translation

## Three-Address Code (TAC) Format

TAC is a linear sequence of instructions where each instruction has at most three operands. The format is:

```
dest = src1 op src2
```

### Instruction Types

#### 1. Assignment and Copy
- `COPY`: `dest = src`
  - Example: `t0 = x`

#### 2. Arithmetic Operations
- `ADD`: `dest = src1 + src2`
- `SUB`: `dest = src1 - src2`
- `MUL`: `dest = src1 * src2`
- `DIV`: `dest = src1 / src2`
- `MOD`: `dest = src1 % src2`
- `NEG`: `dest = -src1`

Example:
```
t0 = a + b
t1 = t0 * c
result = t1 - 10
```

#### 3. Bitwise Operations
- `AND`: `dest = src1 & src2`
- `OR`: `dest = src1 | src2`
- `XOR`: `dest = src1 ^ src2`
- `NOT`: `dest = ~src1`
- `SHL`: `dest = src1 << src2` (left shift)
- `SHR`: `dest = src1 >> src2` (right shift)

#### 4. Logical Operations
- `LAND`: `dest = src1 && src2` (logical AND)
- `LOR`: `dest = src1 || src2` (logical OR)
- `LNOT`: `dest = !src1` (logical NOT)

#### 5. Comparison Operations
- `EQ`: `dest = src1 == src2`
- `NE`: `dest = src1 != src2`
- `LT`: `dest = src1 < src2`
- `LE`: `dest = src1 <= src2`
- `GT`: `dest = src1 > src2`
- `GE`: `dest = src1 >= src2`

#### 6. Control Flow
- `LABEL`: `label:`
- `GOTO`: `goto label`
- `IF_FALSE`: `if (!condition) goto label`
- `IF_TRUE`: `if (condition) goto label`

Example (if-else):
```
t0 = x > 10
if (!t0) goto L1
    # then block
    print x
    goto L2
L1:
    # else block
    print 0
L2:
```

#### 7. Function Operations
- `PARAM`: `param arg` (push function argument)
- `CALL`: `dest = call func, num_args`
- `RETURN`: `return src`
- `RETURN_VOID`: `return`

Example (function call):
```
param a
param b
t0 = call add, 2
result = t0
```

#### 8. Array Operations
- `ARRAY_LOAD`: `dest = arr[index]`
- `ARRAY_STORE`: `arr[index] = src`
- `ARRAY_ADDR`: `dest = &arr[index]`

#### 9. Pointer Operations
- `ADDR`: `dest = &src` (address-of)
- `DEREF`: `dest = *src` (dereference)
- `STORE`: `*dest = src` (store through pointer)

#### 10. Special Operations
- `PRINT`: `print src`
- `NOP`: No operation

## Translation Examples

### Example 1: Simple Expression

**Source Code:**
```c
int x = 5;
int y = 10;
int z = x + y * 2;
```

**Generated TAC:**
```
x = 5
y = 10
t0 = y * 2
t1 = x + t0
z = t1
```

### Example 2: If Statement

**Source Code:**
```c
if (x > 10) {
    y = 20;
} else {
    y = 30;
}
```

**Generated TAC:**
```
t0 = x > 10
if (!t0) goto L1
    y = 20
    goto L2
L1:
    y = 30
L2:
```

### Example 3: While Loop

**Source Code:**
```c
int i = 0;
while (i < 5) {
    print i;
    i = i + 1;
}
```

**Generated TAC:**
```
i = 0
L1:
    t0 = i < 5
    if (!t0) goto L2
    print i
    t1 = i + 1
    i = t1
    goto L1
L2:
```

### Example 4: For Loop

**Source Code:**
```c
for (int i = 0; i < 10; i = i + 1;) {
    print i;
}
```

**Generated TAC:**
```
i = 0
L1:
    t0 = i < 10
    if (!t0) goto L3
    print i
L2:
    t1 = i + 1
    i = t1
    goto L1
L3:
```

### Example 5: Function Call

**Source Code:**
```c
function int add(int a, int b) {
    return a + b;
}

int result = add(5, 10);
```

**Generated TAC:**
```
# Function: add
.function add
    t0 = a + b
    return t0
.end_function

# Main code
param 5
param 10
t1 = call add, 2
result = t1
```

### Example 6: Switch Statement

**Source Code:**
```c
switch (x) {
    case 1:
        print 10;
        break;
    case 2:
        print 20;
        break;
    default:
        print 0;
}
```

**Generated TAC:**
```
t0 = x == 1
if (t0) goto L1
t1 = x == 2
if (t1) goto L2
goto L3
L1:
    print 10
    goto L4
L2:
    print 20
    goto L4
L3:
    print 0
L4:
```

### Example 7: Ternary Operator

**Source Code:**
```c
int max = a > b ? a : b;
```

**Generated TAC:**
```
t0 = a > b
if (!t0) goto L1
    t1 = a
    goto L2
L1:
    t1 = b
L2:
max = t1
```

### Example 8: Short-Circuit Evaluation

**Source Code:**
```c
if (a && b) {
    print 1;
}
```

**Generated TAC:**
```
t0 = a
if (!t0) goto L1
    t0 = b
L1:
if (!t0) goto L2
    print 1
L2:
```

## Operand Types

1. **TEMP**: Temporary variables (t0, t1, t2, ...)
   - Generated for intermediate results
   - Lifetime limited to expression evaluation

2. **VAR**: Named variables
   - User-defined variables
   - Parameters and local variables

3. **CONST**: Constant values
   - Integer literals (42)
   - Float literals (3.14)
   - String literals ("hello")
   - Boolean literals (true/false)

4. **LABEL**: Control flow labels
   - Branch targets (L0, L1, L2, ...)
   - Function entry points

5. **FUNC**: Function names
   - Used in function call instructions

## Error Handling

The IR generator detects and reports the following errors:

1. **InvalidExpression**: Malformed expression in AST
2. **InvalidStatement**: Malformed statement in AST
3. **UndefinedVariable**: Variable not declared in scope
4. **UndefinedFunction**: Function not declared
5. **TypeMismatch**: Incompatible types in operation
6. **InvalidOperand**: Invalid operand for operation
7. **UnreachableCode**: Code after return/break/continue
8. **InvalidArrayAccess**: Invalid array indexing
9. **InvalidPointerOperation**: Invalid pointer dereference/address-of
10. **MissingReturnValue**: Non-void function missing return
11. **InvalidLValue**: Assignment to non-lvalue
12. **DivisionByZero**: Compile-time division by zero

## Usage

### Command Line

```bash
# Compile with IR generation
./compiler input.src

# IR output will be saved to input.tac
```

### Programmatic Usage

```cpp
#include "ir_generator.h"

// After parsing and semantic analysis
IRGenerator generator;
IRGenResult result = generator.generate(ast, scope_result, type_result);

if (result.success) {
    std::cout << result.program.toString();
}
```

## Optimization Opportunities

The TAC format enables various optimizations:

1. **Constant Folding**: Evaluate constant expressions at compile time
2. **Dead Code Elimination**: Remove unreachable code
3. **Common Subexpression Elimination**: Reuse computed values
4. **Copy Propagation**: Replace copies with direct references
5. **Register Allocation**: Assign temporaries to registers
6. **Strength Reduction**: Replace expensive operations with cheaper ones
7. **Loop Optimization**: Induction variable elimination, loop unrolling

## Next Steps: Backend Integration

The TAC can be used as input for:

1. **Interpreter**: Direct execution of TAC instructions
2. **Native Code Generator**: Translate to assembly (MIPS, ARM, x86, RISC-V)
3. **LLVM Backend**: Convert TAC to LLVM IR
4. **QBE Backend**: Use QBE compiler backend
5. **Virtual Machine**: Execute on custom VM

### Example: LLVM Integration

```cpp
// Convert TAC to LLVM IR
LLVMContext context;
Module module("my_module", context);
IRBuilder<> builder(context);

// Translate each TAC instruction to LLVM IR
for (const auto& func : tac_program.functions) {
    Function* llvm_func = createLLVMFunction(func);
    for (const auto& instr : func.instructions) {
        translateToLLVM(instr, builder);
    }
}

// Generate object file
TargetMachine->emit(module, dest, CodeGenFileType::ObjectFile);
```

## Compilation Pipeline

```
Source Code
    ↓
[Lexer] → Tokens
    ↓
[Parser] → AST
    ↓
[Scope Analyzer] → Annotated AST + Symbol Table
    ↓
[Type Checker] → Type-checked AST
    ↓
[IR Generator] → TAC (Intermediate Representation)  ← YOU ARE HERE
    ↓
[Optimizer] → Optimized TAC
    ↓
[Code Generator] → Assembly/Machine Code
    ↓
[Assembler/Linker] → Executable
```

## Benefits of TAC

1. **Platform Independent**: Easy to target multiple architectures
2. **Simple Structure**: Easy to analyze and optimize
3. **Explicit Temporaries**: Makes data flow explicit
4. **Linear Format**: Straightforward control flow representation
5. **Extensible**: Easy to add new instructions
6. **Debuggable**: Human-readable format

## File Format

TAC output is saved with `.tac` extension:

```
# Global Variables
.global int x
.global int y

# String Literals
.STR0: .string "Hello, World!"

# Function: main (returns int)
.function main
# Parameters: 
    x = 10
    y = 20
    t0 = x + y
    result = t0
    print result
    return 0
.end_function
```

## Advanced Features

### 1. Nested Functions (Future)
Support for nested function scopes with proper closure handling.

### 2. Exception Handling (Future)
TAC instructions for try-catch-finally blocks.

### 3. Parallel Constructs (Future)
Instructions for parallel/concurrent execution.

### 4. Debug Information
Annotate TAC with source location mapping for debugging.

## References

- "Compilers: Principles, Techniques, and Tools" (Dragon Book)
- "Modern Compiler Implementation in C/Java/ML" (Tiger Book)
- "Engineering a Compiler" by Cooper & Torczon
- LLVM Documentation: https://llvm.org/docs/
- QBE Compiler Backend: https://c9x.me/compile/
