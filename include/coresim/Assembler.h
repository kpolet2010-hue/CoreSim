#pragma once

#include "coresim/Architecture.h"
#include "coresim/Memory.h"

#include <cstddef>
#include <istream>
#include <string>
#include <vector>

namespace coresim {

    struct ParsedLine {
        std::string label;
        std::string mnemonic;
        std::vector<std::string> operands;
    };

    ParsedLine parseLine(const std::string& line);

    std::vector<Instruction> assemble(std::istream& source);

    std::size_t loadAsm(std::istream& source, Memory& memory);

} // namespace coresim
