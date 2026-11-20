#include "coresim/Architecture.h"
#include "coresim/Assembler.h"
#include "coresim/CPU.h"
#include "coresim/Loader.h"
#include "coresim/Memory.h"
#include "coresim/Debugger.h"

#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

namespace {
    constexpr std::size_t kMaxSteps = 1'000'000;

    // .asm files are assembled, every other file is loaded as hex text
    bool isAssembly(const std::string& path) {
        const std::string extension = ".asm";
        return path.size() >= extension.size()
            && path.compare(path.size() - extension.size(), extension.size(), extension) == 0;
    }
} // namespace

int main(int argc, char* argv[]) {
    std::cout << "CoreSim v" << coresim::kVersion << '\n';
    std::string pathToFile;
    bool debug = false;
    int fileIndex = 1;
    if (argc >= 2 && std::string(argv[1]) == "--debug") {
        debug = true;
        fileIndex = 2;
    }
    if (argc > fileIndex) {
        pathToFile = argv[fileIndex];
    } else {
        std::cout << "Program file (.hex or .asm): ";
        std::getline(std::cin, pathToFile);
    }


    std::ifstream file(pathToFile);
    if (!file.is_open()) {
        std::cerr << "error: could not open " << pathToFile << '\n';
        return 1;
    }

    try {
        coresim::Memory memory;
        coresim::CPU cpu(memory);

        if (isAssembly(pathToFile)) {
            coresim::loadAsm(file, memory);
        } else {
            coresim::loadHex(file, memory);
        }

        if (debug) {
            coresim::Debugger debugger(cpu, std::cout);
            debugger.run(std::cin);
            return 0;
        }

        if (!cpu.run(kMaxSteps)) {
            std::cerr << "error: no HLT after " << kMaxSteps << " steps\n";
            return 1;
        }
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
