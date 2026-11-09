#include "test_cpu.h"

#include "coresim/CPU.h"
#include "coresim/Memory.h"

using namespace coresim;

void CpuTests::runTests() {
    Memory memory;
    CPU cpu(memory);

    cpu.registers().set(15, 0xBEEF);
    check(cpu.registers().get(15) == 0xBEEF, "register set/get");
    cpu.reset();
    check(cpu.registers().get(15) == 0, "CPU reset clears registers");

    cpu.state().setPc(0x100);
    cpu.reset();
    check(cpu.state().pc() == 0, "CPU reset clears state");

    // step: fetch the instrcution at pc, sstore it in ir, advance pc by 4
    cpu.reset();
    memory.write32(0x0000, 0x12345678);
    memory.write32(0x0004, 0xCAFEBABE);

    cpu.step();
    check(cpu.state().ir() == 0x12345678, "step: IR holds the first instruction");
    check(cpu.state().pc() == 0x0004, "step: PC advances to 0x0004");

    cpu.step();
    check(cpu.state().ir() == 0xCAFEBABE, "step: IR holds the second instruction");
    check(cpu.state().pc() == 0x0008, "step: PC advances to 0x0008");   

    // NOP does nothing, HLT stops the cpu until reset
    cpu.reset();
    memory.write32(0x0000, 0x0D000000);                     // NOP
    memory.write32(0x0004, 0x0E000000);                     // HLT
    memory.write32(0x0008, 0x0D000000);                     // NOP, never reached

    cpu.step();
    check(!cpu.halted(), "NOP: cpu keeps running");
    check(cpu.state().pc() == 0x0004, "NOP: PC advances to 0x0004");

    cpu.step();
    check(cpu.halted(), "HLT: cpu is halted");
    check(cpu.state().pc() == 0x0008, "HLT: PC advances to 0x0008");

    cpu.step();
    check(cpu.state().pc() == 0x0008, "halted: step does not fetch, PC stays 0x0008");

    cpu.reset();
    check(!cpu.halted(), "reset clears halted");

    // MOVI loads an immediate, MOV copies a register, neither changes the flags
    cpu.reset();
    memory.write32(0x0000, 0x40101234);                     // MOVI R1, 0x1234
    memory.write32(0x0004, 0x00210000);                     // MOV  R2, R1
    memory.write32(0x0008, 0x0E000000);                     // HLT

    cpu.step();
    check(cpu.registers().get(1) == 0x1234, "MOVI: R1 = 0x1234");
    check(cpu.state().flags() == 0, "MOVI: flags unchanged");

    cpu.step();
    check(cpu.registers().get(2) == 0x1234, "MOV: R2 = 0x1234");
    check(cpu.registers().get(1) == 0x1234, "MOV: R1 still 0x1234 (copy, not move)");
    check(cpu.state().flags() == 0, "MOV: flags unchanged");

    cpu.step();
    check(cpu.halted(), "MOV program: cpu halts at HLT");

    // ADD stores the sum in rd and keeps its operands
    cpu.reset();
    memory.write32(0x0000, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0004, 0x40200007);                     // MOVI R2, 7
    memory.write32(0x0008, 0x01312000);                     // ADD R3, R1, R2

    cpu.step();
    check(cpu.registers().get(1) == 5, "MOVI: R1 = 5");
    check(cpu.state().flags() == 0, "MOVI: flags unchanged");

    cpu.step();
    check(cpu.registers().get(2) == 7, "MOVI: R2 = 7");
    check(cpu.registers().get(1) == 5, "MOVI: R1 still 5");

    cpu.step();
    check(cpu.registers().get(3) == 12, "ADD: R3 = 5 + 7 = 12");
    check(cpu.registers().get(1) == 5, "ADD: R1 unchanged");
    check(cpu.registers().get(2) == 7, "ADD: R2 unchanged");
    check(!cpu.state().flag(kFlagZ), "ADD: 5 + 7 clears Z");

    // ADD wraps around at 16 bits
    cpu.reset();
    memory.write32(0x0000, 0x4010FFFF);                     // MOVI R1, 0xFFFF
    memory.write32(0x0004, 0x40200001);                     // MOVI R2, 1
    memory.write32(0x0008, 0x01312000);                     // ADD R3, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0, "ADD: 0xFFFF + 1 wraps to 0");
    check(cpu.state().flag(kFlagZ), "ADD: 0xFFFF + 1 sets Z");
    check(cpu.state().flag(kFlagC), "ADD: 0xFFFF + 1 sets C");
    check(!cpu.state().flag(kFlagN), "ADD: 0xFFFF + 1 clears N");
    check(!cpu.state().flag(kFlagV), "ADD: 0xFFFF + 1 clears V");

    // ADD: signed overflow sets N and V
    cpu.reset();
    memory.write32(0x0000, 0x40107FFF);                     // MOVI R1, 0x7FFF
    memory.write32(0x0004, 0x40200001);                     // MOVI R2, 1
    memory.write32(0x0008, 0x01312000);                     // ADD R3, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0x8000, "ADD: 0x7FFF + 1 = 0x8000");
    check(!cpu.state().flag(kFlagZ), "ADD: 0x7FFF + 1 clears Z");
    check(cpu.state().flag(kFlagN), "ADD: 0x7FFF + 1 sets N");
    check(!cpu.state().flag(kFlagC), "ADD: 0x7FFF + 1 clears C");
    check(cpu.state().flag(kFlagV), "ADD: 0x7FFF + 1 sets V");

    // ADD: carry and signed overflow at the same time
    cpu.reset();
    memory.write32(0x0000, 0x40108000);                     // MOVI R1, 0x8000
    memory.write32(0x0004, 0x40208000);                     // MOVI R2, 0x8000
    memory.write32(0x0008, 0x01312000);                     // ADD R3, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0, "ADD: 0x8000 + 0x8000 wraps to 0");
    check(cpu.state().flag(kFlagZ), "ADD: 0x8000 + 0x8000 sets Z");
    check(!cpu.state().flag(kFlagN), "ADD: 0x8000 + 0x8000 clears N");
    check(cpu.state().flag(kFlagC), "ADD: 0x8000 + 0x8000 sets C");
    check(cpu.state().flag(kFlagV), "ADD: 0x8000 + 0x8000 sets V");

    // ADDI adds the immediate and keeps the source register
    cpu.reset();
    memory.write32(0x0000, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0004, 0x41210007);                     // ADDI R2, R1, 7

    cpu.step();
    cpu.step();
    check(cpu.registers().get(2) == 12, "ADDI: R2 = 5 + 7 = 12");
    check(cpu.registers().get(1) == 5, "ADDI: R1 unchanged");
    check(cpu.state().flags() == 0, "ADDI: 5 + 7 sets no flags");

    // ADDI: destination equals source, 0xFFFF acts as -1, result wraps to 0
    cpu.reset();
    memory.write32(0x0000, 0x40100001);                     // MOVI R1, 1
    memory.write32(0x0004, 0x4111FFFF);                     // ADDI R1, R1, 0xFFFF

    cpu.step();
    cpu.step();
    check(cpu.registers().get(1) == 0, "ADDI: 1 + 0xFFFF wraps to 0");
    check(cpu.state().flag(kFlagZ), "ADDI: 1 + 0xFFFF sets Z");
    check(!cpu.state().flag(kFlagN), "ADDI: 1 + 0xFFFF clears N");
    check(cpu.state().flag(kFlagC), "ADDI: 1 + 0xFFFF sets C");
    check(!cpu.state().flag(kFlagV), "ADDI: 1 + 0xFFFF clears V");

    // ADDI: signed overflow sets N and V
    cpu.reset();
    memory.write32(0x0000, 0x40107FFF);                     // MOVI R1, 0x7FFF
    memory.write32(0x0004, 0x41210001);                     // ADDI R2, R1, 1

    cpu.step();
    cpu.step();
    check(cpu.registers().get(2) == 0x8000, "ADDI: 0x7FFF + 1 = 0x8000");
    check(!cpu.state().flag(kFlagZ), "ADDI: 0x7FFF + 1 clears Z");
    check(cpu.state().flag(kFlagN), "ADDI: 0x7FFF + 1 sets N");
    check(!cpu.state().flag(kFlagC), "ADDI: 0x7FFF + 1 clears C");
    check(cpu.state().flag(kFlagV), "ADDI: 0x7FFF + 1 sets V");

    // SUB: equal operands give 0, the next SUB clears the old Z
    cpu.reset();
    memory.write32(0x0000, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0004, 0x40200005);                     // MOVI R2, 5
    memory.write32(0x0008, 0x02312000);                     // SUB R3, R1, R2
    memory.write32(0x000C, 0x40200003);                     // MOVI R2, 3
    memory.write32(0x0010, 0x02312000);                     // SUB R3, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0, "SUB: 5 - 5 = 0");
    check(cpu.registers().get(1) == 5, "SUB: R1 unchanged");
    check(cpu.registers().get(2) == 5, "SUB: R2 unchanged");
    check(cpu.state().flag(kFlagZ), "SUB: 5 - 5 sets Z");
    check(!cpu.state().flag(kFlagN), "SUB: 5 - 5 clears N");
    check(!cpu.state().flag(kFlagC), "SUB: 5 - 5 clears C");
    check(!cpu.state().flag(kFlagV), "SUB: 5 - 5 clears V");

    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 2, "SUB: 5 - 3 = 2");
    check(cpu.state().flags() == 0, "SUB: 5 - 3 clears the old Z, no flags set");

    // SUB: borrow wraps around at 16 bits
    cpu.reset();
    memory.write32(0x0000, 0x40100000);                     // MOVI R1, 0
    memory.write32(0x0004, 0x40200001);                     // MOVI R2, 1
    memory.write32(0x0008, 0x02312000);                     // SUB R3, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0xFFFF, "SUB: 0 - 1 wraps to 0xFFFF");
    check(!cpu.state().flag(kFlagZ), "SUB: 0 - 1 clears Z");
    check(cpu.state().flag(kFlagN), "SUB: 0 - 1 sets N");
    check(cpu.state().flag(kFlagC), "SUB: 0 - 1 sets C");
    check(!cpu.state().flag(kFlagV), "SUB: 0 - 1 clears V");

    // SUB: signed overflow sets V
    cpu.reset();
    memory.write32(0x0000, 0x40108000);                     // MOVI R1, 0x8000
    memory.write32(0x0004, 0x40200001);                     // MOVI R2, 1
    memory.write32(0x0008, 0x02312000);                     // SUB R3, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0x7FFF, "SUB: 0x8000 - 1 = 0x7FFF");
    check(!cpu.state().flag(kFlagZ), "SUB: 0x8000 - 1 clears Z");
    check(!cpu.state().flag(kFlagN), "SUB: 0x8000 - 1 clears N");
    check(!cpu.state().flag(kFlagC), "SUB: 0x8000 - 1 clears C");
    check(cpu.state().flag(kFlagV), "SUB: 0x8000 - 1 sets V");

    // SUB: destination equals source
    cpu.reset();
    memory.write32(0x0000, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0004, 0x40200003);                     // MOVI R2, 3
    memory.write32(0x0008, 0x02112000);                     // SUB R1, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(1) == 2, "SUB: R1 = 5 - 3 = 2 (rd equals rs1)");
    check(cpu.registers().get(2) == 3, "SUB: R2 unchanged (rd equals rs1)");

    // CMP: equal operands set Z, no register is written
    cpu.reset();
    memory.write32(0x0000, 0x40001234);                     // MOVI R0, 0x1234
    memory.write32(0x0004, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0008, 0x40200005);                     // MOVI R2, 5
    memory.write32(0x000C, 0x09012000);                     // CMP R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(0) == 0x1234, "CMP: R0 unchanged (unused rd field)");
    check(cpu.registers().get(1) == 5, "CMP: R1 unchanged");
    check(cpu.registers().get(2) == 5, "CMP: R2 unchanged");
    check(cpu.state().flag(kFlagZ), "CMP: 5, 5 sets Z");
    check(!cpu.state().flag(kFlagN), "CMP: 5, 5 clears N");
    check(!cpu.state().flag(kFlagC), "CMP: 5, 5 clears C");
    check(!cpu.state().flag(kFlagV), "CMP: 5, 5 clears V");

    // CMP: a < b sets N and C
    cpu.reset();
    memory.write32(0x0000, 0x40100003);                     // MOVI R1, 3
    memory.write32(0x0004, 0x40200005);                     // MOVI R2, 5
    memory.write32(0x0008, 0x09012000);                     // CMP R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(!cpu.state().flag(kFlagZ), "CMP: 3, 5 clears Z");
    check(cpu.state().flag(kFlagN), "CMP: 3, 5 sets N");
    check(cpu.state().flag(kFlagC), "CMP: 3, 5 sets C");
    check(!cpu.state().flag(kFlagV), "CMP: 3, 5 clears V");

    // CMP: a > b sets no flags
    cpu.reset();
    memory.write32(0x0000, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0004, 0x40200003);                     // MOVI R2, 3
    memory.write32(0x0008, 0x09012000);                     // CMP R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.state().flags() == 0, "CMP: 5, 3 sets no flags");

    // CMP: signed overflow sets V
    cpu.reset();
    memory.write32(0x0000, 0x40108000);                     // MOVI R1, 0x8000
    memory.write32(0x0004, 0x40200001);                     // MOVI R2, 1
    memory.write32(0x0008, 0x09012000);                     // CMP R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(!cpu.state().flag(kFlagZ), "CMP: 0x8000, 1 clears Z");
    check(!cpu.state().flag(kFlagN), "CMP: 0x8000, 1 clears N");
    check(!cpu.state().flag(kFlagC), "CMP: 0x8000, 1 clears C");
    check(cpu.state().flag(kFlagV), "CMP: 0x8000, 1 sets V");

    // CMP: a set rd field does not make it write a register
    cpu.reset();
    memory.write32(0x0000, 0x40301234);                     // MOVI R3, 0x1234
    memory.write32(0x0004, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0008, 0x40200003);                     // MOVI R2, 3
    memory.write32(0x000C, 0x09312000);                     // CMP R1, R2 with rd field = 3

    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0x1234, "CMP: R3 unchanged (rd field is ignored)");

    // MUL keeps the lower 16 bits and sets C if the product does not fit
    cpu.reset();
    memory.write32(0x0000, 0x4010FFFF);                     // MOVI R1, 0xFFFF
    memory.write32(0x0004, 0x4020FFFF);                     // MOVI R2, 0xFFFF
    memory.write32(0x0008, 0x03312000);                     // MUL R3, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 1, "MUL: 0xFFFF * 0xFFFF keeps lower 16 bits");
    check(cpu.registers().get(1) == 0xFFFF, "MUL: R1 unchanged");
    check(cpu.registers().get(2) == 0xFFFF, "MUL: R2 unchanged");
    check(cpu.state().flags() == kFlagC, "MUL: 0xFFFF * 0xFFFF sets only C");

    // DIV: unsigned division
    cpu.reset();
    memory.write32(0x0000, 0x4010FFFF);                     // MOVI R1, 0xFFFF
    memory.write32(0x0004, 0x40200002);                     // MOVI R2, 2
    memory.write32(0x0008, 0x04312000);                     // DIV R3, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0x7FFF, "DIV: 0xFFFF / 2 = 0x7FFF");
    check(cpu.registers().get(1) == 0xFFFF, "DIV: R1 unchanged");
    check(cpu.registers().get(2) == 2, "DIV: R2 unchanged");
    check(cpu.state().flags() == 0, "DIV: 0xFFFF / 2 sets no flags");

    // DIV by zero throws, rd and flags stay unchanged, PC is already advanced
    cpu.reset();
    memory.write32(0x0000, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0004, 0x40301234);                     // MOVI R3, 0x1234
    memory.write32(0x0008, 0x04312000);                     // DIV R3, R1, R2 (R2 = 0)

    cpu.step();
    cpu.step();
    cpu.state().setFlag(kFlagC, true);
    check(throwsRuntimeError([&] { cpu.step(); }), "DIV: division by zero throws runtime_error");
    check(cpu.registers().get(3) == 0x1234, "DIV: division by zero keeps R3");
    check(cpu.state().flags() == kFlagC, "DIV: division by zero keeps the flags");
    check(cpu.state().pc() == 0x000C, "DIV: division by zero has advanced PC");

    // AND, OR, XOR store their result and clear C and V left by the ADD before
    cpu.reset();
    memory.write32(0x0000, 0x40108000);                     // MOVI R1, 0x8000
    memory.write32(0x0004, 0x01411000);                     // ADD R4, R1, R1, sets C and V
    memory.write32(0x0008, 0x4010FF00);                     // MOVI R1, 0xFF00
    memory.write32(0x000C, 0x40200FF0);                     // MOVI R2, 0x0FF0
    memory.write32(0x0010, 0x05312000);                     // AND R3, R1, R2
    memory.write32(0x0014, 0x01411000);                     // ADD R4, R1, R1, sets C
    memory.write32(0x0018, 0x06312000);                     // OR R3, R1, R2
    memory.write32(0x001C, 0x01411000);                     // ADD R4, R1, R1, sets C
    memory.write32(0x0020, 0x07312000);                     // XOR R3, R1, R2

    cpu.step();
    cpu.step();
    check(cpu.state().flag(kFlagC) && cpu.state().flag(kFlagV), "ADD: 0x8000 + 0x8000 sets C and V before AND");
    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0x0F00, "AND: 0xFF00 & 0x0FF0 = 0x0F00");
    check(cpu.state().flags() == 0, "AND: clears the old C and V, no flags set");

    cpu.step();
    check(cpu.state().flag(kFlagC), "ADD: 0xFF00 + 0xFF00 sets C before OR");
    cpu.step();
    check(cpu.registers().get(3) == 0xFFF0, "OR: 0xFF00 | 0x0FF0 = 0xFFF0");
    check(cpu.state().flags() == kFlagN, "OR: clears the old C, sets only N");

    cpu.step();
    check(cpu.state().flag(kFlagC), "ADD: 0xFF00 + 0xFF00 sets C before XOR");
    cpu.step();
    check(cpu.registers().get(3) == 0xF0F0, "XOR: 0xFF00 ^ 0x0FF0 = 0xF0F0");
    check(cpu.state().flags() == kFlagN, "XOR: clears the old C, sets only N");
    check(cpu.registers().get(1) == 0xFF00, "AND, OR, XOR: R1 unchanged");
    check(cpu.registers().get(2) == 0x0FF0, "AND, OR, XOR: R2 unchanged");

    // NOT only reads rs1, a set rs2 field is ignored
    cpu.reset();
    memory.write32(0x0000, 0x40100F0F);                     // MOVI R1, 0x0F0F
    memory.write32(0x0004, 0x40201234);                     // MOVI R2, 0x1234
    memory.write32(0x0008, 0x08312000);                     // NOT R3, R1 with rs2 field = 2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(3) == 0xF0F0, "NOT: ~0x0F0F = 0xF0F0");
    check(cpu.registers().get(1) == 0x0F0F, "NOT: R1 unchanged");
    check(cpu.state().flags() == kFlagN, "NOT: ~0x0F0F sets only N");

    // destination equals source: operands are read before the result is written
    cpu.reset();
    memory.write32(0x0000, 0x40100006);                     // MOVI R1, 6
    memory.write32(0x0004, 0x40200007);                     // MOVI R2, 7
    memory.write32(0x0008, 0x03112000);                     // MUL R1, R1, R2

    cpu.step();
    cpu.step();
    cpu.step();
    check(cpu.registers().get(1) == 42, "MUL: R1 = 6 * 7 = 42 (rd equals rs1)");
    check(cpu.registers().get(2) == 7, "MUL: R2 unchanged (rd equals rs1)");

    // non-ALU instructions keep the flags, Z and C are set, N and V are cleared
    cpu.reset();
    memory.write32(0x0000, 0x40108000);                     // MOVI R1, 0x8000
    memory.write32(0x0004, 0x00210000);                     // MOV R2, R1
    memory.write32(0x0008, 0x80300100);                     // LOAD R3, [R0 + 0x100]
    memory.write32(0x000C, 0x81100100);                     // STORE R1, [R0 + 0x100]
    memory.write32(0x0010, 0x0A010000);                     // PUSH R1
    memory.write32(0x0014, 0x0B400000);                     // POP R4
    memory.write32(0x0018, 0x0F010000);                     // OUT R1
    memory.write32(0x001C, 0x0D000000);                     // NOP
    memory.write32(0x0020, 0x0E000000);                     // HLT

    const int kept = kFlagZ | kFlagC;
    cpu.state().setFlag(kFlagZ, true);
    cpu.state().setFlag(kFlagC, true);

    cpu.step();
    check(cpu.state().flags() == kept, "MOVI keeps the flags");
    cpu.step();
    check(cpu.state().flags() == kept, "MOV keeps the flags");
    cpu.step();
    check(cpu.state().flags() == kept, "LOAD keeps the flags");
    cpu.step();
    check(cpu.state().flags() == kept, "STORE keeps the flags");
    cpu.step();
    check(cpu.state().flags() == kept, "PUSH keeps the flags");
    cpu.step();
    check(cpu.state().flags() == kept, "POP keeps the flags");
    cpu.step();
    check(cpu.state().flags() == kept, "OUT keeps the flags");
    cpu.step();
    check(cpu.state().flags() == kept, "NOP keeps the flags");
    cpu.step();
    check(cpu.state().flags() == kept, "HLT keeps the flags");

    // JMP sets PC to the target, skips the next instruction and keeps registers and flags
    cpu.reset();
    memory.write32(0x0000, 0x40100005);                     // MOVI R1, 5
    memory.write32(0x0004, 0xC0000100);                     // JMP 0x00100
    memory.write32(0x0008, 0x40100BAD);                     // MOVI R1, 0x0BAD, skipped
    memory.write32(0x0100, 0x40200007);                     // MOVI R2, 7

    cpu.step();
    cpu.state().setFlag(kFlagZ, true);
    cpu.state().setFlag(kFlagC, true);

    cpu.step();
    check(cpu.state().ir() == 0xC0000100, "JMP: IR holds the jump");
    check(cpu.state().pc() == 0x0100, "JMP: PC = 0x00100");
    check(cpu.registers().get(1) == 5, "JMP: R1 unchanged");
    check(cpu.state().flags() == (kFlagZ | kFlagC), "JMP: flags unchanged");

    cpu.step();
    check(cpu.registers().get(2) == 7, "JMP: instruction at the target is executed");
    check(cpu.registers().get(1) == 5, "JMP: instruction after the jump is skipped");
    check(cpu.state().pc() == 0x0104, "JMP: PC continues after the target");

    // JMP ignores the 6 unused bits between opcode and address
    cpu.reset();
    memory.write32(0x0000, 0xC0FC0100);                     // JMP 0x00100, unused bits set

    cpu.step();
    check(cpu.state().pc() == 0x0100, "JMP: unused bits are ignored, PC = 0x00100");

    // JMP reaches the highest 4-byte aligned address
    cpu.reset();
    memory.write32(0x0000, 0xC003FFFC);                     // JMP 0x3FFFC

    cpu.step();
    check(cpu.state().pc() == 0x3FFFC, "JMP: PC = 0x3FFFC (full 18-bit address)");

    // JMP to its own address loops forever
    cpu.reset();
    memory.write32(0x0000, 0xC0000000);                     // JMP 0x00000

    cpu.step();
    check(cpu.state().pc() == 0x0000, "JMP to itself: PC = 0x00000");
    cpu.step();
    check(cpu.state().pc() == 0x0000, "JMP to itself: PC stays 0x00000");
}
