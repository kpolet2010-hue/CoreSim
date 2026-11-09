# Program loader

> **Specification, not implemented yet.** This document defines the hex text format for CoreSim programs and how `loadHex()` loads it into memory. Nothing in this file exists in the code yet.

## Why a text format

There is no assembler yet. A hex text file can be written in any editor, holds one instruction per line and allows comments, so a program stays readable without extra tools.

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
- Takes a `std::istream`, so tests can pass a `std::istringstream` instead of a real file. Opening the file is left to the caller, e.g. `main.cpp`.
