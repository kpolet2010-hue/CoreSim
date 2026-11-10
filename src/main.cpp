#include "coresim/Architecture.h"
#include "coresim/Assembler.h"
#include "coresim/CPU.h"
#include "coresim/Loader.h"
#include "coresim/Memory.h"

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
    if (argc >= 2) {
        pathToFile = argv[1];
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
