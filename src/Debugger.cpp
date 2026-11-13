#include "coresim/Debugger.h"
#include "coresim/Disassembler.h"

#include <iomanip>
#include <cctype>
#include <exception>
#include <sstream>
#include <string>
#include <vector>
#include <exception>

namespace coresim {

    namespace {
        bool parseNumber(const std::string& text, long long& value) {
            bool negative = !text.empty() && text[0] == '-';
            std::string digits = negative ? text.substr(1) : text;

            int base = 10;
            if (digits.size() > 2 && digits[0] == '0' && (digits[1] == 'x' || digits[1] == 'X')) {
                base = 16;
                digits = digits.substr(2);        // "0x0C" -> "0C"
            }

            if (digits.empty() || digits.size() > 10) {
                return false;
            }
            for (char c : digits) {
                unsigned char u = static_cast<unsigned char>(c);
                bool valid = (base == 16) ? std::isxdigit(u) : std::isdigit(u);
                if (!valid) {
                    return false;
                }
            }

            value = std::stoll(digits, nullptr, base);
            if (negative) {
                value = -value;
            }
            return true;
        }
    } // namespace


    Debugger::Debugger(CPU& cpu, std::ostream& output) : cpu_(cpu), output_(output) {}

    void Debugger::printLine(Address address) {
        Instruction instruction = cpu_.memory().read32(address);

        output_ << (address == cpu_.state().pc() ? "> " : "  ");
        output_ << "0x" 
                << std::hex 
                << std::uppercase 
                << std::setfill('0')
                << std::setw(5) << address << "  "
                << std::setw(8) << instruction << "  "
                << std::dec << disassemble(instruction) << '\n';
    }


    bool Debugger::execute(const std::string& line) {
        std::istringstream stream(line);
        std::string command;
        std::vector<std::string> args;

        if (!(stream >> command)) {
            return true;
        }

        std::string word;
        while (stream >> word) {
            args.push_back(word);
        }

        std::string name = command;
        for (char& c : name) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        if (name == "quit" || name == "q") {
            return false;
        }

        if (name == "step" || name == "s") {
            if (args.size() > 1) {
                output_ << "error: usage: step [n]\n";
                return true;
            }

            long long count = 1;                          // default without argument
            if (args.size() == 1) {
                if (!parseNumber(args[0], count)) {
                    output_ << "error: invalid number '" << args[0] << "'\n";
                    return true;
                }
                if (count < 1) {
                    output_ << "error: invalid count '" << args[0] << "'\n";
                    return true;
                }
            }

            try {
                for (long long i = 0; i < count && !cpu_.halted(); ++i) {
                    cpu_.step();
                }
            } catch (const std::exception& error) {
                output_ << "error: " << error.what() << '\n';
                printLine(cpu_.state().pc());
                return true;
            }

            if (cpu_.halted()) {
                output_ << "halted\n";
            } else {
                printLine(cpu_.state().pc());
            }
            return true;
        }

        output_ << "error: unknown command '" << command << "'\n";
        return true;
    }


    void Debugger::run(std::istream& input) {
        printLine(cpu_.state().pc());

        std::string line;
        while (true) {
            output_ << "(coresim) ";                             // Prompt without '\n'
            if (!std::getline(input, line)) {
                break;                                          // End of input
            }
            if (!execute(line)) {
                break;                                          // quit
            }
        }
    }



} // namespace coresim
