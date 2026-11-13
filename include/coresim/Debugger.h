#pragma once

#include "coresim/Architecture.h"
#include "coresim/CPU.h"

#include <iostream>
#include <set>
#include <string>

namespace coresim {

// interactive debugger for a cpu with a loaded program, see docs/DEBUGGER.md
    class Debugger {
        public:
            Debugger(CPU& cpu, std::ostream& output);

            // runs one command, returns false after quit
            bool execute(const std::string& line);
            // reads and runs commands until quit or end of input
            void run(std::istream& input);

        private:
            CPU& cpu_;
            std::ostream& output_;
            std::set<Address> breakpoints_;
    };

} // namespace coresim
