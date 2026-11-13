# Disassembler

This document defines how the disassembler (`include/coresim/Disassembler.h`, `src/Disassembler.cpp`) turns a 32-bit instruction back into assembly text. Everything in it is implemented.

## Purpose

The disassembler is the reverse of the [assembler](ASSEMBLER.md):

```
0x4111FFFF   →   ADDI R1, R1, -1
```

It is needed to show what the CPU is executing, for example in a later debugger, without decoding hex values by hand.

## Interface

```cpp
// include/coresim/Disassembler.h
std::string disassemble(Instruction instruction);
```

- Takes one 32-bit instruction and returns one line of assembly text.
- Uses `Decoder` to split the instruction into its fields, so it decodes exactly like the CPU.
- Looks up the mnemonic and the operands by opcode with `findInstruction()` in the shared [instruction table](#instruction-table), the same table the assembler uses.
- Does not throw, also not for unknown opcodes (see [Unknown opcodes](#unknown-opcodes)).

## Instruction table

`include/coresim/InstructionTable.h` holds one entry per instruction with its name, its opcode and its operands in assembly order:

```cpp
enum class Operand { Rd, Rs1, Rs2, Imm, Mem, Target };

struct InstructionInfo {
    const char* name;
    Opcode opcode;
    std::vector<Operand> operands;
};

const InstructionInfo* findInstruction(const std::string& mnemonic);  // assembler
const InstructionInfo* findInstruction(Opcode opcode);                // disassembler
```

- Both functions return `nullptr` if there is no such instruction.
- The search by name ignores letter case.
- The assembler and the disassembler read the same table, so the operands of an instruction are defined only once.

## Output format

The text uses the syntax from [ASSEMBLER.md](ASSEMBLER.md#operands), so the assembler can read it again:

| Rule | Output |
|------|--------|
| Mnemonic | upper case, e.g. `ADDI` |
| Between mnemonic and operands | one space |
| Between operands | `, ` (comma and space) |
| Register | `R0` to `R15` |
| Instruction without operands | only the mnemonic, e.g. `HLT` |

Each instruction shows exactly the operands from the [ISA operand table](ISA.md#operands). Fields the instruction does not use are not shown.

### Numbers

| Field | Format | Examples |
|-------|--------|----------|
| `imm` (`MOVI`, `ADDI`) | signed decimal | `0x0003` → `3`, `0xFFFF` → `-1`, `0x8000` → `-32768` |
| `offset` (`LOAD`, `STORE`) | signed, inside the brackets | `[R1 + 8]`, `[R1 - 4]`, offset 0 → `[R1]` |
| jump `target` | hex, `0x` and 5 digits (18 bit) | `0x00004`, `0x3FFFF` |

- `imm` is shown signed because counters are usually written as `ADDI R1, R1, -1`. The assembler accepts `-1` and `65535` as the same 16 bits, so both forms give the same instruction.
- Jump targets are shown as numbers, because labels are not stored in the instruction.

### Examples

| Instruction | Text |
|-------------|------|
| `0x00210000` | `MOV R2, R1` |
| `0x40100003` | `MOVI R1, 3` |
| `0x01412000` | `ADD R4, R1, R2` |
| `0x4111FFFF` | `ADDI R1, R1, -1` |
| `0x09012000` | `CMP R1, R2` |
| `0x0A010000` | `PUSH R1` |
| `0x0B200000` | `POP R2` |
| `0x0F010000` | `OUT R1` |
| `0x80310008` | `LOAD R3, [R1 + 8]` |
| `0x8031FFFC` | `LOAD R3, [R1 - 4]` |
| `0x81210000` | `STORE R2, [R1]` |
| `0xC2000004` | `JNZ 0x00004` |
| `0xC5000100` | `CALL 0x00100` |
| `0x0E000000` | `HLT` |

## Round trip

For every valid instruction whose unused fields are `0`:

```
assemble(disassemble(x)) == x
```

Unused fields are not shown, so an instruction with set unused bits (e.g. `0x0E123456`, which the CPU executes as `HLT`) disassembles to `HLT` and assembles back to `0x0E000000`.

## Unknown opcodes

An opcode that is not one of the 26 (e.g. `0x10`) is shown as:

```
??? 0x10000000
```

The full instruction is printed as 8 hex digits, so it can still be identified. The assembler rejects this text.
