#pragma once
#include <cstdint>

namespace coresim {
    enum class Opcode : std::uint8_t {
        // transfer register values
        MOV   = 0x00,               
        MOVI  = 0x40,               

        // calculate            
        ADD   = 0x01,               
        SUB   = 0x02,               
        MUL   = 0x03,               
        DIV   = 0x04,               
        ADDI  = 0x41,               

        // bitoperations           
        AND   = 0x05,               
        OR    = 0x06,               
        XOR   = 0x07,               
        NOT   = 0x08,               

        // compare values
        CMP   = 0x09,               

        // stackoperations
        PUSH  = 0x0A,               
        POP   = 0x0B,               
        RET   = 0x0C,               
        CALL  = 0xC5,               

        // cpu control          
        NOP   = 0x0D,               
        HLT   = 0x0E,               

        // output           
        OUT   = 0x0F,               

        // memory accesses
        LOAD  = 0x80,               
        STORE = 0x81,               

        // jumps            
        JMP   = 0xC0,               
        JZ    = 0xC1,               
        JNZ   = 0xC2,               
        JN    = 0xC3,               
        JC    = 0xC4                
    };          
}           
