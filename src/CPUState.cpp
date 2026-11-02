#include "coresim/CPUState.h"

namespace coresim {

    void CPUState::reset() {
        pc_ = 0;
        sp_ = 0;
        ir_ = 0;
        dbr_ = 0;
        flags_ = 0;
    }
} // namespace coresim