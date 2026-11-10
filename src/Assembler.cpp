#include "coresim/Assembler.h"

namespace coresim {

    ParsedLine parseLine(const std::string& line) {
        ParsedLine result{};

        // TODO: your logic, step by step
        //  1. cut off the comment from ';'
        //  2. find a label (text before ':')
        //  3. find the mnemonic (first word after the label)
        //  4. split the rest at ',' into operands, trim spaces and tabs

        std::string text = line;                    // 1. copy of line

        std::size_t comment     = text.find(';');       // 2. position of ';'
        std::size_t comment2    = text.find('#');
        if (comment != std::string::npos || comment != std::string::npos) {
            text.erase(comment);
        }
        return result;
    }

} // namespace coresim
