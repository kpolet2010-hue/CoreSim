#include "test_alu.h"
#include "test_cpu.h"
#include "test_memory.h"
#include "test_register.h"
#include "test_cpu_state.h"
#include "test_decoder.h"
#include "test_isa.h"
#include <iostream>

int main() {
    int failures = 0;

    // run all tests
    failures += MemoryTests().run();
    failures += RegisterTests().run();
    failures += AluTests().run();
    failures += CpuTests().run();
    failures += CpuStateTests().run();
    failures += DecoderTests().run();
    failures += IsaTests().run();
    if (failures == 0) {
        std::cout << "All tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
