#include "test_alu.h"
#include "test_cpu.h"
#include "test_memory.h"
#include "test_register.h"
#include "test_cpu_state.h"
#include "test_decoder.h"
#include "test_isa.h"
#include "test_loader.h"
#include "test_examples.h"
#include "test_assembler.h"
#include "test_disassembler.h"
#include "test_debugger.h"
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
    failures += LoaderTests().run();
    failures += ExamplesTests().run();
    failures += AssemblerTests().run();
    failures += DisassemblerTests().run();
    failures += DebuggerTests().run();
    if (failures == 0) {
        std::cout << "All tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
