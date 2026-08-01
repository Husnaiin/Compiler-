# Contributing

Thanks for taking a look. Issues and pull requests are both welcome.

## Getting set up

You need a C++17 compiler and nothing else.

```bash
git clone https://github.com/Husnaiin/Compiler-.git
cd Compiler-
mkdir -p build
g++ -std=c++17 -O2 Non_Regex/*.cpp -o build/compiler
```

Note the wildcard build works only if `ir_demo.cpp` is excluded — it has its own
`main()`. Use the explicit nine-file command in the [README](README.md#build) if the
linker complains about a duplicate `main`.

Verify before changing anything:

```bash
for f in tests/test_scope_*.txt; do ./build/compiler "$f"; done
```

## Where things live

The pipeline runs in a fixed order and each stage owns one concern:

| Stage | File | Owns |
|---|---|---|
| 1 | `Non_Regex/lexer.cpp` | characters → tokens, escapes, positions |
| 2 | `Non_Regex/parser.cpp` | tokens → AST, precedence |
| 3 | `Non_Regex/scope_analyzer.cpp` | scope tree, symbol table |
| 4 | `Non_Regex/type_checker.cpp` | types, arity, returns |
| 5 | `Non_Regex/ir_generator.cpp` | AST → three-address code |
| 6 | `Non_Regex/llvm_backend.cpp` | TAC → `.ll` |
| 7 | `Non_Regex/riscv_backend.cpp` | TAC → `.s` |

**Keep diagnostics on the stage that owns them.** The parser should not report type
errors and the type checker should not report undeclared identifiers. Fail-fast
staging is the reason a single mistake produces one message instead of twenty, and it
stops working the moment stages start reaching into each other's territory.

**New backends consume TAC, not the AST.** If you are adding a target, start from
`ir_generator.h` and look at `riscv_backend.cpp` as the reference — it is deliberately
the smallest file in the back end.

## Adding a test

Tests are plain source files under `tests/`, run through the compiler:

- Valid programs: `tests/test_scope_<what>.txt` — should complete all seven stages
- Error cases: `tests/err_<what>.src` — should stop with exactly one clear diagnostic

Name the file after the behaviour, not the bug number. Every pull request that
changes compiler behaviour should add or update at least one.

## Pull requests

Before opening one:

1. It builds clean with `-std=c++17` and no new warnings
2. The full `tests/` sweep behaves as expected
3. Any behaviour change has a test file demonstrating it
4. No build artifacts are staged — `build/`, `*.o` and the binaries are gitignored

Describe what changed and which stage it touched. If you hit something in
[Known Limitations](README.md#known-limitations), say so in the PR rather than
working around it silently.

## Style

Match the file you are editing. Broadly: four-space indent, `snake_case` for
functions and variables, `PascalCase` for types, braces on the same line. Prefer a
clear twenty-line function to a clever five-line one — this codebase is meant to be
read by people learning how compilers fit together.

## Reporting a bug

Include the source file that triggers it, the command you ran, and the full output.
For a wrong-output bug rather than a crash, say what you expected the emitted TAC,
`.ll` or `.s` to look like.
