#include "test_examples.h"

#include "coresim/Assembler.h"
#include "coresim/CPU.h"
#include "coresim/Loader.h"
#include "coresim/Memory.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace coresim;

namespace {

    // result of loading and running one file from examples/
    struct ExampleRun {
        bool opened = false;
        std::size_t instructions = 0;
        std::vector<std::uint32_t> program;     // the loaded instructions, read back from memory
        bool halted = false;
        std::string output;
    };

    bool endsWith(const std::string& text, const std::string& end) {
        return text.size() >= end.size() && text.compare(text.size() - end.size(), end.size(), end) == 0;
    }

    // loads examples/<name> like coresim (.asm is assembled, .hex is loaded) and runs it with the same step limit
    ExampleRun runExample(const std::string& name) {
        ExampleRun result;
        std::ifstream file(std::string(CORESIM_EXAMPLES_DIR) + "/" + name);
        if (!file.is_open()) {
            return result;
        }
        result.opened = true;

        Memory memory;
        std::ostringstream output;
        CPU cpu(memory, output);
        result.instructions = endsWith(name, ".asm") ? loadAsm(file, memory) : loadHex(file, memory);
        for (std::size_t i = 0; i < result.instructions; ++i) {
            result.program.push_back(memory.read32(static_cast<Address>(i * 4)));
        }
        result.halted = cpu.run(1'000'000);
        result.output = output.str();
        return result;
    }

} // namespace

void ExamplesTests::runTests() {
    ExampleRun countdown = runExample("countdown.hex");
    check(countdown.opened, "countdown.hex is found");
    check(countdown.instructions == 5, "countdown.hex: 5 instructions");
    check(countdown.halted, "countdown.hex halts");
    check(countdown.output == "3\n2\n1\n", "countdown.hex prints 3, 2, 1");

    ExampleRun factorial = runExample("factorial.hex");
    check(factorial.opened, "factorial.hex is found");
    check(factorial.instructions == 7, "factorial.hex: 7 instructions");
    check(factorial.halted, "factorial.hex halts");
    check(factorial.output == "120\n", "factorial.hex prints 5! = 120");

    ExampleRun fibonacci = runExample("fibonacci.hex");
    check(fibonacci.opened, "fibonacci.hex is found");
    check(fibonacci.instructions == 10, "fibonacci.hex: 10 instructions");
    check(fibonacci.halted, "fibonacci.hex halts");
    check(fibonacci.output == "0\n1\n1\n2\n3\n5\n8\n13\n21\n34\n",
          "fibonacci.hex prints the first 10 Fibonacci numbers");

    ExampleRun squares = runExample("squares.hex");
    check(squares.opened, "squares.hex is found");
    check(squares.instructions == 12, "squares.hex: 12 instructions");
    check(squares.halted, "squares.hex halts");
    check(squares.output == "1\n4\n9\n16\n25\n", "squares.hex prints the squares of 1 to 5");

    // every .asm example assembles to exactly the hand-written .hex file and prints the same
    for (const std::string name : {"countdown", "factorial", "fibonacci", "squares"}) {
        ExampleRun hex = runExample(name + ".hex");
        ExampleRun assembled = runExample(name + ".asm");
        std::string prefix = name + ".asm";
        check(assembled.opened, (prefix + " is found").c_str());
        check(!assembled.program.empty() && assembled.program == hex.program,
              (prefix + " assembles to the same instructions as " + name + ".hex").c_str());
        check(assembled.halted && assembled.output == hex.output,
              (prefix + " halts and prints the same as " + name + ".hex").c_str());
    }

    // a missing file is reported, not loaded as an empty program
    check(!runExample("does-not-exist.hex").opened, "a missing example is reported as not found");
}
