# CoreSim

CoreSim is an emulator for a custom 16-bit CPU, written in C++17.

## Architecture

| Property         | Value                       |
|------------------|-----------------------------|
| Data width       | 16-bit                      |
| Registers        | 16 general-purpose (R0–R15) |
| RAM              | 256 KB                      |
| Address space    | 18-bit                      |
| Instruction size | 32-bit, fixed length        |
| Byte order       | Little-endian               |

## Building

```sh
cmake -S . -B build
cmake --build build
./build/coresim
```

## Running tests

```sh
ctest --test-dir build
```

## Layout

- `include/coresim/` – public headers
- `src/` – implementation and `main.cpp`
- `tests/` – unit tests (run via CTest)
- `examples/` – example programs

## Status

Early scaffolding. CPU execution is not implemented yet.
