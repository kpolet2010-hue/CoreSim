#include "coresim/Loader.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace coresim {

    std::size_t loadHex(std::istream& input, Memory& memory) {
        std::size_t count = 0;
        std::size_t lineNumber = 0;     // counts every line, also comments and empty lines
        std::string line;

        while (std::getline(input, line)) {
            ++lineNumber;

            std::size_t comment = line.find(';');
            if (comment != std::string::npos) {
                line.erase(comment);
            }
            std::size_t first = line.find_first_not_of(" \t\r");
            if (first == std::string::npos) {
                continue;
            }
            std::size_t last = line.find_last_not_of(" \t\r");
            std::string text = line.substr(first, last - first + 1);
            if (text.rfind("0x", 0) == 0 || text.rfind("0X", 0) == 0) {
                text.erase(0, 2);
            }

            // exactly 8 hex digits, so a missing or extra digit is reported instead of loading a wrong value
            bool allHexDigits = std::all_of(text.begin(), text.end(),
                                            [](unsigned char c) { return std::isxdigit(c) != 0; });
            if (text.size() != 8 || !allHexDigits) {
                throw std::runtime_error("line " + std::to_string(lineNumber) + ": expected 8 hex digits");
            }

            std::uint32_t value = static_cast<std::uint32_t>(std::stoul(text, nullptr, 16));
            memory.write32(count * 4, value);
            ++count;
        }

        return count;
    }

} // namespace coresim
