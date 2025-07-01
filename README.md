# gocxx/io

🔧 Go-style I/O interfaces and utilities for C++

Part of the [gocxx](https://github.com/gocxx) standard library project.

## Features

- `Reader`, `Writer` base interfaces
- `MemoryReader`, `FileWriter`, `IStreamReader`, etc.
- Composable, minimal, and Go-inspired design

## Build & Test

```bash
cmake -B build
cmake --build build
ctest --test-dir build -C Release
