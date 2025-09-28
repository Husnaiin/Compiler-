# **TERMINAL SYMBOLS**

### **Keywords**
```
KEYWORDS ::= "function" | "int" | "float" | "string" | "char" | "bool" | "void"
           | "if" | "else" | "elif" | "while" | "for" | "do"
           | "switch" | "case" | "default"
           | "return" | "break" | "continue"
           | "print" | "read"
           | "true" | "false" | "null"
           | "include"
```

### **Operators**
```
ARITHMETIC_OPERATORS ::= "+" | "-" | "*" | "/" | "%"
RELATIONAL_OPERATORS ::= "==" | "!=" | ">" | "<" | ">=" | "<="
LOGICAL_OPERATORS ::= "&&" | "||" | "!"
BITWISE_OPERATORS ::= "&" | "|" | "^" | "~" | "<<" | ">>"
ASSIGNMENT_OPERATORS ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" 
                        | "&=" | "|=" | "^=" | "<<=" | ">>="
UNARY_OPERATORS ::= "++" | "--" | "-" | "!" | "~" | "*" | "&"
TERNARY_OPERATORS ::= "?" | ":"
```

### **Punctuators**
```
PUNCTUATORS ::= "(" | ")" | "{" | "}" | "[" | "]" | ";" | "," | ":" | "." | "?"
```

### **Literals**
```
INT_LITERAL ::= [0-9]+ | 0[xX][0-9a-fA-F]+ | 0[oO][0-7]+ | 0[bB][01]+
FLOAT_LITERAL ::= [0-9]+\.[0-9]+([eE][+-]?[0-9]+)?
STRING_LITERAL ::= "([^"\\]|\\.)*"
CHAR_LITERAL ::= '([^'\\]|\\.)'
BOOL_LITERAL ::= "true" | "false"
NULL_LITERAL ::= "null"
IDENTIFIER ::= [a-zA-Z_][a-zA-Z0-9_]*
```

---

## **NON-TERMINAL SYMBOLS**

### **1. Program Structure**
```
<program> ::= (<global_declaration> | <function_declaration>)*

<global_declaration> ::= <variable_declaration>
                      | <array_declaration>
                      | <pointer_declaration>
```

### **2. Function Declarations**
```
<function_declaration> ::= <function_keyword> <identifier> "(" <parameter_list> ")" <block>
                         | <type> <identifier> "(" <parameter_list> ")" <block>

<function_keyword> ::= "function"

<parameter_list> ::= <parameter> ("," <parameter>)*
                   | ε

<parameter> ::= <type> <identifier>
```

### **3. Type System**
```
<type> ::= <base_type> <type_modifier>*

<base_type> ::= "int" | "float" | "string" | "char" | "bool" | "void"

<type_modifier> ::= "*"  // pointer
                  | "[" <expression>? "]"  // array
```

### **4. Variable Declarations**
```
<variable_declaration> ::= <type> <identifier> ("=" <expression>)? ";"

<array_declaration> ::= <type> <identifier> "[" <expression>? "]" ("=" <array_initializer>)? ";"

<pointer_declaration> ::= <base_type> "*" <identifier> ("=" <expression>)? ";"

<array_initializer> ::= "{" <expression_list>? "}"

<expression_list> ::= <expression> ("," <expression>)*
```

### **5. Statements**
```
<statement> ::= <variable_declaration>
              | <array_declaration>
              | <pointer_declaration>
              | <assignment_statement>
              | <if_statement>
              | <while_statement>
              | <for_statement>
              | <do_while_statement>
              | <switch_statement>
              | <return_statement>
              | <break_statement>
              | <continue_statement>
              | <print_statement>
              | <block>
              | <expression_statement>

<assignment_statement> ::= <identifier> <assignment_operator> <expression> ";"

<assignment_operator> ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" 
                        | "&=" | "|=" | "^=" | "<<=" | ">>="

<expression_statement> ::= <expression> ";"
```

### **6. Control Flow Statements**
```
<if_statement> ::= "if" "(" <expression> ")" <statement> ("else" <statement>)?

<while_statement> ::= "while" "(" <expression> ")" <statement>

<for_statement> ::= "for" "(" <for_init>? ";" <expression>? ";" <for_update>? ")" <statement>

<for_init> ::= <variable_declaration> | <expression>

<for_update> ::= <expression>

<do_while_statement> ::= "do" <statement> "while" "(" <expression> ")" ";"

<switch_statement> ::= "switch" "(" <expression> ")" "{" <case_list>? <default_case>? "}"

<case_list> ::= <case_statement>+

<case_statement> ::= "case" <expression> ":" <statement_list>?

<default_case> ::= "default" ":" <statement_list>?

<statement_list> ::= <statement>+
```

### **7. Expression Grammar (Complete Precedence Hierarchy)**

