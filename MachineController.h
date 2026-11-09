#ifndef MACHINE_CONTROLLER_H
#define MACHINE_CONTROLLER_H
#include <string>
#include <cstdint>
#include "Screen.h"
#include "Memory.h"
#include "CPU.h"
#include "ProgramLoader.h"
#include "MachineState.h"


class MachineController {
private:
    Screen screen;
    Memory memory;
    CPU cpu;
    ProgramLoader loader;

public:
    MachineController();


    MachineController(const MachineController&) = delete;
    MachineController& operator=(const MachineController&) = delete;

    LoadOutcome loadProgram(const std::string& path, uint16_t start);

    bool overwriteRegister(uint8_t index, uint8_t value);

    void overwriteMemory(uint8_t address, uint8_t value);

    bool setPC(uint16_t address);

    StepStatus step();

    StepStatus run(unsigned maxSteps = 1000000);

    void reset();

    bool isHalted() const;

    MachineState snapshot() const;
};

#endif
