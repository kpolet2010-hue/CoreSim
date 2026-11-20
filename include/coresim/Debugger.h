#pragma once

#include "coresim/Architecture.h"
#include "coresim/CPU.h"

#include <iostream>
#include <set>
#include <string>

namespace coresim {

    class Debugger {
        public:
            Debugger(CPU& cpu, std::ostream& output);

            bool execute(const std::string& line);
            void run(std::istream& input);

        private:
            CPU& cpu_;
            std::ostream& output_;
            std::set<Address> breakpoints_;
            void printLine(Address address);   // "> 0x0000C  C2000004  JNZ 0x00004"
    };

} // namespace coresim