#### **Expression Precedence (Highest to Lowest)**
```
<expression> ::= <assignment_expression>

<assignment_expression> ::= <ternary_expression> (<assignment_operator> <assignment_expression>)?

<ternary_expression> ::= <logical_or_expression> ("?" <expression> ":" <ternary_expression>)?

<logical_or_expression> ::= <logical_and_expression> ("||" <logical_and_expression>)*

<logical_and_expression> ::= <bitwise_or_expression> ("&&" <bitwise_or_expression>)*

<bitwise_or_expression> ::= <bitwise_xor_expression> ("|" <bitwise_xor_expression>)*

<bitwise_xor_expression> ::= <bitwise_and_expression> ("^" <bitwise_and_expression>)*

<bitwise_and_expression> ::= <equality_expression> ("&" <equality_expression>)*

<equality_expression> ::= <relational_expression> (("==" | "!=") <relational_expression>)*

<relational_expression> ::= <shift_expression> ((">" | ">=" | "<" | "<=") <shift_expression>)*

<shift_expression> ::= <additive_expression> (("<<" | ">>") <additive_expression>)*

<additive_expression> ::= <multiplicative_expression> (("+" | "-") <multiplicative_expression>)*

<multiplicative_expression> ::= <unary_expression> (("*" | "/" | "%") <unary_expression>)*

<unary_expression> ::= ("!" | "-" | "~" | "++" | "--" | "*" | "&") <unary_expression>
                      | <postfix_expression>

<postfix_expression> ::= <primary_expression> <postfix_operator>*

<postfix_operator> ::= "++" | "--" | "[" <expression> "]" | "(" <argument_list> ")"

<primary_expression> ::= <literal>
                       | <identifier>
                       | <function_call>
                       | <array_access>
                       | <dereference_expression>
                       | <address_of_expression>
                       | "(" <expression> ")"

<function_call> ::= <identifier> "(" <argument_list> ")"

<argument_list> ::= <expression> ("," <expression>)*
                  | ε

<array_access> ::= <identifier> "[" <expression> "]"

<dereference_expression> ::= "*" <expression>

<address_of_expression> ::= "&" <expression>
```

### **8. Literals**
```
<literal> ::= <int_literal>
            | <float_literal>
            | <string_literal>
            | <char_literal>
            | <bool_literal>
            | <null_literal>

<int_literal> ::= <decimal_literal>
                | <hex_literal>
                | <octal_literal>
                | <binary_literal>

<decimal_literal> ::= [0-9]+
<hex_literal> ::= "0" [xX] [0-9a-fA-F]+
<octal_literal> ::= "0" [oO] [0-7]+
<binary_literal> ::= "0" [bB] [01]+

<float_literal> ::= [0-9]+ "." [0-9]+ ([eE] [+-]? [0-9]+)?
                  | [0-9]+ [eE] [+-]? [0-9]+

<string_literal> ::= "\"" <string_content> "\""
<string_content> ::= ([^"\\] | <escape_sequence>)*
<escape_sequence> ::= "\\" [nrt\\"']

<char_literal> ::= "'" <char_content> "'"
<char_content> ::= [^'\\] | <escape_sequence>

<bool_literal> ::= "true" | "false"
<null_literal> ::= "null"
```

### **9. Blocks and Scope**
```
<block> ::= "{" <statement>* "}"
```

### **10. Special Statements**
```
<return_statement> ::= "return" <expression>? ";"

<break_statement> ::= "break" ";"

<continue_statement> ::= "continue" ";"

<print_statement> ::= "print" <expression> ";"
```

---

## **OPERATOR PRECEDENCE AND ASSOCIATIVITY**

### **Complete Precedence Hierarchy (Highest to Lowest)**

1. **Primary**: `()` (grouping), `[]` (array access), `.` (member access)
2. **Postfix**: `++`, `--` (postfix)
3. **Unary**: `!`, `-`, `~`, `++`, `--` (prefix), `*` (dereference), `&` (address-of)
4. **Multiplicative**: `*`, `/`, `%`
5. **Additive**: `+`, `-`
6. **Shift**: `<<`, `>>`
7. **Relational**: `<`, `<=`, `>`, `>=`
8. **Equality**: `==`, `!=`
9. **Bitwise AND**: `&`
10. **Bitwise XOR**: `^`
11. **Bitwise OR**: `|`
12. **Logical AND**: `&&`
13. **Logical OR**: `||`
14. **Ternary**: `? :`
15. **Assignment**: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`

### **Associativity Rules**
- **Left Associative**: All binary operators except assignment and ternary
- **Right Associative**: Assignment operators, ternary operator, unary operators

---

## **PARSER ERROR HANDLING**

### **Implemented ParseError Types**
```cpp
enum class ParseError {
    UnexpectedEOF,           // End of file reached unexpectedly
    FailedToFindToken,       // Token not found during parsing
    ExpectedTypeToken,        // Expected a type token (int, float, etc.)
    ExpectedIdentifier,      // Expected an identifier
    UnexpectedToken,         // Unexpected token encountered
    ExpectedFloatLit,        // Expected float literal
    ExpectedIntLit,          // Expected integer literal
    ExpectedStringLit,       // Expected string literal
    ExpectedBoolLit,         // Expected boolean literal
    ExpectedExpr,            // Expected expression
    ExpectedStatement,       // Expected statement
    ExpectedFunctionBody,    // Expected function body
    ExpectedParameterList,   // Expected parameter list
    ExpectedBlock            // Expected block statement
};
```

---



#### **Control Flow**
- `if` statements with `else`
- `while` loops
- `for` loops (with initialization, condition, update)
- `do-while` loops
- `switch` statements with `case` and `default`
- `break` and `continue` statements

#### **Data Types**
- Basic types: `int`, `float`, `string`, `char`, `bool`, `void`
- Arrays: `int arr[10]`, `arr[index]`
- Pointers: `int* ptr`, `*ptr`, `&value`

#### **Expressions**
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Relational: `==`, `!=`, `>`, `<`, `>=`, `<=`
- Logical: `&&`, `||`, `!`
- Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>`
- Assignment: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`
- Unary: `++`, `--`, `-`, `!`, `~`, `*`, `&`
- Ternary: `condition ? true : false`

#### **Functions**
- Function declarations with parameters
- Function calls with arguments
- Return statements

#### **Variables**
- Global variable declarations
- Local variable declarations
- Variable initialization
- Array initialization

#### **Literals**
- Integer literals (decimal, hex, octal, binary)
- Float literals (with scientific notation)
- String literals (with escape sequences)
- Character literals
- Boolean literals

---
