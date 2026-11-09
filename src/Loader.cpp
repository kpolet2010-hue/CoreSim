#include "coresim/Loader.h"

#include <string>

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
            ++count;
        }

        (void)memory;
        return count;
    }

} // namespace coresim
