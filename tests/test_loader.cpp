#include "test_loader.h"

#include "coresim/CPU.h"
#include "coresim/Loader.h"
#include "coresim/Memory.h"

#include <sstream>

using namespace coresim;

void LoaderTests::runTests() {
    Memory memory;

    // the countdown example from LOADER.md: 5 instructions, a comment line and an empty line
    std::istringstream countdown(
        "; countdown.hex - counts down from 3 to 1\n"
        "40100003   ; MOVI R1, 3\n"
        "0F010000   ; loop: OUT R1\n"
        "\n"
        "4111FFFF   ;       ADDI R1, R1, -1\n"
        "0xC2000004 ;       JNZ loop\n"
        "0e000000   ; HLT\n");
    check(loadHex(countdown, memory) == 5, "countdown example: 5 instructions");

    // empty input
    std::istringstream empty("");
    check(loadHex(empty, memory) == 0, "empty input: 0 instructions");

    // only comments, empty lines and whitespace
    std::istringstream noInstructions(
        "; only a comment\n"
        "\n"
        "   \t  \n"
        "    ; indented comment\n");
    check(loadHex(noInstructions, memory) == 0, "comments and blank lines: 0 instructions");

    // spaces, tabs and Windows line endings around an instruction
    std::istringstream whitespace("  \t40100003\t ; MOVI R1, 3\r\n0E000000\r\n");
    check(loadHex(whitespace, memory) == 2, "spaces, tabs and \\r\\n are ignored: 2 instructions");

    // the last line does not need a newline
    std::istringstream noFinalNewline("40100003\n0E000000");
    check(loadHex(noFinalNewline, memory) == 2, "last line without newline is counted");

    // instructions are written one after another from 0x00000, little-endian
    memory.reset();
    std::istringstream program(
        "40100003   ; MOVI R1, 3\n"
        "; comment between instructions\n"
        "0F010000   ; OUT R1\n"
        "\n"
        "0e000000   ; HLT, lower case\n");
    loadHex(program, memory);
    check(memory.read32(0x0000) == 0x40100003, "1st instruction at 0x00000");
    check(memory.read32(0x0004) == 0x0F010000, "2nd instruction at 0x00004, comment line takes no space");
    check(memory.read32(0x0008) == 0x0E000000, "3rd instruction at 0x00008, lower case is accepted");
    check(memory.read8(0x0000) == 0x03, "instructions are stored little-endian");

    // the 0x prefix is accepted in both spellings
    memory.reset();
    std::istringstream prefix("0xC2000004\n0XABCDEF01\n");
    loadHex(prefix, memory);
    check(memory.read32(0x0000) == 0xC2000004, "0x prefix is accepted");
    check(memory.read32(0x0004) == 0xABCDEF01, "0X prefix is accepted");

    // the full 32-bit range
    memory.reset();
    std::istringstream extremes("00000000\nFFFFFFFF\n");
    loadHex(extremes, memory);
    check(memory.read32(0x0000) == 0x00000000, "00000000 is loaded");
    check(memory.read32(0x0004) == 0xFFFFFFFF, "FFFFFFFF is loaded");

    // the loader does not clear memory outside the program
    memory.reset();
    memory.write32(0x0100, 0x12345678);
    std::istringstream small("0E000000\n");
    loadHex(small, memory);
    check(memory.read32(0x0100) == 0x12345678, "memory after the program is not cleared");

    // load the countdown example and run it on the CPU
    Memory runMemory;
    std::ostringstream output;
    CPU cpu(runMemory, output);
    std::istringstream countdownProgram(
        "40100003   ; MOVI R1, 3\n"
        "0F010000   ; loop: OUT R1\n"
        "4111FFFF   ;       ADDI R1, R1, -1\n"
        "C2000004   ;       JNZ loop\n"
        "0E000000   ; HLT\n");
    loadHex(countdownProgram, runMemory);
    check(cpu.run(100), "loaded countdown halts");
    check(output.str() == "3\n2\n1\n", "loaded countdown prints 3, 2 and 1");
}
