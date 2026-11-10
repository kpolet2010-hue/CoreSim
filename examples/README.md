# Examples

Every example program exists twice:

- **`.asm`** in assembly with labels (see [ASSEMBLER.md](../docs/ASSEMBLER.md)),
- **`.hex`** in the hex text format (see [LOADER.md](../docs/LOADER.md)). Every line holds one instruction, the comment next to it shows its address and the matching assembly.

Both versions contain exactly the same instructions. Run one from the project root:

```sh
./build/coresim examples/countdown.asm
./build/coresim examples/countdown.hex
```

| Program | What it does | Output | Instructions used |
|---------|--------------|--------|-------------------|
| countdown ([.asm](countdown.asm), [.hex](countdown.hex)) | counts down from 3 to 1 | `3` `2` `1` | `MOVI`, `OUT`, `ADDI`, `JNZ`, `HLT` |
| factorial ([.asm](factorial.asm), [.hex](factorial.hex)) | calculates 5! | `120` | `MOVI`, `MUL`, `ADDI`, `JNZ`, `OUT`, `HLT` |
| fibonacci ([.asm](fibonacci.asm), [.hex](fibonacci.hex)) | prints the first 10 Fibonacci numbers | `0` `1` `1` `2` `3` `5` `8` `13` `21` `34` | `MOVI`, `OUT`, `ADD`, `MOV`, `ADDI`, `JNZ`, `HLT` |
| squares ([.asm](squares.asm), [.hex](squares.hex)) | prints the squares of 1 to 5 with a subroutine | `1` `4` `9` `16` `25` | `MOVI`, `CALL`, `RET`, `PUSH`, `POP`, `MUL`, `CMP`, `JC`, `ADDI`, `OUT`, `HLT` |

Each number is printed on its own line. The test suite `Examples` (`tests/test_examples.cpp`) runs every program, checks this output and checks that each `.asm` file assembles to exactly the instructions of its `.hex` file, see [TESTING.md](../docs/TESTING.md#example-programs).

## How the loops work

- **countdown, factorial, fibonacci** count a register down with `ADDI rX, rX, -1` (`imm = 0xFFFF`). `ADDI` sets the Z flag when the counter reaches 0, so `JNZ` jumps back until then.
- **squares** counts up instead and compares with `CMP R1, R5`. `JC` jumps back while `R1 < R5` (borrow → C = 1).
- **squares** calls the subroutine `square` with `CALL`, which saves the return address on the stack. The subroutine saves `R2` with `PUSH` and restores it with `POP` before `RET`.

In the `.hex` files, jump targets are absolute addresses that have to be recalculated by hand when an instruction is added or removed. In the `.asm` files, the assembler calculates them from the labels.
