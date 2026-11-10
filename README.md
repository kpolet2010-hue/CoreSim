# CoreSim

[![CI](https://github.com/kpolet2010-hue/CoreSim/actions/workflows/ci.yml/badge.svg)](https://github.com/kpolet2010-hue/CoreSim/actions/workflows/ci.yml)

CoreSim is an emulator for a custom 16-bit CPU (**CoreSim-16**), written in C++17.

## Architecture at a glance

| Property         | Value                                   |
|------------------|-----------------------------------------|
| Data width       | 16-bit                                  |
| Registers        | 16 general-purpose (R0–R15)             |
| Special registers| PC, SP (18-bit), IR (32-bit), DBR (2-bit), FLAGS (Z, N, C, V) |
| RAM              | 256 KB, byte-addressable                |
| Address space    | 18-bit (`0x00000`–`0x3FFFF`)            |
| Instruction size | 32-bit, fixed length                    |
| Byte order       | Little-endian                           |

## Current status

| Component | State | Docs |
|-----------|-------|------|
| Memory (`Memory`) | ✅ 8/16/32-bit reads and writes, bounds checking | [MEMORY.md](docs/MEMORY.md) |
| General-purpose registers (`Register`) | ✅ R0–R15, indexed access, reset | [REGISTERS.md](docs/REGISTERS.md) |
| Special registers (`CPUState`) | ✅ PC, SP, IR, DBR, FLAGS with bit-width masking | [REGISTERS.md](docs/REGISTERS.md) |
| Instruction decoder (`Decoder`) | ✅ R, I, M and J format, selected by the top 2 opcode bits | [DECODER.md](docs/DECODER.md) |
| Instruction set (`Opcode`) | ✅ 26 opcodes with values in their format ranges | [ISA.md](docs/ISA.md) |
| ALU (`ALU`) | ✅ `ADD`, `ADDI`, `SUB`, `MUL`, `DIV`, `CMP`, `AND`, `OR`, `XOR`, `NOT` with Z, N, C, V, independent of the CPU | [ARCHITECTURE.md](docs/ARCHITECTURE.md#alu) |
| CPU (`CPU`) | ✅ Owns registers, state, decoder and ALU, references memory and an output stream, `reset()`, `step()` (fetch, decode and all 26 instructions), `run()` until `HLT` | [ARCHITECTURE.md](docs/ARCHITECTURE.md) |
| Program loader (`loadHex`) | ✅ Loads hex text programs with comments into memory, reports invalid lines with their line number | [LOADER.md](docs/LOADER.md) |
| Command line (`coresim`) | ✅ Loads a hex file and runs it until `HLT`, reports errors with exit code 1 | [Running a program](#running-a-program) |

`CPU::step()` fetches, decodes and executes one instruction. All 26 opcodes specified in [ISA.md](docs/ISA.md) are implemented. `OUT` writes to `std::cout` unless another stream is passed to the CPU. `CPU::run(maxSteps)` executes a whole program until `HLT`, with a step limit against endless loops.

## Building

Requirements: CMake ≥ 3.16, a C++17 compiler (GCC, Clang or MSVC), optionally Ninja.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Running a program

```sh
./build/coresim examples/countdown.hex
```

```
CoreSim v0.1
3
2
1
```

`coresim` loads the hex file given as the first argument (see [LOADER.md](docs/LOADER.md)) and runs it until `HLT`. Without an argument it asks for the file name. The values written by `OUT` appear on the console.

| Case | Message on stderr | Exit code |
|------|-------------------|-----------|
| program reaches `HLT` | – | `0` |
| file cannot be opened | `error: could not open <file>` | `1` |
| invalid line in the file | `error: line N: expected 8 hex digits` | `1` |
| no `HLT` after 1,000,000 steps | `error: no HLT after 1000000 steps` | `1` |
| the CPU throws (e.g. `DIV` by zero) | `error: <message>` | `1` |

More programs are in [examples/](examples/README.md).

## Running tests

```sh
ctest --test-dir build --output-on-failure
```

or run the test binary directly to see every check:

```sh
./build/tests/coresim_tests
```

See [TESTING.md](docs/TESTING.md) for the test structure and CI.

## Project layout

```
include/coresim/   public headers (ALU, Architecture, CPU, CPUState, Memory, Register, Decoder, DecodedInstruction, Opcode, Loader)
src/               implementation + main.cpp
tests/             test suites (TestSuite base class, one suite per component)
docs/              architecture documentation (Markdown + Mermaid)
diagrams/          draw.io diagrams (registers & memory, data flow, decoder, ISA)
examples/          example programs in hex text format
.github/workflows/ CI pipeline (build + test on every push/PR to main)
```

## Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) – components, ownership, data flow and the ALU
- [MEMORY.md](docs/MEMORY.md) – RAM, little-endian layout, bounds checking
- [REGISTERS.md](docs/REGISTERS.md) – R0–R15 and the special registers
- [DECODER.md](docs/DECODER.md) – 32-bit instruction format and decoding
- [ISA.md](docs/ISA.md) – opcodes, their values and formats
- [TESTING.md](docs/TESTING.md) – test suites and CI
- [LOADER.md](docs/LOADER.md) – hex text format for programs and `loadHex()`
- [ASSEMBLER.md](docs/ASSEMBLER.md) – assembly language and assembler (specification, not implemented yet)

Diagrams (open with [draw.io](https://www.drawio.com/)):

- [diagrams/registers-memory.drawio](diagrams/registers-memory.drawio) – pages *Registers* and *Memory*
- [diagrams/data-flow.drawio](diagrams/data-flow.drawio) – build targets, components, data flow, error handling
- [diagrams/decoder.drawio](diagrams/decoder.drawio) – format selection, bit layouts of R/I/M/J, shift + mask, `DecodedInstruction`, tested examples
- [diagrams/isa.drawio](diagrams/isa.drawio) – opcode map and opcode groups
