#include "coresim/Assembler.h"
#include <string>

namespace coresim {

    namespace {

        std::string trim(const std::string& s) {
            std::size_t first = s.find_first_not_of(" \t");
            if (first == std::string::npos) {
                return "";
            }
            std::size_t last = s.find_last_not_of(" \t");
            return s.substr(first, last - first + 1);
        }

    } // namespace

    ParsedLine parseLine(const std::string& line) {
        ParsedLine result{};

        std::string text = line;
        std::size_t comment = text.find(';');
        if (comment != std::string::npos) {
            text.erase(comment);
        }
        std::size_t colon = text.find(':');
        if (colon != std::string::npos) {
            result.label = text.substr(0, colon);
            text = text.substr(colon + 1);
        }

        std::size_t start = text.find_first_not_of(" \t");
        if (start == std::string::npos) {
            return result;
        }
        std::size_t end = text.find_first_of(" \t", start);
        result.mnemonic = text.substr(start, end - start);

        if (end == std::string::npos) {
            return result;
        }
        std::string rest = text.substr(end);

        // only spaces after the mnemonic, e.g. "HLT ; stop": no operands
        if (trim(rest).empty()) {
            return result;
        }

        std::size_t comma = rest.find(',');
        while (comma != std::string::npos) {
            result.operands.push_back(trim(rest.substr(0, comma)));
            rest = rest.substr(comma + 1);
            comma = rest.find(',');
        }
        result.operands.push_back(trim(rest));

        return result;
    }

} // namespace coresim
