#include "test_cpu.h"
#include "test_memory.h"

#include <iostream>

int main() {
    int failures = 0;

    failures += MemoryTests().run();
    failures += CpuTests().run();

    if (failures == 0) {
        std::cout << "All tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
