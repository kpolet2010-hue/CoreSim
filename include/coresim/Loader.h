// Written by me; Claude (AI) reviewed and gave advice on how to improve or solve problems. See README "Use of AI tools".
#pragma once
#include "coresim/Memory.h"
#include <istream>

namespace coresim {
    std::size_t loadHex(std::istream& input, Memory& memory);

}