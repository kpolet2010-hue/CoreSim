#include "test_debugger.h"

#include "coresim/Assembler.h"
#include "coresim/CPU.h"
#include "coresim/Debugger.h"
#include "coresim/Memory.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

using namespace coresim;

namespace {

    const char* const kCountdown =
        "        MOVI R1, 3\n"        // 0x00000
        "loop:   OUT  R1\n"           // 0x00004
        "        ADDI R1, R1, -1\n"   // 0x00008
        "        JNZ  loop\n"         // 0x0000C
        "        HLT\n";              // 0x00010

    // memory, cpu and debugger with one shared output stream
    struct Session {
        Memory memory;
        std::ostringstream out;
        CPU cpu;
        Debugger debugger;
        bool lastResult = true;

        explicit Session(const std::string& source = kCountdown) : cpu(memory, out), debugger(cpu, out) {
            std::istringstream input(source);
            std::vector<Instruction> program = assemble(input);
            for (std::size_t i = 0; i < program.size(); ++i) {
                memory.write32(static_cast<Address>(i * 4), program[i]);
            }
        }

        // runs one command and returns only what it printed
        std::string execute(const std::string& line) {
            out.str("");
            out.clear();
            lastResult = debugger.execute(line);
            return out.str();
        }
    };

    // shows newlines as \n so a message stays on one line
    std::string visible(const std::string& text) {
        std::string result;
        for (char c : text) {
            result += (c == '\n') ? std::string("\\n") : std::string(1, c);
        }
        return result;
    }

    std::string describe(const std::string& label, const std::string& actual, const std::string& expected) {
        std::string message = label + " -> '" + visible(expected) + "'";
        if (actual != expected) {
            message += " (got '" + visible(actual) + "')";
        }
        return message;
    }

    const std::string kLine0 = "> 0x00000  40100003  MOVI R1, 3\n";
    const std::string kLine4 = "> 0x00004  0F010000  OUT R1\n";
    const std::string kLine8 = "> 0x00008  4111FFFF  ADDI R1, R1, -1\n";
    const std::string kLineC = "> 0x0000C  C2000004  JNZ 0x00004\n";

    const std::string kRegsStart =
        "R0  0x0000  R1  0x0000  R2  0x0000  R3  0x0000\n"
        "R4  0x0000  R5  0x0000  R6  0x0000  R7  0x0000\n"
        "R8  0x0000  R9  0x0000  R10 0x0000  R11 0x0000\n"
        "R12 0x0000  R13 0x0000  R14 0x0000  R15 0x0000\n"
        "PC  0x00000  SP  0x00000  DBR 0  FLAGS ----\n";

} // namespace

