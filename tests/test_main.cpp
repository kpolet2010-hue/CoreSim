#include "test_cpu.h"
#include "test_memory.h"
#include "test_register.h"
#include <iostream>

int main() {
    int failures = 0;

    // run all tests
    failures += MemoryTests().run();
    failures += RegisterTests().run();
    failures += CpuTests().run();
    
    if (failures == 0) {
        std::cout << "All tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
