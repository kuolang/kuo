# Kuo Language Roadmap

This document outlines the planned evolution of the Kuo programming language beyond the MVP. It is organized into milestones, each building on the last. Priorities may shift based on community feedback.

---

## Current State - v0.0.1


- [x] Declare variables with type annotations or type inference
- [x] Use primitive types: `int`, `float`, `bool`, `string`
- [x] Perform arithmetic, comparison, and logical operations
- [x] Use `if`/`else`, `while`, and `for` control flow
- [x] Declare and call functions with typed parameters and return values
- [x] Recurse through function calls
- [x] Print values with `print()`
- [x] Report syntax errors with line and column information
- [x] Compile via a single `kuo <file.kuo>` command using a C++ backend

---

## v0.2.0 - Language Completeness

*Goal: Close the most common gaps a developer hits when writing real programs.*

### Control Flow
- [x] `break` and `continue` in loops
- [x] `else if` as a first-class construct (currently requires nesting)
- [x] `loop` keyword for infinite loops (`loop { ... }`)

### Operators
- [ ] Compound assignment operators: `%=`
- [ ] Bitwise operators: `&`, `|`, `^`, `~`, `<<`, `>>`
- [ ] Ternary expression: `condition ? a : b`

### Variables
- [ ] `const` declarations for compile-time constants
- [ ] Multiple assignment: `let a, b = 1, 2;`

### Strings
- [ ] String interpolation: `"Hello, {name}!"`
- [x] Built-in string functions: `len()`, `upper()`, `lower()`, `contains()`, `split()`

### Error Reporting
- [ ] Source-line context in error messages (print the offending line with a `^` caret)
- [ ] Multiple errors reported in a single pass rather than stopping at the first

---

## v0.3.0 - Collections

*Goal: Make it possible to work with groups of data.*

### Arrays
- [ ] Fixed-size array type: `let nums: int[5] = [1, 2, 3, 4, 5];`
- [ ] Index access: `nums[0]`
- [ ] Index assignment: `nums[2] = 99;`
- [ ] `len(array)` built-in

### Dynamic Lists
- [ ] `list<T>` type backed by `std::vector`
- [ ] `push()`, `pop()`, `len()` methods
- [ ] Range-based `for` loop: `for (item in list) { ... }`

### Strings as Collections
- [ ] Character access via index: `name[0]`
- [ ] `len(string)` built-in

---

## v0.4.0 - Type System

*Goal: Catch more errors at compile time with a proper semantic analysis pass.*

### Semantic Analysis Pass
- [ ] Type-checking pass between parser and codegen (currently types are inferred lazily in codegen)
- [ ] Type mismatch errors with source location
- [ ] Use-before-declaration errors
- [ ] Unused variable warnings

### Type Features
- [ ] Explicit casting: `int(3.7)`, `float(5)`, `string(42)`
- [ ] Nullable types: `int?` with `null` literal and null checks
- [ ] Type aliases: `type Score = int;`

---

## v0.5.0 - Structs

*Goal: Allow users to define their own data structures.*

### Struct Declaration
```kuo
struct Point {
    x: float,
    y: float,
}
```

### Struct Usage
- [ ] Field access: `p.x`
- [ ] Field assignment: `p.x = 3.0;`
- [ ] Struct literals: `let p = Point { x: 1.0, y: 2.0 };`
- [ ] Structs as function parameters and return types

### Methods
- [ ] Method declarations on structs:
```kuo
fn Point.distance(other: Point) -> float {
    ...
}
```

---

## v0.6.0 - Modules & Imports

*Goal: Allow programs to span multiple files.*

### Module System
- [ ] `import "path/to/file.kuo"` statement
- [ ] Exported vs. private declarations (`pub` modifier)
- [ ] Circular import detection
- [ ] Package manager

### Standard Library (Initial)
- [ ] `std/math` - `sqrt`, `pow`, `abs`, `floor`, `ceil`, `min`, `max`
- [ ] `std/string` - string manipulation functions
- [ ] `std/io` - `read_line()` for user input, file read/write
- [ ] `std/convert` - explicit type conversion utilities

---

## v0.7.0 - Error Handling

*Goal: Give programs a structured way to handle failure.*

### Result Type
- [ ] Built-in `Result<T>` type with `Ok(value)` and `Err(message)` variants
- [ ] `?` propagation operator for early return on error
- [ ] Pattern matching on result: `match result { Ok(v) => ..., Err(e) => ... }`

### Panic
- [ ] `panic("message")` for unrecoverable errors
- [ ] Stack trace output on panic

---

## v0.8.0 - Tooling

*Goal: Make the development experience feel complete.*

### CLI Improvements
- [ ] `kuo check <file.kuo>` - type-check without compiling
- [ ] `kuo fmt <file.kuo>` - auto-format source code
- [ ] `kuo new <project>` - scaffold a new project
- [ ] Watch mode: `kuo run --watch <file.kuo>`

### Editor Support
- [ ] VS Code extension with syntax highlighting
- [ ] `.kuo` TextMate grammar for GitHub rendering
- [ ] Language Server Protocol (LSP) implementation for hover types, go-to-definition, and diagnostics

### Package Manager (Early Design)
- [ ] Project manifest (name, version, dependencies)
- [ ] `kuo add <package>` to fetch dependencies
- [ ] Central package registry

---

## v1.0.0 - Stable Release

*Goal: A language stable enough to commit to for real projects.*

### Stability
- [ ] Stable language specification document
- [ ] No breaking syntax changes after 1.0
- [ ] Comprehensive test suite covering all language features
- [ ] Fuzz testing of the lexer and parser

### Performance
- [ ] Benchmark suite comparing Kuo programs to equivalent C++ and Python
- [ ] Optimization hints in codegen (e.g., avoid unnecessary `std::string` copies)

### Documentation
- [ ] Full language reference manual
- [ ] Interactive tutorial (web-based)
- [ ] Cookbook with real-world code examples

---

## Long-Term / Research

These are exploratory ideas that may or may not ship, depending on how the language evolves:

- **Native backend** - emit LLVM IR or x86-64 assembly directly, removing the C++ intermediary
- **Generics** - parameterized types like `fn max<T>(a: T, b: T) -> T`
- **Interfaces / Traits** - structural typing contracts between structs
- **Concurrency primitives** - `async`/`await` or lightweight goroutine-style tasks
- **WASM target** - compile Kuo programs to WebAssembly
- **REPL** - interactive Kuo prompt for experimentation

---

## How to Influence the Roadmap

The roadmap is a living document. To propose changes:

1. Open a GitHub Issue tagged `roadmap`.
2. Describe the feature, the problem it solves, and a rough syntax sketch.
3. Community discussion will determine if and when it gets scheduled.

Contributions toward any roadmap item are welcome at any time - you don't have to wait for the corresponding milestone to be active. See [CONTRIBUTING.md](CONTRIBUTING.md) for how to get started.