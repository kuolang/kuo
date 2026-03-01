<div align="center" style="display:grid;place-items:center;">
<p>
    <a href="https://kuolang.org/" target="_blank"><img width="700" src="https://socialify.git.ci/kuolang/kuo/image?description=1&font=Inter&forks=1&issues=1&logo=https%3A%2F%2Fgithub.com%2Fkuolang%2Fkuo%2Fblob%2Fmaster%2F.github%2Flogo.png%3Fraw%3Dtrue&name=1&owner=1&pattern=Plus&pulls=1&stargazers=1&theme=Auto" alt="The Kuo logo"></a>
</p>
</div>
<div align="center" style="display:grid;place-items:center;">

[![Version](https://img.shields.io/badge/version-0.0.1-blue?style=for-the-badge)](https://github.com/kuolang/kuo/releases)
[![License](https://img.shields.io/badge/License-GNU--License-yellow?style=for-the-badge)](./LICENSE)
[![Docker](https://img.shields.io/badge/docker-ready-2496ED?style=for-the-badge&logo=docker&logoColor=white)](./Dockerfile)

</div>

## Language Tour

### Variables

```kuo
let x: int = 42;
let pi: float = 3.14159;
let name: string = "Kuo";
let flag: bool = true;

// Type inference (type is inferred from the initializer)
let y = 100;        // int
let z = 2.5;        // float
```

### Primitive Types

| Kuo type | C++ equivalent | Example         |
|----------|----------------|-----------------|
| `int`    | `int`          | `42`, `-7`      |
| `float`  | `double`       | `3.14`, `1.0`   |
| `bool`   | `bool`         | `true`, `false` |
| `string` | `std::string`  | `"hello"`       |
| `void`   | `void`         | (return type)   |

### Output

```kuo
print("Hello, World!");
print(42);
print(3.14);
print(true);
```

### Arithmetic & Operators

```kuo
let a = 10 + 5;   // 15
let b = 10 - 3;   // 7
let c = 4 * 6;    // 24
let d = 20 / 4;   // 5
let e = 17 % 3;   // 2

// Comparison (returns bool)
let bigger = a > b;        // true
let equal  = a == 15;      // true
let combo  = a > 5 && b < 10;  // true
```

### If / Else

```kuo
if (x > 0) {
    print("positive");
} else {
    print("non-positive");
}
```

### While Loop

```kuo
let i: int = 0;
while (i < 5) {
    print(i);
    i++;
}
```

### For Loop

```kuo
for (let i = 0; i < 5; i++) {
    print(i);
}
```

### Functions

```kuo
fn add(a: int, b: int) -> int {
    return a + b;
}

fn greet(name: string) -> string {
    return "Hello, " + name + "!";
}

// Void function (no return type needed)
fn sayHi() {
    print("Hi!");
}

// Recursive functions work too
fn factorial(n: int) -> int {
    if (n <= 1) { return 1; }
    return n * factorial(n - 1);
}

print(add(3, 4));          // 7
print(greet("Kuo"));       // Hello, Kuo!
print(factorial(6));       // 720
```

### Comments

```kuo
// Single-line comment

/* Multi-line
   comment */
```

---

## Compiler Pipeline

```
source.kuo
    │
    ▼
 Lexer          → tokens
    │
    ▼
 Parser         → AST
    │
    ▼
 Code Generator → C++ source
    │
    ▼
 g++ / clang++  → native binary
```

---

## CLI Reference

```
kuo <source.kuo>              Compile and run immediately
kuo <source.kuo> -o <output>  Compile to executable
kuo <source.kuo> --emit-cpp   Print generated C++ and exit
kuo --help                    Show help
```

---

## Building

### Requirements

- A C++17-compatible compiler (g++ ≥ 7 or clang++ ≥ 5)
- CMake ≥ 3.15 (optional; Makefile also provided)

### Build script

```bash
# Using Make (simplest)
make

# Or using CMake
mkdir build && cd build
cmake .. && make
```

### Run a program

```bash
# Compile and run immediately
./kuo examples/hello.kuo

# Compile to a standalone executable
./kuo examples/hello.kuo -o hello
./hello

# Inspect the generated C++ code
./kuo examples/hello.kuo --emit-cpp
```

---

## License
[GNU License](./LICENSE)