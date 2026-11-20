# CoreSim – a 16-bit CPU emulator with assembler and debugger
#### Video Demo:  <URL HERE>
#### Description:
[![CI](https://github.com/kpolet2010-hue/CoreSim/actions/workflows/ci.yml/badge.svg)](https://github.com/kpolet2010-hue/CoreSim/actions/workflows/ci.yml)

CoreSim is a custom 16-bit CPU emulator written in C++17. Instead of emulating an existing processor, I designed my own CPU architecture, including its instruction set, registers, memory layout and instruction formats.

Around the CPU, I also built an assembler with label support, a disassembler and an interactive debugger. I chose this project because I wanted to understand how processors work internally, how machine instructions are executed and how software interacts with hardware at a low level. Building my own CPU allowed me to explore these concepts by implementing every part myself.

### How to use it

CoreSim is built with CMake (see [Building](#building)). `coresim program.asm` assembles a program and runs it until `HLT`, `coresim program.hex` loads a program in hex text instead. `coresim --debug program.asm` starts the debugger, where the program can be run step by step, stopped at breakpoints and inspected. Four example programs are in [examples/](examples/README.md). The test binary `coresim_tests` checks every component and runs on every push in GitHub Actions.

### Files

| Files | Contents |
|-------|----------|
| `include/coresim/Architecture.h`, `Opcode.h` | Shared types (`Word`, `Instruction`, `Address`), the memory size and the values of the 26 opcodes |
| `Memory`, `Register`, `CPUState` | 256 KB of byte-addressed little-endian RAM with bounds checking; the registers R0–R15; the special registers PC, SP, IR, DBR and FLAGS, masked to their bit widths |
| `Decoder`, `DecodedInstruction` | Splits a 32-bit instruction into its fields. The top two opcode bits select one of the four formats R, I, M and J |
| `ALU`, `CPU` | The ALU computes results and the flags Z, N, C, V without knowing the CPU. The CPU fetches, decodes and executes one instruction per `step()` and repeats it in `run()` until `HLT` |
| `Loader`, `Assembler` | `loadHex()` loads programs written as hex text. `assemble()` translates assembly with labels in two passes and reports errors with their line number |
| `InstructionTable`, `Disassembler` | One table with the name, opcode and operands of every instruction, used by both the assembler and the disassembler. `disassemble()` turns an instruction back into text |
| `Debugger` | Runs debugger commands (`step`, `continue`, `break`, `delete`, `regs`, `mem`, `disasm`, `help`, `quit`) on a CPU. `execute()` runs one command, `run()` reads them from the console |
| `src/main.cpp` | The `coresim` program: reads the arguments, loads `.asm` or hex files, runs the program or starts the debugger with `--debug`, reports errors with exit code 1 |
| `tests/` | A small test framework (`TestSuite.h`) and one test suite per component, 12 in total |
| `examples/` | Four example programs (`countdown`, `factorial`, `fibonacci`, `squares`), each as `.asm` and `.hex` |
| `docs/`, `diagrams/` | One Markdown document per component and draw.io diagrams of the registers, memory, data flow, decoder and instruction set |
| `CMakeLists.txt`, `.github/workflows/ci.yml` | Build configuration for the library `coresim_core`, the program and the tests; CI that builds and tests every push |

The headers are in `include/coresim/`, the implementations with the same name in `src/`.

### Design choices

**My own CPU instead of an existing one.** I could have emulated an existing CPU like the 6502, which already has test programs available. Instead, I decided to create my own instruction set. This meant I had to make all the important decisions myself, such as bit widths, instruction formats and flags. It also helped me understand how real CPUs work while keeping the project manageable.

**Fixed 32-bit instructions.** Using different instruction lengths could save memory, but I decided that every CoreSim instruction should be exactly 32 bits long. The first two bits of the opcode determine which format is used. This means the PC always increases by 4, and the decoder only needs simple bit operations. It uses a bit more memory, but makes the decoder easier to implement and test.

**A two-pass assembler.** A jump instruction like `JNZ loop` can refer to a label that appears later in the code. Since its address is not known yet, the assembler needs two passes. The first pass collects all labels and their addresses. The second pass translates the instructions into machine code.

**One instruction table for assembler and disassembler.** Instead of having two separate lists for instruction names and opcodes, I use one shared table. The assembler searches by instruction name, while the disassembler searches by opcode. This avoids duplicate information and prevents the two lists from getting out of sync.

**A testable debugger.** The debugger does not handle keyboard input directly. Instead, `execute()` receives a command as a string and writes its output to a stream. The `run()` function simply handles the input loop. This makes it possible to test every debugger command automatically without having to type anything manually.

### Use of AI tools

I designed and built CoreSim myself over about three weeks in November 2026, using Claude (Anthropic) as a tutor and assistant, as allowed for the CS50 final project.

**My work:** the CPU and its instruction set, the ALU, decoder, memory and registers, the loader, the assembler with labels, the disassembler and the debugger. I wrote this logic step by step, decided how the components fit together and fixed my own bugs with the help of the tests. Claude explained concepts, asked guiding questions and, where I was stuck, gave me templates with gaps to fill in. A few small pieces it completed directly; those are marked in the code.

**Generated with Claude and reviewed by me:** the unit tests, which I used for test-driven development, the Markdown documentation and diagrams, the file overview and usage section of this README, a first draft of the text describing my design choices (which I rewrote), the example `.hex` files and some boilerplate (CMake setup, empty class skeletons, the shared instruction table).

---

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
| Assembler (`assemble`, `loadAsm`) | ✅ Translates assembly with labels into instructions in two passes, reports errors with their line number | [ASSEMBLER.md](docs/ASSEMBLER.md) |
| Disassembler (`disassemble`) | ✅ Turns an instruction back into assembly text that assembles to the same instruction, shows unknown opcodes as `???` | [DISASSEMBLER.md](docs/DISASSEMBLER.md) |
| Debugger (`Debugger`) | ✅ Steps through a program, stops at breakpoints, shows registers, memory and disassembled instructions | [DEBUGGER.md](docs/DEBUGGER.md) |
| Command line (`coresim`) | ✅ Assembles a `.asm` file or loads a hex file and runs it until `HLT`, reports errors with exit code 1, starts the debugger with `--debug` | [Running a program](#running-a-program) |

`CPU::step()` fetches, decodes and executes one instruction. All 26 opcodes specified in [ISA.md](docs/ISA.md) are implemented. `OUT` writes to `std::cout` unless another stream is passed to the CPU. `CPU::run(maxSteps)` executes a whole program until `HLT`, with a step limit against endless loops.

## Building

Requirements: CMake ≥ 3.16, a C++17 compiler (GCC, Clang or MSVC), optionally Ninja.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Running a program

```sh
./build/coresim examples/countdown.asm     # assembly
./build/coresim examples/countdown.hex     # hex text, same program
```

```
CoreSim v0.1
3
2
1
```

`coresim` loads the file given as the first argument and runs it until `HLT`. A file ending in `.asm` is assembled (see [ASSEMBLER.md](docs/ASSEMBLER.md)), every other file is loaded as hex text (see [LOADER.md](docs/LOADER.md)). Without an argument it asks for the file name. The values written by `OUT` appear on the console.

| Case | Message on stderr | Exit code |
|------|-------------------|-----------|
| program reaches `HLT` | – | `0` |
| file cannot be opened | `error: could not open <file>` | `1` |
| invalid line in a hex file | `error: line N: expected 8 hex digits` | `1` |
| error in a `.asm` file | `error: line N: <problem>`, e.g. `error: line 3: unknown instruction 'MOVE'` | `1` |
| no `HLT` after 1,000,000 steps | `error: no HLT after 1000000 steps` | `1` |
| the CPU throws (e.g. `DIV` by zero) | `error: <message>` | `1` |

More programs are in [examples/](examples/README.md).

### Debugging a program

```sh
./build/coresim --debug examples/countdown.asm
```

```
CoreSim v0.1
> 0x00000  40100003  MOVI R1, 3
(coresim) break 0x0C
breakpoint at 0x0000C
(coresim) continue
3
breakpoint at 0x0000C
> 0x0000C  C2000004  JNZ 0x00004
(coresim) quit
```

`--debug` loads the program the same way and then reads commands such as `step`, `continue`, `break`, `regs`, `mem` and `disasm` from the console instead of running it. `help` lists all commands, `quit` ends the debugger with exit code `0`. See [DEBUGGER.md](docs/DEBUGGER.md).

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
include/coresim/   public headers (ALU, Architecture, CPU, CPUState, Memory, Register, Decoder, DecodedInstruction, Opcode, Loader, InstructionTable, Assembler, Disassembler, Debugger)
src/               implementation + main.cpp
tests/             test suites (TestSuite base class, one suite per component)
docs/              architecture documentation (Markdown + Mermaid)
diagrams/          draw.io diagrams (registers & memory, data flow, decoder, ISA)
examples/          example programs in assembly (.asm) and hex text format (.hex)
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
- [ASSEMBLER.md](docs/ASSEMBLER.md) – assembly language, `assemble()` and `loadAsm()`
- [DISASSEMBLER.md](docs/DISASSEMBLER.md) – instruction table, `disassemble()`
- [DEBUGGER.md](docs/DEBUGGER.md) – interactive debugger, its commands and `--debug`

Diagrams (open with [draw.io](https://www.drawio.com/)):

- [diagrams/registers-memory.drawio](diagrams/registers-memory.drawio) – pages *Registers* and *Memory*
- [diagrams/data-flow.drawio](diagrams/data-flow.drawio) – build targets, components, data flow, error handling
- [diagrams/decoder.drawio](diagrams/decoder.drawio) – format selection, bit layouts of R/I/M/J, shift + mask, `DecodedInstruction`, tested examples
- [diagrams/isa.drawio](diagrams/isa.drawio) – opcode map and opcode groups
