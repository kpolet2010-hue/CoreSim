#pragma once

#include "coresim/Architecture.h"

#include <cstdint>

namespace coresim {
    // flag bits in flag register
    enum Flag : std::uint8_t {
        kFlagZ = 1 << 0, // Zero
        kFlagN = 1 << 1, // Negative
        kFlagC = 1 << 2, // Carry
        kFlagV = 1 << 3, // Overflow
    };

    class CPUState {
        public:
            void reset();

            Address pc() const { return pc_; }
            void setPc(Address value) { pc_ = value & kAddressMask; } // 18 bit

            Address sp() const { return sp_; }
            void setSp(Address value) {sp_ = value & kAddressMask; } // 18 bit

            std::uint8_t dbr() const { return dbr_; }
            void setDbr(std::uint8_t value) { dbr_ = value & 0x3; } // 2 bit

            Instruction ir() const { return ir_; }
            void setIr(Instruction value) { ir_ = value; } // 32 bit

            bool flag(Flag f) const { return (flags_ & f) != 0; }
            void setFlag(Flag f, bool on) {
                flags_ = on ? (flags_ | f) : (flags_ & ~f); 
            }
            std::uint8_t flags() const { return flags_; }
        
        private:
            Address pc_ = 0;
            Address sp_ = 0;
            Instruction ir_ = 0;
            std::uint8_t dbr_ = 0;
            std::uint8_t flags_ = 0;
    };
} // namespace coresim