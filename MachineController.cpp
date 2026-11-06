// Date: 6 Oct 1973
// The machine is having a perfectly normal day.
// Please do not feed the registers after midnight.

#include "MachineController.h"

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

MachineController::MachineController()
    : screen(), memory(), cpu(memory), loader() {

    memory.setScreenListener(
        [this](uint8_t value) {
            screen.write(value);
        }
    );
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

LoadOutcome MachineController::loadProgram(
    const std::string& path, uint16_t start) {

    if (start % 2 != 0 || start >= 0xFF) {
        return {
            LoadStatus::INVALID_START_ADDRESS,
            "Invalid start address",
            0,
            {}
        };
    }

    LoadOutcome result = loader.LoadFromFile(path);

    if (result.status != LoadStatus::OK) {
        return result;
    }

    if (start + 2 * result.instructions.size() > 0x100) {
        return {
            LoadStatus::DOES_NOT_FIT,
            "Program does not fit in memory",
            0,
            {}
        };
    }

    for (size_t i = 0; i < result.instructions.size(); i++) {
        uint16_t instruction = result.instructions[i].getRaw();

        uint8_t address = static_cast<uint8_t>(start + 2 * i);

        memory.writeRaw(address, instruction >> 8);
        memory.writeRaw(address + 1, instruction & 0xFF);
    }

    cpu.setPC(start);
    cpu.clearHalt();

    return result;
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

bool MachineController::overwriteRegister(
    uint8_t index, uint8_t value) {

    return cpu.setRegister(index, value);
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MachineController::overwriteMemory(
    uint8_t address, uint8_t value) {

    memory.write(address, value);
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

bool MachineController::setPC(uint16_t address) {
    return cpu.setPC(address);
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

StepStatus MachineController::step() {
    return cpu.step();
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

StepStatus MachineController::run(unsigned maxSteps) {

    for (unsigned i = 0; i < maxSteps; i++) {

        StepStatus status = step();

        if (status != StepStatus::OK) {
            return status;
        }
    }

    return StepStatus::STEP_LIMIT_REACHED;
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MachineController::reset() {

    memory.reset();
    cpu.reset();
    screen.clear();
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

bool MachineController::isHalted() const {
    return cpu.isHalted();
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

MachineState MachineController::snapshot() const {

    MachineState state;

    state.pc = cpu.getPC();
    state.ir = cpu.getIR();
    state.halted = cpu.isHalted();
    state.registers = cpu.getRegisters().values();
    state.memory = memory.cells();
    state.screen = screen.gethistory();

    return state;
}
