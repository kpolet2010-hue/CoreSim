#pragma once
#include "coresim/Memory.h"
#include <istream>

namespace coresim {
    std::size_t loadHex(std::istream& input, Memory& memory);

}