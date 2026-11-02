#include "coresim/CPU.h"
#include "coresim/Memory.h"
#include "coresim/Register.h"

#include <iostream>

namespace {

int failures = 0;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

} // namespace

int main() {
    using namespace coresim;

    Memory memory;
    check(memory.size() == 256 * 1024, "memory is 256 KB");

    memory.write32(0x100, 0x12345678);
    check(memory.read8(0x100) == 0x78, "write32 is little-endian");
    check(memory.read32(0x100) == 0x12345678, "read32 round-trips");

    CPU cpu(memory);
    cpu.reg(15).set(0xBEEF);
    check(cpu.reg(15).get() == 0xBEEF, "register set/get");
    cpu.reset();
    check(cpu.reg(15).get() == 0, "reset clears registers");

    if (failures == 0) {
        std::cout << "All tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
