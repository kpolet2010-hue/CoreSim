# Debugger

> **Specification, not implemented yet.** This document defines an interactive debugger for CoreSim-16. Nothing in it exists in the code so far.

## Purpose

`coresim` runs a program until `HLT` and only shows what `OUT` writes. The debugger runs the same program one instruction at a time, stops at breakpoints and shows registers, memory and the next instructions as assembly text (with the [disassembler](DISASSEMBLER.md)).

## Example session

`examples/countdown.asm`:

| Address | Instruction | Text |
|---------|-------------|------|
| `0x00000` | `40100003` | `MOVI R1, 3` |
| `0x00004` | `0F010000` | `OUT R1` |
| `0x00008` | `4111FFFF` | `ADDI R1, R1, -1` |
| `0x0000C` | `C2000004` | `JNZ 0x00004` |
| `0x00010` | `0E000000` | `HLT` |

```
$ coresim --debug examples/countdown.asm
CoreSim v0.1
> 0x00000  40100003  MOVI R1, 3
(coresim) break 0x0C
breakpoint at 0x0000C
(coresim) continue
3
breakpoint at 0x0000C
> 0x0000C  C2000004  JNZ 0x00004
(coresim) regs
R0  0x0000  R1  0x0002  R2  0x0000  R3  0x0000
R4  0x0000  R5  0x0000  R6  0x0000  R7  0x0000
R8  0x0000  R9  0x0000  R10 0x0000  R11 0x0000
R12 0x0000  R13 0x0000  R14 0x0000  R15 0x0000
PC  0x0000C  SP  0x00000  DBR 0  FLAGS ----
(coresim) step
> 0x00004  0F010000  OUT R1
(coresim) delete 0x0C
deleted breakpoint at 0x0000C
(coresim) continue
2
1
halted
(coresim) quit
```

## Interface

```cpp
// include/coresim/Debugger.h
class Debugger {
public:
    Debugger(CPU& cpu, std::ostream& output);

    bool execute(const std::string& line);   // runs one command, false after quit
    void run(std::istream& input);           // reads and runs commands until quit or end of input
};
```

