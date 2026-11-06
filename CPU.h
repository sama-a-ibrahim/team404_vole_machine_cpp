#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include "RegisterFile.h"
#include "Memory.h"
#include "Instruction.h"
#include "StepStatus.h"

class CPU {
private:
    uint16_t pc;
    uint16_t ir;
    bool halted;
    RegisterFile registers;
    Memory& memory;

    uint16_t fetch() const;
    StepStatus execute(const Instruction& instr);

public:
    CPU(Memory& mem);

    StepStatus step();
    bool setPC(uint16_t addr);
    uint16_t getPC() const;
    uint16_t getIR() const;
    bool isHalted() const;
    void clearHalt();
    const RegisterFile& getRegisters() const;
    bool setRegister(uint8_t index, uint8_t value);
    void reset();
};

#endif