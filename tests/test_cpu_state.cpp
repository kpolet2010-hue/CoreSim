#include "test_cpu_state.h"

#include "coresim/CPUState.h"

using namespace coresim;

void CpuStateTests::runTests() {
    CPUState state;

    // initial values
    check(state.pc() == 0 && state.sp() == 0 && state.ir() == 0 && state.dbr() == 0 && 
        state.flags() == 0, 
        "state starts at zero");

    // pc / sp, 18 bit masking
    state.setPc(0x3FFFF);
    check(state.pc() == 0x3FFFF, "PC holds max 18-bit value");
    state.setPc(0x40000);
    check(state.pc() == 0, "PC is masked to 18 bits");
    state.setSp(0x3FFFF);
    check(state.sp() == 0x3FFFF, "SP holds max 18-bit value");
    state.setSp(0x40001);
    check(state.sp() == 1, "SP is masked to 18 bits");

    // dbr 2 bit, masking
    state.setDbr(3);
    check(state.dbr() == 3, "DBR holds 3");
    state.setDbr(4);
    check(state.dbr() == 0, "DBR is masked to 2 bits");

    // ir full 32 bits
    state.setIr(0xFFFFFFFF);
    check(state.ir() == 0xFFFFFFFF, "IR holds full 32 bits");

    // set each flag individually
    const Flag all[] = {kFlagZ, kFlagN, kFlagC, kFlagV};
    for (Flag f : all) {
        state.setFlag(f, true);
    }
    check(state.flags() == 0x0F, "all four flags set");

    // clearing one flag wont affect the others
    state.setFlag(kFlagC, false);
    check(!state.flag(kFlagC), "C cleared");
    check(state.flag(kFlagZ) && state.flag(kFlagN) && state.flag(kFlagV),
          "clearing C keeps Z, N, V");

    // reset clears everything
    state.setPc(0x100);
    state.reset();
    check(state.pc() == 0 && state.sp() == 0 && state.ir() == 0 &&
          state.dbr() == 0 && state.flags() == 0,
          "reset clears all state");
}