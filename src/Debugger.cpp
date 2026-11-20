#include "coresim/Debugger.h"
#include "coresim/Disassembler.h"

#include <cctype>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace coresim {

    namespace {
        constexpr std::size_t kMaxSteps = 1'000'000;
        bool parseNumber(const std::string& text, long long& value) {
            bool negative = !text.empty() && text[0] == '-';
            std::string digits = negative ? text.substr(1) : text;

            int base = 10;
            if (digits.size() > 2 && digits[0] == '0' && (digits[1] == 'x' || digits[1] == 'X')) {
                base = 16;
                digits = digits.substr(2);                  // "0x0C" -> "0C"
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

        // "0x0000C"
        std::string formatAddress(Address address) {
            std::ostringstream text;
            text << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(5) << address;
            return text.str();
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
            return true;                                    // empty line
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

            long long count = 1;                            // default without argument
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

        if (name == "continue" || name == "c") {
            if (!args.empty()) {
                output_ << "error: usage: continue\n";
                return true;
            }
            if (cpu_.halted()) {
                output_ << "halted\n";
                return true;
            }

            std::size_t steps = 0;
            try {
                do {
                    cpu_.step();
                    ++steps;
                } while (!cpu_.halted()
                         && breakpoints_.count(cpu_.state().pc()) == 0
                         && steps < kMaxSteps);
            } catch (const std::exception& error) {
                output_ << "error: " << error.what() << '\n';
                printLine(cpu_.state().pc());
                return true;
            }

            if (cpu_.halted()) {
                output_ << "halted\n";
            } else if (breakpoints_.count(cpu_.state().pc()) > 0) {
                output_ << "breakpoint at " << formatAddress(cpu_.state().pc()) << '\n';
                printLine(cpu_.state().pc());
            } else {
                output_ << "stopped after " << kMaxSteps << " steps\n";
                printLine(cpu_.state().pc());
            }
            return true;
        }

        if (name == "regs" || name == "r") {
            if (!args.empty()) {
                output_ << "error: usage: regs\n";
                return true;
            }

            output_ << std::hex << std::uppercase << std::setfill('0');

            // R0-R15, four per line
            for (std::size_t i = 0; i < kRegisterCount; ++i) {
                std::string reg = "R" + std::to_string(i);
                if (reg.size() < 3) {
                    reg += ' ';                             // "R0" -> "R0 ", "R10" stays
                }
                output_ << reg << " 0x" << std::setw(4) << cpu_.registers().get(i);
                output_ << (i % 4 == 3 ? "\n" : "  ");      // after R3, R7, R11, R15: new line
            }

            std::string flags = "----";
            if (cpu_.state().flag(kFlagZ)) { flags[0] = 'Z'; }
            if (cpu_.state().flag(kFlagN)) { flags[1] = 'N'; }
            if (cpu_.state().flag(kFlagC)) { flags[2] = 'C'; }
            if (cpu_.state().flag(kFlagV)) { flags[3] = 'V'; }

            output_ << "PC  0x" << std::setw(5) << cpu_.state().pc()
                    << "  SP  0x" << std::setw(5) << cpu_.state().sp()
                    << "  DBR " << std::dec << static_cast<int>(cpu_.state().dbr())
                    << "  FLAGS " << flags << '\n';
            return true;
        }

        if (name == "break" || name == "b") {
            if (args.size() > 1) {
                output_ << "error: usage: break [address]\n";
                return true;
            }

            if (args.empty()) {
                if (breakpoints_.empty()) {
                    output_ << "no breakpoints\n";
                }
                for (Address address : breakpoints_) {
                    output_ << "breakpoint at " << formatAddress(address) << '\n';
                }
                return true;
            }

            long long value = 0;
            if (!parseNumber(args[0], value)) {
                output_ << "error: invalid number '" << args[0] << "'\n";
                return true;
            }
            if (value < 0 || value > 0x3FFFC || value % 4 != 0) {
                output_ << "error: invalid address '" << args[0] << "'\n";
                return true;
            }

            Address address = static_cast<Address>(value);
            breakpoints_.insert(address);
            output_ << "breakpoint at " << formatAddress(address) << '\n';
            return true;
        }

        if (name == "delete") {
            if (args.size() != 1) {
                output_ << "error: usage: delete <address>\n";
                return true;
            }

            long long value = 0;
            if (!parseNumber(args[0], value)) {
                output_ << "error: invalid number '" << args[0] << "'\n";
                return true;
            }

            Address address = static_cast<Address>(value);
            if (breakpoints_.erase(address) == 0) {
                output_ << "error: no breakpoint at " << formatAddress(address) << '\n';
            } else {
                output_ << "deleted breakpoint at " << formatAddress(address) << '\n';
            }
            return true;
        }

        if (name == "mem" || name == "m") {
            if (args.empty() || args.size() > 2) {
                output_ << "error: usage: mem <address> [n]\n";
                return true;
            }

            long long value = 0;
            if (!parseNumber(args[0], value)) {
                output_ << "error: invalid number '" << args[0] << "'\n";
                return true;
            }
            if (value < 0 || value > 0x3FFFC) {
                output_ << "error: invalid address '" << args[0] << "'\n";
                return true;
            }

            long long count = 4;                          // default without n
            if (args.size() == 2) {
                if (!parseNumber(args[1], count)) {
                    output_ << "error: invalid number '" << args[1] << "'\n";
                    return true;
                }
                if (count < 1) {
                    output_ << "error: invalid count '" << args[1] << "'\n";
                    return true;
                }
            }

            Address address = static_cast<Address>(value);
            for (long long i = 0; i < count && address <= 0x3FFFE; ++i) {
                output_ << formatAddress(address) << "  "
                        << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
                        << cpu_.memory().read16(address)
                        << std::dec << '\n';
                address += 2;
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
            output_ << "(coresim) ";                        // prompt without '\n'
            if (!std::getline(input, line)) {
                break;                                      // end of input
            }
            if (!execute(line)) {
                break;                                      // quit
            }
        }
    }

} // namespace coresim
