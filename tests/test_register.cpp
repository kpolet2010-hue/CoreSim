#include "test_register.h"
#include "coresim/Register.h"

using namespace coresim;

void RegisterTests::runTests() {
    Register regs;
    
    // define that all registers start at 0
    bool allZero = true;

    for (std::size_t i = 0; i < kRegisterCount; i++) {
        if (regs.get(i) != 0) {
            allZero = false;
        }
    }
    check(allZero, "registers start at zero");
    
    // R0 - R15 each keep their own values
    //  (R0 = 0x1000, R1 = 0x1001, ... R15 = 100F)
    for (std::size_t i = 0; i < kRegisterCount; i++) {
        regs.set(i, static_cast<Word>(0x1000 + i));
    }
    bool independent = true;
    for (std::size_t i = 0; i < kRegisterCount; i++) {
        if (regs.get(i) != 0x1000 + i) {
            independent = false;
        }
    }
    check(independent, "R0-R15 store independent values");

    // full 16 bit range
    regs.set(0, 0xFFFF);
    check(regs.get(0) == 0xFFFF, "register holds 0xFFFF");

    // reset clears all registers
    regs.reset();
    bool allCleared = true;
    for (std::size_t i = 0; i < kRegisterCount; i++) {
        if (regs.get(i) != 0) {
            allCleared = false;
        }
    }
    check(allCleared, "reset clears all registers");

    // invalid register index
    check(!throwsOutOfRange([&] { regs.get(15); }), "R15 is valid");
    check(throwsOutOfRange([&] {regs.get(16); }), "get R16 throws exception");
    check(throwsOutOfRange([&] { regs.set(16, 1); }), "set R16 throws exception");


}