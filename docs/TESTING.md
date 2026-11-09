# Testing

CoreSim uses a small self-written test framework with no external dependencies. All tests are compiled into one executable, `coresim_tests`, which is registered with CTest.

## Running

```sh
cmake --build build
ctest --test-dir build --output-on-failure   # summary via CTest
./build/tests/coresim_tests                  # every single check
```

The test binary prints each check and a summary per suite:

```
  PASS [Memory]: memory is 256 KB
  ...
[Memory] passed
[Registers] passed
[ALU] passed
[CPU] passed
[CPUState] passed
[Decoder] passed
[Isa] passed
All tests passed
```

Failed checks are printed as `FAIL [Suite]: message` on stderr, and the process exits with code `1`.

## Structure

```mermaid
classDiagram
    class TestSuite {
        <<abstract>>
        +run() int
        #runTests()*
        #check(condition, message)
        #throwsOutOfRange(func)$ bool
        #throwsRuntimeError(func)$ bool
    }
    TestSuite <|-- MemoryTests
    TestSuite <|-- RegisterTests
    TestSuite <|-- AluTests
    TestSuite <|-- CpuTests
    TestSuite <|-- CpuStateTests
    TestSuite <|-- DecoderTests
    TestSuite <|-- IsaTests
```

| File | Purpose |
|------|---------|
| `tests/TestSuite.h` | Base class: `check()`, `throwsOutOfRange()`, `throwsRuntimeError()`, `run()` |
| `tests/test_main.cpp` | Runs every suite and adds up the failures |
| `tests/test_<name>.h/.cpp` | One suite per component |

| Helper | Description |
|--------|-------------|
| `check(condition, message)` | Records a PASS or FAIL |
| `throwsOutOfRange(func)` | `true` if calling `func` throws `std::out_of_range` |
| `throwsRuntimeError(func)` | `true` if calling `func` throws `std::runtime_error` |
| `run()` | Calls `runTests()`, prints the summary, returns the number of failures |

## What is covered

| Suite | File | Covers |
|-------|------|--------|
| Memory | `test_memory.cpp` | Size, little-endian `write32`/`read32` and `write16`/`read16`, untouched neighbour bytes, odd addresses, out-of-bounds reads and writes, no partial write on failure |
| Registers | `test_register.cpp` | R0–R15 hold independent values, full 16-bit range, `reset()`, index 16 throws |
| ALU | `test_alu.cpp` | Result and Z, N, C, V of all nine operations without a CPU: normal values, zero results, `0xFFFF`, carry, borrow, signed overflow, `MUL` overflow, unsigned `DIV`, `DIV` by zero throws, `CMP` has no result to write, `NOT` ignores its second operand |
| CPU | `test_cpu.cpp` | Register access through the CPU, `CPU::reset()` clears registers and state, `step()` fetches into IR and advances PC by 4, `NOP` keeps running, `HLT` halts, a halted CPU does not fetch, `reset()` clears halted, `MOVI` loads an immediate, `MOV` copies a register and keeps the source, neither changes the flags, `ADD` and `ADDI` results, unchanged operands, 16-bit wrap-around, Z, N, C and V including signed overflow, `ADDI` with destination equal to source, `SUB` results, borrow, signed overflow, clearing an old Z, destination equal to source, `CMP` flags for equal, smaller and larger operands and signed overflow, `CMP` writes no register even with a set `rd` field, integration of `MUL`, `DIV`, `AND`, `OR`, `XOR` and `NOT` with the ALU, `DIV` by zero keeps `rd` and the flags and has advanced PC, `MOV`, `MOVI`, `LOAD`, `STORE`, `PUSH`, `POP`, `OUT`, `NOP` and `HLT` keep the flags, `JMP` skips the next instruction and continues at the target, ignores the unused bits and reaches `0x3FFFC`, `JMP` to itself, `JZ`, `JNZ`, `JN` and `JC` taken and not taken after `CMP`, jumps keep the flags, `JN` does not jump after a signed overflow, `LOAD` and `STORE` with positive and negative offsets, DBR banks, wrap-around inside the bank, `rd` equal to `rs1`, a `STORE`/`LOAD` round-trip, an access at `0x3FFFF` throws and keeps register and memory unchanged, neither changes the flags, `PUSH` wraps SP from `0x00000` to `0x3FFFE` and ignores the `rd` field and DBR, `POP` after `PUSH` and last-in-first-out order, `CALL` with the example from `ISA.md`, nested `CALL`/`RET`, `RET` masks the stored value to 18 bits, failing stack accesses throw and keep SP, PC and registers, the stack instructions keep the flags, `OUT` writes unsigned decimal lines into a captured `std::ostringstream` and ignores the `rd` field, the default output stream is `std::cout` |
| CPUState | `test_cpu_state.cpp` | Initial values, 18-bit masking of PC/SP, 2-bit masking of DBR, 32-bit IR, setting/clearing single flags, `reset()` |
| Decoder | `test_decoder.cpp` | One case per format (R `0x12345000`, I `0x4123ABCD`, M `0x8012FFFC`/`0x80120008`, J `0xC0012345`/`0xFFFFFFFF`), negative and positive offset, 18-bit address masking |
| Isa | `test_isa.cpp` | Value of all 26 opcodes, each opcode lies in the range of its format (`opcode >> 6`), operand encoding of `STORE`, `LOAD`, `PUSH`, `POP`, `MOVI`, `CALL` and `HLT` |

## Adding a suite

1. Create `tests/test_<name>.h` with a class deriving from `TestSuite` and overriding `runTests()`.
2. Write the checks in `tests/test_<name>.cpp`.
3. Add the `.cpp` file to `tests/CMakeLists.txt`.
4. Include the header in `tests/test_main.cpp` and add `failures += <Name>Tests().run();`.

## Continuous integration

`.github/workflows/ci.yml` builds and tests the project on every push and pull request to `main`, and can be started manually.

```mermaid
flowchart LR
    T["push / PR to main"] --> C["Checkout"]
    C --> I["Install Ninja + ccache"]
    I --> CFG["cmake -G Ninja<br/>Release"]
    CFG --> B["cmake --build"]
    B --> TEST["ctest --output-on-failure"]
```

The job runs on `ubuntu-latest` and fails if the build or any test fails. Compiler output is cached with ccache between runs.
