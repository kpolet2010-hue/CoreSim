#include "coresim/ALU.h"

#include <cstdint>
#include <stdexcept>

namespace coresim {

    namespace {

        // z and n only depend on the 16 bit result
        AluResult makeResult(Word value, bool carry = false, bool overflow = false) {
            return AluResult{value, value == 0, (value & 0x8000) != 0, carry, overflow, true};
        }

        AluResult add(Word a, Word b) {
            std::uint32_t sum = static_cast<std::uint32_t>(a) + b;
            Word result = static_cast<Word>(sum);
            return makeResult(result, sum > 0xFFFF, ((a ^ result) & (b ^ result) & 0x8000) != 0);
        }

        AluResult sub(Word a, Word b) {
            Word result = static_cast<Word>(a - b);
            return makeResult(result, a < b, ((a ^ b) & (a ^ result) & 0x8000) != 0);
        }

        AluResult mul(Word a, Word b) {
            // Word * Word would be calculated as int and 0xFFFF * 0xFFFF overflows int
            std::uint32_t product = static_cast<std::uint32_t>(a) * b;
            return makeResult(static_cast<Word>(product), product > 0xFFFF);
        }

    } // namespace

    AluResult ALU::execute(AluOp op, Word a, Word b) const {
        switch (op) {
            case AluOp::ADD:
            case AluOp::ADDI:
                return add(a, b);
            case AluOp::SUB:
                return sub(a, b);
            case AluOp::MUL:
                return mul(a, b);
            case AluOp::DIV:
                if (b == 0) {
                    throw std::runtime_error("Division by zero");
                }
                return makeResult(static_cast<Word>(a / b));
            case AluOp::CMP: {
                AluResult result = sub(a, b);
                result.writesResult = false;
                return result;
            }
            case AluOp::AND:
                return makeResult(static_cast<Word>(a & b));
            case AluOp::OR:
                return makeResult(static_cast<Word>(a | b));
            case AluOp::XOR:
                return makeResult(static_cast<Word>(a ^ b));
            case AluOp::NOT:
                return makeResult(static_cast<Word>(~a));
        }
        throw std::invalid_argument("Unknown ALU operation");
    }

} // namespace coresim
