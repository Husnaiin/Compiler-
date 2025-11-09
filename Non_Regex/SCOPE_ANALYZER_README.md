# Scope Analyzer Module

## Overview

The Scope Analyzer is a complete, production-ready module for performing scope analysis on the AST produced by the parser. It implements C-like scoping semantics, maintains symbol tables, reports scope-related errors, and annotates the AST for use by subsequent compiler passes (type checker, code generator).

## Features

- **Nested Block Scopes**: Supports C-like block scoping with proper nesting
- **Shadowing**: Inner scopes can shadow outer names (allowed)
- **Redefinition Detection**: Detects and reports redefinitions within the same scope
- **Function Handling**: Supports function declarations and definitions with forward reference
- **Error Reporting**: Comprehensive error reporting with location information
- **AST Annotation**: Annotates identifier and function call nodes with resolved symbol information
- **Debug Output**: Provides scope tree and symbol table visualization

## API

### Main Function

```cpp
ScopeAnalysisResult analyze_scopes(std::shared_ptr<ProgramNode> ast);
```

Returns a `ScopeAnalysisResult` containing:
- `annotated_ast`: The AST with identifier nodes annotated
- `scopes`: Map of all scopes (scope_id -> Scope)
- `symbols`: Map of all symbols (symbol_id -> SymbolInfo)
- `errors`: List of scope errors found

### Lookup Functions

```cpp
std::optional<SymbolInfo*> lookup_in_env(const std::string& name, ScopeId scope_id);
std::optional<SymbolInfo*> resolve_ident(const std::string& name, const SourceLocation& loc);
```

### Debug Functions

```cpp
void dump_scope_tree() const;      // Print scope tree structure
void dump_symbol_table() const;     // Print all symbols
void print_scope_tree() const;      // Alias for dump_scope_tree
```

## Error Types

The analyzer reports the following errors:

1. **UndeclaredVariableAccessed**: Variable is used but not declared in any accessible scope
2. **UndefinedFunctionCalled**: Function is called but never declared/defined
3. **VariableRedefinition**: Variable is declared twice in the same scope
4. **FunctionPrototypeRedefinition**: Function is declared twice in the same scope

## Data Structures

### SymbolInfo
- `id`: Unique symbol identifier
- `name`: Symbol name
- `kind`: SymbolKind (GlobalVar, LocalVar, Parameter, Function, FunctionPrototype)
- `scope_id`: ID of the scope where declared
- `location`: Source location (line, column)
- `type_name`: Type placeholder (for type checker)
- `is_defined`: For functions, whether full definition exists
- `decl_node`: Pointer to declaration AST node

### Scope
- `id`: Unique scope identifier
- `parent_id`: Optional parent scope ID
- `kind`: ScopeKind (Global, Function, Block)
- `symbols`: Map of names to symbol IDs declared in this scope
- `start_location`, `end_location`: Source locations

## Scoping Rules

1. **Global Scope**: Top-level declarations are visible throughout the file
2. **Function Scope**: Function parameters are part of the function's scope
3. **Block Scope**: Every `{ ... }` introduces a new scope
4. **Shadowing**: Inner scopes can shadow outer names (allowed)
5. **Redefinition**: Declaring the same name twice in the same scope is an error
6. **Forward References**: Functions can be called before they are defined (forward reference support)

## Implementation Details

### Scope Stack

The analyzer uses a scope stack to track the current scope chain. When entering a function or block, a new scope is pushed. When exiting, it's popped.

### Symbol Lookup

Lookup walks up the scope chain from the current scope to parent scopes until the symbol is found or the global scope is reached.

### AST Annotation

The analyzer maintains a separate annotation map (`annotations_`) that maps AST node pointers to their resolved symbol information. This allows the type checker and code generator to query which symbol an identifier refers to.

## Usage Example

```cpp
#include "scope_analyzer.h"

// ... after parsing ...
ScopeAnalyzer analyzer;
ScopeAnalysisResult result = analyzer.analyze_scopes(ast);

if (result.hasErrors()) {
    // Handle errors
    for (const auto& error : result.errors) {
        std::cout << error.message << std::endl;
    }
}

// Access annotations
auto annotation = analyzer.get_annotation(identifier_node);
if (annotation.has_value() && annotation->is_resolved) {
    SymbolId symbol_id = annotation->symbol_id;
    // Use symbol_id to look up in result.symbols
}
```

## Test Cases

The module includes several test files:

- `test_scope_undeclared.txt`: Tests undeclared variable access
- `test_scope_shadowing.txt`: Tests variable shadowing (should pass)
- `test_scope_redefinition.txt`: Tests variable redefinition error
- `test_scope_function_redef.txt`: Tests function redefinition error
- `test_scope_undefined_func.txt`: Tests undefined function call error
- `test_scope_params_locals.txt`: Tests parameter and local variable resolution
- `test_scope_nested_blocks.txt`: Tests nested block visibility
- `test_scope_valid.txt`: Valid program with multiple scopes

## Integration with Compiler Pipeline

The scope analyzer fits into the compiler pipeline as follows:

```
Lexer → Parser → Scope Analyzer → Type Checker → Code Generator
```

The scope analyzer:
1. Takes the AST from the parser
2. Performs scope analysis and error checking
3. Annotates the AST with symbol information
4. Returns the annotated AST and symbol table for the type checker

## Design Choices

- **Spaghetti Stack**: Uses a scope stack (vector) for efficient lookup
- **Separate Annotation Map**: AST nodes are not modified; annotations are stored separately
- **Best-Effort Analysis**: Continues analysis after errors to find all issues
- **Deterministic**: Errors are reported in a consistent order

## Future Enhancements

- Support for function prototypes (separate from definitions)
- Better source location tracking (store locations in AST nodes)
- Support for built-in functions
- Module/file-level scoping for multi-file programs


