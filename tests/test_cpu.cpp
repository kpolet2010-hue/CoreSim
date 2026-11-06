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
}
