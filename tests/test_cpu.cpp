#include "test_cpu.h"

#include "coresim/CPU.h"
#include "coresim/Memory.h"

using namespace coresim;

void CpuTests::runTests() {
    Memory memory;
    CPU cpu(memory);

    cpu.reg(15).set(0xBEEF);
    check(cpu.reg(15).get() == 0xBEEF, "register set/get");
    cpu.reset();
    check(cpu.reg(15).get() == 0, "reset clears registers");
}
