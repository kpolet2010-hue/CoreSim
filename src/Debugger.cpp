#include "coresim/Debugger.h"

namespace coresim {

    Debugger::Debugger(CPU& cpu, std::ostream& output) : cpu_(cpu), output_(output) {}

    bool Debugger::execute(const std::string& line) {
        // TODO: split the line into command and arguments, run the command
        (void)line;
        return true;
    }

    void Debugger::run(std::istream& input) {
        // TODO: print the instruction line at PC, then prompt, read, execute
        (void)input;
    }

} // namespace coresim
