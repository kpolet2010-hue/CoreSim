#pragma once

#include <cstdint>

#include "DecodedInstruction.h"

namespace coresim {

    class Decoder {
        public:
            DecodedInstruction decode(uint32_t instruction);
    };
}