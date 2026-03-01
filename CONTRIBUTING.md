# Contributing to Kuo

Thank you for your interest in contributing to the Kuo programming language! This document explains how to get involved, what standards to follow, and how the project is organized for contributors.

---

## Table of Contents

- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Development Workflow](#development-workflow)
- [Code Style](#code-style)
- [How to Contribute](#how-to-contribute)
- [Testing](#testing)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)
- [Commit Message Guidelines](#commit-message-guidelines)
- [Pull Request Process](#pull-request-process)

---

## Getting Started

### Prerequisites

- C++17-compatible compiler (g++ ≥ 7 or clang++ ≥ 5)
- CMake ≥ 3.15 or Make
- Git

### Building from Source

```bash
git clone https://github.com/kuolang/kuo.git
cd kuo
make
```

To verify everything works:

```bash
make test
```

All example programs in `examples/` should compile and run without errors.

The compiler pipeline flows in one direction:

```
Lexer → Parser → AST → CodeGen → C++ source → g++/clang++ → binary
```

Each stage is isolated in its own header/source pair. Changes to one stage should not require changes to unrelated stages.

---

## Development Workflow

1. Fork the repository and create a feature branch off `master`:
   ```bash
   git checkout -b feature/my-feature
   ```

2. Make your changes. Keep each commit focused on a single concern.

3. Run `make test` before submitting to confirm nothing is broken.

4. Open a pull request with a clear title and description.

---

## Code Style

Kuo's compiler is written in C++17. Please follow these conventions:

- Use 4-space indentation, no tabs.
- Keep lines under 100 characters where reasonable.
- Use `snake_case` for variables and functions, `PascalCase` for classes and structs, `UPPER_CASE` for constants and enum values.
- Prefer `std::unique_ptr` and `std::move` for AST node ownership. Do not use raw `new`/`delete` for AST nodes.
- All public functions and structs in headers should have a brief comment explaining their purpose.
- Avoid `using namespace std;` in headers.
- Compiler warnings (`-Wall -Wextra`) must produce zero warnings on your changes.

---

## How to Contribute

### Good First Issues

If you're new to the project, look for issues labeled `Type: First issue`. These are typically well-scoped and self-contained. Examples of beginner-friendly tasks:

- Adding a new example `.kuo` program
- Improving error messages in the lexer or parser
- Adding a missing operator (e.g., bitwise operators)
- Writing or improving documentation

### Larger Contributions

For larger changes - new language features, refactors, or changes to the compiler pipeline - please open an issue first to discuss the approach before writing code. This avoids duplicated effort and ensures the change aligns with the project's direction.

Areas where contributions are especially welcome:

- **Standard library functions** - math, string manipulation, type conversions
- **Better type checking** - a proper semantic analysis pass
- **Improved error messages** - pointing to the exact source location with context
- **New control flow** - `break`, `continue`, `switch`/`match`
- **Arrays and collections** - basic list/array support
- **Tooling** - syntax highlighting, language server, VS Code extension

---

## Testing

Tests are run by compiling and executing the `.kuo` programs in the `examples/` directory.

```bash
make test
```

When adding a new language feature, you must include a corresponding example program in `examples/` that exercises it. The program must produce correct, deterministic output.

For bug fixes, add or update an example that reproduces the bug, confirming it no longer occurs after your fix.

---

## Reporting Bugs

Please open a GitHub Issue and include:

1. A minimal `.kuo` program that reproduces the bug.
2. The expected output.
3. The actual output or error message.
4. Your OS and compiler version (`g++ --version` or `clang++ --version`).

---

## Suggesting Features

Open a GitHub Issue with the label `Type: Enhancement`. Describe:

1. What problem the feature solves.
2. What the syntax would look like (provide a short `.kuo` code example).
3. Any implementation considerations you're aware of.

---

## Commit Message Guidelines

Use the following format:

```
<type>: <short summary>

<optional body>
```

Types:

| Type       | Use for                                          |
|------------|--------------------------------------------------|
| `feat`     | A new language feature or compiler capability    |
| `fix`      | A bug fix                                        |
| `refactor` | Code restructuring with no behavior change       |
| `docs`     | Documentation only                               |
| `test`     | Adding or updating tests / example programs      |
| `chore`    | Build system, CI, tooling changes                |

Examples:

```
feat: add break and continue statements

fix: string + int concatenation emitting incorrect C++

docs: add for loop section to README
```

---

## Pull Request Process

1. Ensure `make test` passes with zero errors.
2. Ensure your code compiles with zero warnings under `-Wall -Wextra`.
3. Squash trivial fixup commits before opening the PR.
4. Fill out the PR description with: what changed, why, and how to test it.
5. A maintainer will review within a few days. Be prepared to revise based on feedback.
6. Once approved, a maintainer will merge your PR.

---

By contributing to Kuo, you agree that your contributions will be licensed under the same license as the project. Thank you for helping build the language!