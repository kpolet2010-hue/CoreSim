#include "test_loader.h"

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
}
