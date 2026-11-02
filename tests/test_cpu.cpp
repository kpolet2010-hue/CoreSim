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

}
