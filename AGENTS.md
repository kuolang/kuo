# AGENTS.md - AI Agent Guidelines for Kuo

This document provides guidance for humans and AI coding agents (such as Claude, Copilot, or similar tools) working on the Kuo compiler codebase. It describes the architecture, conventions, and rules that agents must follow to make safe, correct contributions.

---

## What Is Kuo?

Kuo is a compiled programming language that transpiles to C++. The compiler is written in C++17 and follows a classic pipeline:

```
.kuo source
    │
    ▼
 Lexer          (src/lexer.cpp)       Tokenizes source text into a flat token stream
    │
    ▼
 Parser         (src/parser.cpp)      Builds an AST from the token stream
    │
    ▼
 Code Generator (src/codegen.cpp)     Walks the AST and emits C++ source
    │
    ▼
 C++ Compiler   (g++ / clang++)       Produces a native binary
```

Each stage is independent. Agents should understand which stage a task belongs to before making changes.

---

## Core Invariants

These invariants must be preserved by any agent-authored change:

1. **AST nodes are owned by `std::unique_ptr`.** Never use raw `new` or raw owning pointers for AST nodes. Pass ownership via `std::move`.

2. **The pipeline is one-directional.** The lexer does not call the parser; the parser does not call the codegen. Data flows forward only via return values.

3. **`ast.h` is the shared contract.** The parser produces nodes defined in `ast.h`. The codegen consumes them. If you add a new AST node, you must update both the parser (to produce it) and the codegen (to consume it). An unhandled node type in `genStmt()` or `genExpr()` will throw at runtime - this is intentional.

4. **Zero warnings policy.** All code must compile cleanly under `g++ -std=c++17 -Wall -Wextra`. Do not suppress warnings with pragmas or casts unless absolutely necessary and documented.

5. **The `examples/` directory is the test suite.** After any change, `make test` must pass without modification to any existing example. If a change alters output intentionally, update the example and document why.

---

## How to Add a New Language Feature

Adding a feature (e.g., a `break` statement, a new operator, a new type) always touches at least three files. Follow this checklist:

### Step 1 - Token (if needed)
If the feature requires a new keyword or operator, add it to `include/token.h`:
- Add the variant to `TokenType`
- Add the string mapping in `tokenTypeName()`
- Add the keyword to the `KEYWORDS` map in `src/lexer.cpp`

### Step 2 - AST Node (if needed)
If the feature introduces new syntax structure, add a node to `include/ast.h`:
- For statements, inherit from `Stmt`
- For expressions, inherit from `Expr`
- Keep node structs simple: store only what the codegen needs
- Use `ExprPtr` / `StmtPtr` (`std::unique_ptr<Expr/Stmt>`) for child ownership

### Step 3 - Parser
Add a parsing method to `src/parser.cpp`:
- Dispatch from `parseStatement()` or `parseExpr()` based on where the feature appears syntactically
- Throw `ParseError` (not `std::runtime_error`) on unexpected tokens so line/col info is preserved
- Consume tokens with `consume()`, validate with `expect()`, test with `check()` and `match()`

### Step 4 - Code Generator
Add a generation method to `src/codegen.cpp`:
- Add a `dynamic_cast` branch in `genStmt()` or `genExpr()`
- Emit valid C++17. When in doubt, check what `./kuo --emit-cpp` produces and verify it compiles with `g++ -std=c++17`
- Update `inferType()` if the new expression has a deterministic type

### Step 5 - Example & Test
Add or update a `.kuo` file in `examples/` that exercises the new feature. Run `make test` to confirm all examples pass.

---

## How the Codegen Scope System Works

`CodeGen` maintains a scope stack (`std::vector<std::unordered_map<std::string, KuoType>>`):

- `pushScope()` / `popScope()` - called at block entry/exit
- `declareVar(name, type)` - registers a variable in the current scope
- `lookupVar(name)` - walks the stack from innermost to outermost

This is used by `inferType()` to determine the C++ type of an `IdentExpr`. When adding a new statement that introduces a variable, always call `declareVar` after resolving the type, before generating any sub-expressions that might reference it.

Function return types are collected in `funcReturnTypes` during a first-pass scan in `generate()`, so recursive function calls resolve correctly.

---

## String Concatenation Rules

Kuo allows `+` between strings and any other primitive type. The codegen in `genBinary()` handles this by wrapping non-string operands:

- `int` / `float` → `std::to_string(...)`
- `bool` → `(std::string(... ? "true" : "false"))`
- `string` → used as-is

If you add a new type, update this logic in `genBinary()`.

---

## Error Handling Conventions

| Location   | Exception Type | Fields             |
|------------|----------------|--------------------|
| Lexer      | `std::runtime_error` | message with line info embedded |
| Parser     | `ParseError`   | `message`, `line`, `col` |
| CodeGen    | `std::runtime_error` | message only |

`main.cpp` catches each type separately and prefixes output with `[Lexer Error]`, `[Parse Error]`, or `[CodeGen Error]`. Preserve this pattern when adding new error cases.

---

## What Agents Should NOT Do

- **Do not modify `main.cpp` to change the pipeline order.** The lexer → parser → codegen sequence is fixed.
- **Do not add runtime memory allocation in the codegen output** (e.g., no `new` in generated C++ code). All Kuo values should map to C++ stack variables or `std::string`.
- **Do not add external library dependencies** to the compiler itself. The compiler must build with only the C++ standard library.
- **Do not change the `.kuo` file extension** or the CLI flag names without updating the README and all examples.
- **Do not merge AST node responsibilities.** Each node type should represent exactly one syntactic construct. Do not add optional fields to existing nodes to handle unrelated features.
- **Do not skip the `make test` step.** Even if a change seems trivial, always verify before considering the task complete.

---

## Suggested Agent Workflow for Bug Fixes

1. Read the bug report and identify which pipeline stage is responsible.
2. Use `./kuo examples/repro.kuo --emit-cpp` to inspect the generated C++ for logic errors in codegen.
3. Add `std::cerr` debug output temporarily in the relevant stage to trace token/AST state.
4. Make the minimal fix. Do not refactor unrelated code in the same change.
5. Remove all debug output.
6. Run `make test` and confirm all examples still pass.
7. Add or update an example in `examples/` that would have caught the bug.

---

## Versioning

The compiler version is defined in `main.cpp` as a string literal in `printUsage()`. When the language gains a significant new feature, increment the minor version. Patch-level fixes do not require a version bump.

---

## Questions?

If something in the codebase is unclear, check `README.md` first for user-facing documentation, then this file for internals. If still unclear, open a GitHub Issue with the label `question`.