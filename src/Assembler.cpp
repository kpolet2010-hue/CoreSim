#include "coresim/Assembler.h"
#include <string>

namespace coresim {

    ParsedLine parseLine(const std::string& line) {
        ParsedLine result{};

        //  3. find the mnemonic (first word after the label)
        //  4. split the rest at ',' into operands, trim spaces and tabs

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
        return result;
    }

} // namespace coresim