void DebuggerTests::runTests() {
    // runs line in session and checks the printed text
    auto expect = [this](Session& session, const std::string& line, const std::string& expected) {
        std::string actual = session.execute(line);
        check(actual == expected, describe("'" + line + "'", actual, expected).c_str());
    };

    // ---- quit, empty lines, unknown commands ----
    {
        Session s;
        check(s.execute("quit").empty() && !s.lastResult, "quit returns false and prints nothing");
        s.execute("q");
        check(!s.lastResult, "q returns false");
        s.execute("QUIT");
        check(!s.lastResult, "QUIT returns false (letter case ignored)");
        expect(s, "", "");
        check(s.lastResult, "empty line returns true");
        expect(s, "   \t ", "");
        check(s.lastResult, "line with only spaces and tabs returns true");
        expect(s, "foo", "error: unknown command 'foo'\n");
        check(s.lastResult, "unknown command returns true");
        check(s.cpu.state().pc() == 0, "commands without execution keep PC at 0");
    }

    // ---- step ----
    {
        Session s;
        expect(s, "step", kLine4);
        check(s.lastResult, "step returns true");
        expect(s, "s", kLine8);
        expect(s, "STEP", kLineC);
    }
    {
        Session s;
        expect(s, "step 2", "3\n" + kLine8);
        check(s.cpu.state().pc() == 0x08, "step 2 executes two instructions");
    }
    {
        Session s;
        expect(s, "step 0x2", "3\n" + kLine8);
        expect(s, "  step\t1  ", kLineC);
    }
    {
        Session s;
        expect(s, "step 100", "3\n2\n1\nhalted\n");
        check(s.cpu.halted(), "step 100 stops at HLT");
        Address pc = s.cpu.state().pc();
        expect(s, "step", "halted\n");
        check(s.cpu.state().pc() == pc, "step on a halted CPU executes nothing");
    }
    {
        Session s;
        expect(s, "break 0x04", "breakpoint at 0x00004\n");
        expect(s, "step 2", "3\n" + kLine8);
    }
    {
        Session s;
        expect(s, "step 0", "error: invalid count '0'\n");
        expect(s, "step -1", "error: invalid count '-1'\n");
        expect(s, "step abc", "error: invalid number 'abc'\n");
        expect(s, "step 1 2", "error: usage: step [n]\n");
        check(s.cpu.state().pc() == 0, "invalid step executes nothing");
    }

    // ---- continue ----
    {
        Session s;
        expect(s, "break 0x0C", "breakpoint at 0x0000C\n");
        expect(s, "continue", "3\nbreakpoint at 0x0000C\n" + kLineC);
        check(s.lastResult, "continue returns true");
        check(s.cpu.state().pc() == 0x0C, "continue stops before the breakpoint instruction");
        expect(s, "c", "2\nbreakpoint at 0x0000C\n" + kLineC);
        expect(s, "delete 0x0C", "deleted breakpoint at 0x0000C\n");
        expect(s, "continue", "1\nhalted\n");
        check(s.cpu.halted(), "continue runs until HLT");
        expect(s, "continue", "halted\n");
    }
    {
        Session s;
        expect(s, "break 0", "breakpoint at 0x00000\n");
        expect(s, "continue", "3\n2\n1\nhalted\n");
    }
    {
        Session s("loop: JMP loop\n");
        expect(s, "continue", "stopped after 1000000 steps\n> 0x00000  C0000000  JMP 0x00000\n");
    }
    {
        Session s;
        expect(s, "continue now", "error: usage: continue\n");
        check(s.cpu.state().pc() == 0, "continue with arguments executes nothing");
    }

    // ---- errors from the CPU ----
    {
        Session s("MOVI R1, 1\nMOVI R2, 0\nDIV R3, R1, R2\nHLT\n");
        expect(s, "step 3", "error: Division by zero\n> 0x0000C  0E000000  HLT\n");
        check(s.lastResult, "CPU error returns true");
    }
    {
        Session s("MOVI R1, 1\nMOVI R2, 0\nDIV R3, R1, R2\nHLT\n");
        expect(s, "continue", "error: Division by zero\n> 0x0000C  0E000000  HLT\n");
        expect(s, "continue", "halted\n");
    }

    // ---- break and delete ----
    {
        Session s;
        expect(s, "break", "no breakpoints\n");
        expect(s, "b 0x10", "breakpoint at 0x00010\n");
        expect(s, "break 4", "breakpoint at 0x00004\n");
        expect(s, "break 0X04", "breakpoint at 0x00004\n");
        expect(s, "break", "breakpoint at 0x00004\nbreakpoint at 0x00010\n");
        expect(s, "break 0x3FFFC", "breakpoint at 0x3FFFC\n");
        expect(s, "break 0x03", "error: invalid address '0x03'\n");
        expect(s, "break 0x40000", "error: invalid address '0x40000'\n");
        expect(s, "break -4", "error: invalid address '-4'\n");
        expect(s, "break xyz", "error: invalid number 'xyz'\n");
        expect(s, "break 4 8", "error: usage: break [address]\n");
        expect(s, "break", "breakpoint at 0x00004\nbreakpoint at 0x00010\nbreakpoint at 0x3FFFC\n");
    }
    {
        Session s;
        expect(s, "break 0x0C", "breakpoint at 0x0000C\n");
        expect(s, "delete 12", "deleted breakpoint at 0x0000C\n");
        expect(s, "delete 0x0C", "error: no breakpoint at 0x0000C\n");
        expect(s, "break", "no breakpoints\n");
        expect(s, "delete", "error: usage: delete <address>\n");
        expect(s, "delete abc", "error: invalid number 'abc'\n");
    }

    // ---- regs ----
    {
        Session s;
        expect(s, "regs", kRegsStart);
        expect(s, "r", kRegsStart);
        expect(s, "regs all", "error: usage: regs\n");
    }
    {
        Session s;
        s.execute("step 3");
        expect(s, "regs",
               "R0  0x0000  R1  0x0002  R2  0x0000  R3  0x0000\n"
               "R4  0x0000  R5  0x0000  R6  0x0000  R7  0x0000\n"
               "R8  0x0000  R9  0x0000  R10 0x0000  R11 0x0000\n"
               "R12 0x0000  R13 0x0000  R14 0x0000  R15 0x0000\n"
               "PC  0x0000C  SP  0x00000  DBR 0  FLAGS ----\n");
    }
    {
        Session s;
        s.cpu.registers().set(10, 0xABCD);
        s.cpu.registers().set(15, 0xFFFF);
        s.cpu.state().setPc(0x3FFFC);
        s.cpu.state().setSp(0x3FFFE);
        s.cpu.state().setDbr(3);
        s.cpu.state().setFlag(kFlagZ, true);
        s.cpu.state().setFlag(kFlagC, true);
        expect(s, "regs",
               "R0  0x0000  R1  0x0000  R2  0x0000  R3  0x0000\n"
               "R4  0x0000  R5  0x0000  R6  0x0000  R7  0x0000\n"
               "R8  0x0000  R9  0x0000  R10 0xABCD  R11 0x0000\n"
               "R12 0x0000  R13 0x0000  R14 0x0000  R15 0xFFFF\n"
               "PC  0x3FFFC  SP  0x3FFFE  DBR 3  FLAGS Z-C-\n");
        s.cpu.state().setFlag(kFlagN, true);
        s.cpu.state().setFlag(kFlagV, true);
        std::string all = s.execute("regs");
        check(all.find("FLAGS ZNCV\n") != std::string::npos, describe("all flags set", all, "... FLAGS ZNCV\\n").c_str());
        s.cpu.state().setFlag(kFlagZ, false);
        s.cpu.state().setFlag(kFlagC, false);
        std::string nv = s.execute("regs");
        check(nv.find("FLAGS -N-V\n") != std::string::npos, describe("N and V set", nv, "... FLAGS -N-V\\n").c_str());
    }

    // ---- mem ----
    {
        Session s;
        s.memory.write16(0x100, 0x0005);
        s.memory.write16(0x102, 0x0003);
        s.memory.write16(0x104, 0xFFFF);
        expect(s, "mem 0x100 3", "0x00100  0005\n0x00102  0003\n0x00104  FFFF\n");
        expect(s, "m 256", "0x00100  0005\n0x00102  0003\n0x00104  FFFF\n0x00106  0000\n");
        expect(s, "mem 0x101 1", "0x00101  0300\n");
        expect(s, "mem 0 2", "0x00000  0003\n0x00002  4010\n");
        expect(s, "mem 0x3FFFE 1", "0x3FFFE  0000\n");
        expect(s, "mem 0x3FFFC", "0x3FFFC  0000\n0x3FFFE  0000\n");
        expect(s, "mem 0x3FFFF 1", "error: invalid address '0x3FFFF'\n");
        expect(s, "mem 0x40000", "error: invalid address '0x40000'\n");
        expect(s, "mem 0x100 0", "error: invalid count '0'\n");
        expect(s, "mem zz", "error: invalid number 'zz'\n");
        expect(s, "mem 0x100 x", "error: invalid number 'x'\n");
        expect(s, "mem", "error: usage: mem <address> [n]\n");
        expect(s, "mem 1 2 3", "error: usage: mem <address> [n]\n");
    }

    // ---- disasm ----
    {
        Session s;
        const std::string listing =
            "> 0x00000  40100003  MOVI R1, 3\n"
            "  0x00004  0F010000  OUT R1\n"
            "  0x00008  4111FFFF  ADDI R1, R1, -1\n"
            "  0x0000C  C2000004  JNZ 0x00004\n"
            "  0x00010  0E000000  HLT\n";
        expect(s, "disasm", listing);
        expect(s, "d", listing);
        s.execute("step");
        expect(s, "disasm 0 2", "  0x00000  40100003  MOVI R1, 3\n> 0x00004  0F010000  OUT R1\n");
    }
    {
        Session s;
        s.execute("break 0x0C");
        s.execute("continue");
        expect(s, "disasm 0x08 3",
               "  0x00008  4111FFFF  ADDI R1, R1, -1\n"
               "> 0x0000C  C2000004  JNZ 0x00004\n"
               "  0x00010  0E000000  HLT\n");
        expect(s, "disasm 0x10 1", "  0x00010  0E000000  HLT\n");
    }
    {
        Session s;
        s.memory.write32(0x20, 0xFFFFFFFF);
        expect(s, "disasm 0x20 1", "  0x00020  FFFFFFFF  ??? 0xFFFFFFFF\n");
        expect(s, "disasm 0x3FFFC 5", "  0x3FFFC  00000000  MOV R0, R0\n");
        expect(s, "disasm 0x02", "error: invalid address '0x02'\n");
        expect(s, "disasm 0x40000", "error: invalid address '0x40000'\n");
        expect(s, "disasm 0 0", "error: invalid count '0'\n");
        expect(s, "disasm abc", "error: invalid number 'abc'\n");
        expect(s, "disasm 0 1 2", "error: usage: disasm [address] [n]\n");
    }

    // ---- help ----
    {
        Session s;
        std::string help = s.execute("help");
        check(s.lastResult, "help returns true");
        for (const char* command : {"step", "continue", "break", "delete", "regs", "mem", "disasm", "help", "quit"}) {
            check(help.find(command) != std::string::npos, ("help lists " + std::string(command)).c_str());
        }
        check(s.execute("h") == help, "h prints the same as help");
    }

    // ---- run ----
    {
        Session s;
        std::istringstream input("step\nquit\n");
        s.debugger.run(input);
        std::string expected = kLine0 + "(coresim) " + kLine4 + "(coresim) ";
        check(s.out.str() == expected, describe("run: step, quit", s.out.str(), expected).c_str());
    }
    {
        Session s;
        std::istringstream input("quit\nstep\n");
        s.debugger.run(input);
        check(s.cpu.state().pc() == 0, "run: no command after quit is executed");
    }
    {
        Session s;
        std::istringstream input("step\n\nstep");
        s.debugger.run(input);
        check(s.cpu.state().pc() == 0x08, "run: stops at the end of the input, also without a final newline");
        std::string expected = kLine0 + "(coresim) " + kLine4 + "(coresim) " + "(coresim) " + "3\n" + kLine8 + "(coresim) ";
        check(s.out.str() == expected, describe("run: until end of input", s.out.str(), expected).c_str());
    }
}
