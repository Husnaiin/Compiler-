# Example 01 - Expressions

## Source code

```
fn ginti a(ginti x, ginti y) { wapsi x + y . } .
ginti b = a(3, 6) .
ginti f = 1 .
ginti z = 2 .
ginti h = 6 * f + 3 * 2 - b * b * z * z .
ginti c = 10 - (-b * 10 + 12) + (5 - 10) .
```

## Token Stream

```
[Function, Int, Identifier("a"), ParenL, Int, Identifier("x"), Comma, Int, Identifier("y"), ParenR, BraceL, Return, Identifier("x"), AddOp, Identifier("y"), Dot, BraceR, Dot, Int, Identifier("b"), AssignOp, Identifier("a"), ParenL, IntLit(3), Comma, IntLit(6), ParenR, Dot, Int, Identifier("f"), AssignOp, IntLit(1), Dot, Int, Identifier("z"), AssignOp, IntLit(2), Dot, Int, Identifier("h"), AssignOp, IntLit(6), MulOp, Identifier("f"), AddOp, IntLit(3), MulOp, IntLit(2), SubOp, Identifier("b"), MulOp, Identifier("b"), MulOp, Identifier("z"), MulOp, Identifier("z"), Dot, Int, Identifier("c"), AssignOp, IntLit(10), SubOp, ParenL, SubOp, Identifier("b"), MulOp, IntLit(10), AddOp, IntLit(12), ParenR, AddOp, ParenL, IntLit(5), SubOp, IntLit(10), ParenR, Dot]
```

## Abstract Syntax Tree

```
[
    Fn(
        FnDecl {
            type_tok: Int,
            ident: "a",
            params: [
                Param {
                    type_tok: Int,
                    ident: "x",
                },
                Param {
                    type_tok: Int,
                    ident: "y",
                },
            ],
            block: [
                Ret(
                    ExprStmt {
                        expr: Some(
                            ::Add(AddOp)
                                "x"
                                "y"
                            ),
                        ),
                    },
                ),
            ],
        },
    ),
    Var(
        VarDecl {
            type_tok: Int,
            ident: "b",
            expr: Some(
                    Call(FnCall {
                    ident: "a",
                    args: [
                        Some(
                            3,
                        ),
                        Some(
                            6,
                        ),
                    ],
                }),
            ),
        },
    ),
    Var(
        VarDecl {
            type_tok: Int,
            ident: "f",
            expr: Some(
                1,
            ),
        },
    ),
    Var(
        VarDecl {
            type_tok: Int,
            ident: "z",
            expr: Some(
                2,
            ),
        },
    ),
    Var(
        VarDecl {
            type_tok: Int,
            ident: "h",
            expr: Some(
                ::Add(SubOp)
                    ::Add(AddOp)
                        ::Mul(MulOp)
                            6
                            "f"
                        )
                        ::Mul(MulOp)
                            3
                            2
                        )
                    )
                    ::Mul(MulOp)
                        ::Mul(MulOp)
                            ::Mul(MulOp)
                                "b"
                                "b"
                            )
                            "z"
                        )
                        "z"
                    )
                ),
            ),
        },
    ),
    Var(
        VarDecl {
            type_tok: Int,
            ident: "c",
            expr: Some(
                ::Add(AddOp)
                    ::Add(SubOp)
                        10
                        ::Add(AddOp)
                            ::Mul(MulOp)
                                Unary(SubOp)
                                    "b"
                                )
                                10
                            )
                            12
                        )
                    )
                    ::Add(SubOp)
                        5
                        10
                    )
                ),
            ),
        },
    ),
]
```

# Example 02 - Nested Blocks

## Source code

```
fn ginti some_fn() {
	duhrao (ginti a = 0 . a < 10 . a = a + 1) {
		agar (a == 5) { toro } warna {}
	}

	wapsi 5 .
} .
```

## Token Stream

```
[Function, Int, Identifier("some_fn"), ParenL, ParenR, BraceL, For, ParenL, Int, Identifier("a"), AssignOp, IntLit(0), Dot, Identifier("a"), LessThan, IntLit(10), Dot, Identifier("a"), AssignOp, Identifier("a"), AddOp, IntLit(1), ParenR, BraceL, If, ParenL, Identifier("a"), EqualsOp, IntLit(5), ParenR, BraceL, Break, BraceR, Else, BraceL, BraceR, BraceR, Return, IntLit(5), Dot, BraceR, Dot]
```

## Abstract Syntax Tree

```
[
    Fn(
        FnDecl {
            type_tok: Int,
            ident: "some_fn",
            params: [],
            block: [
                For(
                    ForStmt {
                        init: Some(
                            VarDecl {
                                type_tok: Int,
                                ident: "a",
                                expr: Some(
                                    0,
                                ),
                            },
                        ),
                        cond: ExprStmt {
                            expr: Some(
                                Comp(LessThan)
                                    "a"
                                    10
                                ),
                            ),
                        },
                        updt: Some(
                            Assign(AssignOp)
                                "a"
                                ::Add(AddOp)
                                    "a"
                                    1
                                )
                            ),
                        ),
                        block: [
                            If(
                                IfStmt {
                                    cond: Some(
                                        Comp(EqualsOp)
                                            "a"
                                            5
                                        ),
                                    ),
                                    if_block: [
                                        Break,
                                    ],
                                    else_block: [],
                                },
                            ),
                        ],
                    },
                ),
                Ret(
                    ExprStmt {
                        expr: Some(
                            5,
                        ),
                    },
                ),
            ],
        },
    ),
]
```
