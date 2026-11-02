#include "coresim/Architecture.h"
#include "coresim/Decoder.h"
#include <iostream>

int main() {
    coresim::Decoder decoder;

    uint32_t instruction = 0xFFFFFFFF;

    coresim::DecodedInstruction result = decoder.decode(instruction);

    std::cout << "Opcode " << static_cast<int>(result.opcode) << '\n';
    std::cout << "RD " << static_cast<int>(result.rd) << '\n';
    std::cout << "RS1 " << static_cast<int>(result.rs1) << '\n';
    std::cout << "RS2 " << static_cast<int>(result.rs2) << '\n';
    
    std::cout << "CoreSim v" << coresim::kVersion << '\n';
    return 0;
}
