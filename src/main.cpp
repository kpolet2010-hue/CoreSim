#include "coresim/Architecture.h"
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
} // namespace

int main(int argc, char* argv[]) {
    std::cout << "CoreSim v" << coresim::kVersion << '\n';
    std::string pathToFile;
    if (argc >= 2) {
        pathToFile = argv[1];
    } else {
        std::cout << "Hex file: ";
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

        coresim::loadHex(file, memory);
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
