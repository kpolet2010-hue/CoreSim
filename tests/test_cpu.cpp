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

}