- The debugger controls a `CPU` that already holds a loaded program. It reaches the memory through `cpu.memory()`.
- Everything the debugger prints goes to `output`. What `OUT` writes goes to the output stream of the CPU. In the command line both are `std::cout`, in the tests both can be the same `std::ostringstream`.
- `execute()` never throws. Errors are printed as `error: …` and the debugger keeps running.
- `run()` prints the [instruction line](#instruction-line) at PC once, then repeats: print the prompt `(coresim) ` (with a space, no newline), read one line, call `execute()`. It stops after `quit` or at the end of the input.

## Command line

```sh
coresim --debug examples/countdown.asm
```

`--debug` as the first argument starts the debugger instead of running the program. The file is the second argument, or the program asks for it as before. Loading works as without `--debug` (`.asm` is assembled, everything else is hex text) and load errors end the program with exit code `1` before the debugger starts. After `quit` or the end of the input, `coresim` exits with code `0`.

## Commands

| Command | Short | Action |
|---------|-------|--------|
| `step [n]` | `s` | executes `n` instructions (default 1) |
| `continue` | `c` | runs until `HLT`, a breakpoint or 1,000,000 steps |
| `break [address]` | `b` | sets a breakpoint, without an address lists all breakpoints |
| `delete <address>` | | removes a breakpoint |
| `regs` | `r` | shows R0–R15, PC, SP, DBR and the flags |
| `mem <address> [n]` | `m` | shows `n` 16-bit words from memory (default 4) |
| `disasm [address] [n]` | `d` | shows `n` instructions as assembly text (default: from PC, 5) |
| `help` | `h` | lists the commands |
| `quit` | `q` | ends the debugger |

- Command and arguments are separated by spaces or tabs. Letter case of the command is ignored (`STEP` = `step`).
- An empty line or a line with only spaces does nothing.
- Numbers are written like in the assembler: decimal (`12`) or hex (`0x0C`, `0X0C`), see [ASSEMBLER.md](ASSEMBLER.md#numbers).
- Addresses are always printed as `0x` and 5 uppercase hex digits.

### Instruction line

Several commands print instructions in the same form:

```
> 0x0000C  C2000004  JNZ 0x00004
  0x00010  0E000000  HLT
```

| Part | Format |
|------|--------|
| marker | `> ` if the address is the current PC, otherwise two spaces |
| address | `0x` and 5 hex digits |
| instruction | 8 hex digits, read with `read32` |
| text | `disassemble()` of the instruction |

The parts are separated by two spaces.

### step

- `n` must be at least 1.
- Executes up to `n` instructions. It stops early when the CPU halts. Breakpoints are ignored.
- Afterwards it prints `halted` if the CPU is halted, otherwise the instruction line at the new PC.
- On a halted CPU it executes nothing and prints `halted`.

### continue

- Always executes the instruction at PC first, so it can leave a breakpoint it stopped at.
- Then it continues until one of these happens:

| Stop | Output |
|------|--------|
| the CPU halts | `halted` |
| PC is on a breakpoint (the instruction there is not executed yet) | `breakpoint at 0x0000C`, then the instruction line |
| 1,000,000 instructions executed | `stopped after 1000000 steps`, then the instruction line |

- On a halted CPU it executes nothing and prints `halted`.

### Errors during step and continue

If the CPU throws (e.g. `DIV` by zero or a memory access outside the RAM), the debugger prints `error: <message>` and then the instruction line at PC, and waits for the next command.

### break and delete

- An address for a breakpoint must be a multiple of 4 and at most `0x3FFFC`.
- `break <address>` prints `breakpoint at 0x0000C`. Setting the same breakpoint again is allowed and prints the same.
- `break` without an address prints one line `breakpoint at 0x…` per breakpoint, sorted by address, or `no breakpoints`.
- `delete <address>` prints `deleted breakpoint at 0x0000C`, or `error: no breakpoint at 0x0000C` if there is none.

### regs

```
R0  0x0000  R1  0x0002  R2  0x0000  R3  0x0000
R4  0x0000  R5  0x0000  R6  0x0000  R7  0x0000
R8  0x0000  R9  0x0000  R10 0x0000  R11 0x0000
R12 0x0000  R13 0x0000  R14 0x0000  R15 0x0000
PC  0x0000C  SP  0x00000  DBR 0  FLAGS ----
```

- Four registers per line. Each entry is the name, padded with spaces to 3 characters, one space and the value as `0x` and 4 hex digits. Entries are separated by two spaces.
- PC and SP have 5 hex digits, DBR is a decimal digit.
- FLAGS shows `Z`, `N`, `C`, `V` in this order, a cleared flag as `-`. Example: `Z-C-` means Z and C are set.

### mem

```
(coresim) mem 0x100 3
0x00100  0005
0x00102  0003
0x00104  FFFF
```

- Shows `n` words of 16 bits read with `read16`, one per line, starting at `address` in steps of 2.
- `n` must be at least 1. All words must lie inside the memory (last word at `0x3FFFE` at most).

### disasm

```
(coresim) disasm 0x08 3
  0x00008  4111FFFF  ADDI R1, R1, -1
> 0x0000C  C2000004  JNZ 0x00004
  0x00010  0E000000  HLT
```

- Shows `n` instruction lines from `address` in steps of 4. The address must be a multiple of 4.
- `n` must be at least 1. The listing stops early at the end of the memory (last instruction at `0x3FFFC`).

### Error messages

| Problem | Output |
|---------|--------|
| unknown command | `error: unknown command 'foo'` |
| missing or extra arguments | `error: usage: <command from the table>`, e.g. `error: usage: mem <address> [n]` |
| an argument is not a number | `error: invalid number 'xyz'` |
| an address is out of range or not aligned | `error: invalid address '0x03'` |
| a count is 0 or negative | `error: invalid count '0'` |

The argument is printed as it was typed.

## Not part of this specification

- Watchpoints, conditional breakpoints
- Changing registers or memory from the debugger
- Repeating the last command with an empty line
- Breakpoints on labels (the assembler does not keep labels)
