# Program loader

> **Implemented** in `include/coresim/Loader.h` and `src/Loader.cpp`, tested in `tests/test_loader.cpp`. This document defines the hex text format for CoreSim programs and how `loadHex()` loads it into memory.

## Why a text format

A hex text file can be written in any editor, holds one instruction per line and allows comments, so a program stays readable without extra tools. It is the format of the encoded instructions; programs written in assembly are translated by the assembler instead (see [ASSEMBLER.md](ASSEMBLER.md)).

## Format

```
; countdown.hex - counts down from 3 to 1
40100003   ; MOVI R1, 3
0F010000   ; loop: OUT R1

4111FFFF   ;       ADDI R1, R1, -1
0xC2000004 ;       JNZ loop
0e000000   ; HLT
```

| Rule | Definition |
|------|------------|
| Instruction | exactly **8 hex digits** per line, an optional `0x` prefix is allowed |
| Letter case | ignored, `0E000000` and `0e000000` are the same |
| Comment | from `;` to the end of the line |
| Whitespace | spaces and tabs before and after the instruction are ignored |
| Empty lines | ignored, including lines that only hold a comment |
| Load address | the first instruction goes to `0x00000`, every further one 4 bytes after the previous one |
| Byte order | each instruction is stored with `Memory::write32`, so little-endian like every other instruction |

Exactly 8 digits are required so that a missing or extra digit is reported instead of silently loading a wrong instruction.

## Errors

| Line | Result |
|------|--------|
| `4010003` (7 digits) | error |
| `401000033` (9 digits) | error |
| `4010000G` (not a hex digit) | error |
| `40100003 0F010000` (two instructions) | error |
| `0x` (prefix without digits) | error |

An invalid line throws `std::runtime_error`. The message contains the line number, counted from 1, e.g. `line 3: expected 8 hex digits`.

- Instructions from the lines before the error have already been written to memory.
- A program with more instructions than fit into memory (more than 65,536) fails with `std::out_of_range` from `Memory::write32`.
- The loader does not clear memory. Bytes that are not part of the program keep their value. Call `Memory::reset()` first for a clean start.

## Interface

```cpp
// include/coresim/Loader.h
std::size_t loadHex(std::istream& input, Memory& memory);
```

- Reads the program from `input` and writes it to `memory`.
- Returns the number of loaded instructions.
- Takes a `std::istream`, so tests can pass a `std::istringstream` instead of a real file. Opening the file is left to the caller, `coresim` (`src/main.cpp`) does it with a `std::ifstream`. Example files are in [examples/](../examples/README.md).

```cpp
Memory memory;
std::ostringstream output;
CPU cpu(memory, output);

std::istringstream program("40100003\n0F010000\n4111FFFF\nC2000004\n0E000000\n");
loadHex(program, memory);   // returns 5
cpu.run(100);               // output.str() == "3\n2\n1\n"
```

## How a line is processed

1. Count the line, so the error message can name it.
2. Cut off everything from `;`.
3. Skip the line if only whitespace is left.
4. Trim spaces, tabs and `\r` at both ends.
5. Remove a `0x` or `0X` prefix.
6. Check that exactly 8 hex digits are left (`std::isxdigit`), otherwise throw.
7. Convert with `std::stoul(text, nullptr, 16)` and write the value with `write32` to `count * 4`.
