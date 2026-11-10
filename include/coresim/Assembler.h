#pragma once

#include <string>
#include <vector>

namespace coresim {

    struct ParsedLine {
        std::string label;                      
        std::string mnemonic;                   
        std::vector<std::string> operands;      
    };

    ParsedLine parseLine(const std::string& line);

} // namespace coresim
