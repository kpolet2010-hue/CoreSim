#include "coresim/Loader.h"

#include <string>
#include <cstdint>

namespace coresim {

    std::size_t loadHex(std::istream& input, Memory& memory) {
        std::size_t count = 0;
        std::string line;

        while (std::getline(input, line)) {

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

            }
            std::uint32_t value = static_cast<std::uint32_t>(std::stoul(text, nullptr, 16));
            memory.write32((count * 4) ,value);
            ++count;
        }

        return count;
    }

} // namespace coresim
