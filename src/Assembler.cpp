#include "coresim/Assembler.h"
#include "coresim/InstructionTable.h"
#include "coresim/Opcode.h"

#include <cctype>
#include <cstdint>
#include <map>
#include <stdexcept>
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

        bool allDigits(const std::string& s, bool hex) {
            if (s.empty()) {
                return false;
            }
            for (char c : s) {
                unsigned char u = static_cast<unsigned char>(c);
                if (hex ? !std::isxdigit(u) : !std::isdigit(u)) {
                    return false;
                }
            }
            return true;
        }

        int registerNumber(const std::string& text) {
            if (text.size() < 2 || (text[0] != 'R' && text[0] != 'r')) {
                return -1;
            }
            std::string digits = text.substr(1);
            if (!allDigits(digits, false) || digits.size() > 2 || (digits.size() == 2 && digits[0] == '0')) {
                return -1;
            }
            int number = std::stoi(digits);
            return number < static_cast<int>(kRegisterCount) ? number : -1;
        }

        std::uint32_t parseRegister(const std::string& text) {
            int number = registerNumber(text);
            if (number >= 0) {
                return static_cast<std::uint32_t>(number);
            }
            if (!text.empty() && (text[0] == 'R' || text[0] == 'r')) {
                throw std::runtime_error("invalid register '" + text + "'");
            }
            throw std::runtime_error("expected a register, got '" + text + "'");
        }

        long long parseNumber(const std::string& text) {
            bool negative = !text.empty() && text[0] == '-';
            std::string digits = negative ? text.substr(1) : text;
            bool hex = !negative && digits.size() > 2 && digits[0] == '0' && (digits[1] == 'x' || digits[1] == 'X');
            if (hex) {
                digits.erase(0, 2);
            }
            if (!allDigits(digits, hex)) {
                throw std::runtime_error("expected a number, got '" + text + "'");
            }
            if (digits.size() > 8) {
                throw std::runtime_error("number out of range '" + text + "'");
            }
            long long value = std::stoll(digits, nullptr, hex ? 16 : 10);
            return negative ? -value : value;
        }

        long long parseNumberInRange(const std::string& text, long long min, long long max) {
            long long value = parseNumber(text);
            if (value < min || value > max) {
                throw std::runtime_error("number out of range '" + text + "'");
            }
            return value;
        }

        std::uint32_t parseMemory(const std::string& text) {
            if (text.size() < 2 || text.front() != '[' || text.back() != ']') {
                throw std::runtime_error("malformed memory operand '" + text + "'");
            }
            std::string inside = trim(text.substr(1, text.size() - 2));

            long long offset = 0;
            std::size_t sign = inside.find_first_of("+-");
            std::string base = trim(inside.substr(0, sign));
            if (sign != std::string::npos) {
                std::string number = trim(inside.substr(sign + 1));
                if (number.empty()) {
                    throw std::runtime_error("malformed memory operand '" + text + "'");
                }
                offset = parseNumber(number);
                if (inside[sign] == '-') {
                    offset = -offset;
                }
                if (offset < -32768 || offset > 32767) {
                    throw std::runtime_error("offset out of range '" + text + "'");
                }
            }

            std::uint32_t rs1 = parseRegister(base);
            return (rs1 << 16) | (static_cast<std::uint32_t>(offset) & 0xFFFF);
        }

        std::uint32_t parseTarget(const std::string& text, const std::map<std::string, Address>& labels) {
            if (!text.empty() && (std::isdigit(static_cast<unsigned char>(text[0])) || text[0] == '-')) {
                return static_cast<std::uint32_t>(parseNumberInRange(text, 0, kAddressMask));
            }
            auto label = labels.find(text);
            if (label == labels.end()) {
                throw std::runtime_error("unknown label '" + text + "'");
            }
            return label->second;
        }

        bool isValidLabel(const std::string& name) {
            if (name.empty() || std::isdigit(static_cast<unsigned char>(name[0]))) {
                return false;
            }
            for (char c : name) {
                if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
                    return false;
                }
            }
            return registerNumber(name) < 0 && findInstruction(name) == nullptr;
        }

        Instruction encode(const InstructionInfo& info, const ParsedLine& parsed,
                           const std::map<std::string, Address>& labels) {
            std::size_t expected = info.operands.size();
            if (parsed.operands.size() != expected) {
                throw std::runtime_error("'" + parsed.mnemonic + "' expects " + std::to_string(expected)
                                         + (expected == 1 ? " operand" : " operands") + ", got "
                                         + std::to_string(parsed.operands.size()));
            }

            Instruction word = static_cast<Instruction>(info.opcode) << 24;
            for (std::size_t i = 0; i < expected; ++i) {
                const std::string& text = parsed.operands[i];
                switch (info.operands[i]) {
                    case Operand::Rd:
                        word |= parseRegister(text) << 20;
                        break;
                    case Operand::Rs1:
                        word |= parseRegister(text) << 16;
                        break;
                    case Operand::Rs2:
                        word |= parseRegister(text) << 12;
                        break;
                    case Operand::Imm:
                        word |= static_cast<std::uint32_t>(parseNumberInRange(text, -32768, 65535)) & 0xFFFF;
                        break;
                    case Operand::Mem:
                        word |= parseMemory(text);
                        break;
                    case Operand::Target:
                        word |= parseTarget(text, labels);
                        break;
                }
            }
            return word;
        }

        std::runtime_error lineError(std::size_t lineNumber, const std::exception& error) {
            return std::runtime_error("line " + std::to_string(lineNumber) + ": " + error.what());
        }

        struct SourceInstruction {
            std::size_t lineNumber;
            const InstructionInfo* info;
            ParsedLine parsed;
        };

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
            result.label = trim(text.substr(0, colon));
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

    std::vector<Instruction> assemble(std::istream& source) {
        constexpr std::size_t kMaxInstructions = kMemorySize / kInstructionSize;

        std::vector<SourceInstruction> program;
        std::map<std::string, Address> labels;
        std::size_t lineNumber = 0;
        std::string line;

        while (std::getline(source, line)) {
            ++lineNumber;
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            try {
                ParsedLine parsed = parseLine(line);
                Address address = static_cast<Address>(program.size() * kInstructionSize);

                bool hasLabel = line.find(':') < line.find(';');
                if (hasLabel) {
                    if (!isValidLabel(parsed.label)) {
                        throw std::runtime_error("invalid label name '" + parsed.label + "'");
                    }
                    if (!labels.emplace(parsed.label, address).second) {
                        throw std::runtime_error("label '" + parsed.label + "' defined twice");
                    }
                }

                if (parsed.mnemonic.empty()) {
                    continue;
                }
                const InstructionInfo* info = findInstruction(parsed.mnemonic);
                if (info == nullptr) {
                    throw std::runtime_error("unknown instruction '" + parsed.mnemonic + "'");
                }
                if (program.size() == kMaxInstructions) {
                    throw std::runtime_error("program does not fit into memory");
                }
                program.push_back({lineNumber, info, parsed});
            } catch (const std::runtime_error& error) {
                throw lineError(lineNumber, error);
            }
        }

        std::vector<Instruction> instructions;
        instructions.reserve(program.size());
        for (const SourceInstruction& entry : program) {
            try {
                instructions.push_back(encode(*entry.info, entry.parsed, labels));
            } catch (const std::runtime_error& error) {
                throw lineError(entry.lineNumber, error);
            }
        }
        return instructions;
    }

    std::size_t loadAsm(std::istream& source, Memory& memory) {
        std::vector<Instruction> instructions = assemble(source);
        for (std::size_t i = 0; i < instructions.size(); ++i) {
            memory.write32(static_cast<Address>(i * kInstructionSize), instructions[i]);
        }
        return instructions.size();
    }

} // namespace coresim
