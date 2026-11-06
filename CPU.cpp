#include "CPU.h"
#include "ALU.h"

CPU::CPU(Memory& mem) : pc(0), ir(0), halted(false), memory(mem) {
}

uint16_t CPU::fetch() const {
    uint8_t high = memory.read(static_cast<uint8_t>(pc));
    uint8_t low  = memory.read(static_cast<uint8_t>(pc + 1));
    return (high << 8) | low;
}

StepStatus CPU::step() {
    if (halted) {
        return StepStatus::HALTED;
    }
    if (pc > 0xFE) {
        return StepStatus::PC_OUT_OF_RANGE;
    }

    ir = fetch();

    Instruction instr(ir);
    if (!instr.isValid()) {
        return StepStatus::INVALID_INSTRUCTION;
    }

    pc += 2;
    return execute(instr);
}

StepStatus CPU::execute(const Instruction& instr) {
    uint8_t r  = instr.getR();
    uint8_t s  = instr.getS();
    uint8_t t  = instr.getT();
    uint8_t xy = instr.getXY();

    switch (instr.getOpcode()) {
        case 0x1:
            registers.set(r, memory.read(xy));
            break;

        case 0x2:
            registers.set(r, xy);
            break;

        case 0x3:
            memory.write(xy, registers.get(r));
            break;

        case 0x4:
            registers.set(t, registers.get(s));
            break;

        case 0x5:
            registers.set(r, ALU::addInt(registers.get(s), registers.get(t)));
            break;

        case 0x6:
            registers.set(r, ALU::addFloat(registers.get(s), registers.get(t)));
            break;

        case 0x7:
            registers.set(r, ALU::bitOr(registers.get(s), registers.get(t)));
            break;

        case 0x8:
            registers.set(r, ALU::bitAnd(registers.get(s), registers.get(t)));
            break;

        case 0x9:
            registers.set(r, ALU::bitXor(registers.get(s), registers.get(t)));
            break;

        case 0xA:
            registers.set(r, ALU::rotateRight(registers.get(r), t));
            break;

        case 0xB:
            if (registers.get(r) == registers.get(0)) {
                pc = xy;
            }
            break;

        case 0xC:
            halted = true;
            return StepStatus::HALTED;

        default:
            return StepStatus::INVALID_INSTRUCTION;
    }
    return StepStatus::OK;
}

bool CPU::setPC(uint16_t addr) {
    if (addr > 0xFE || addr % 2 != 0) {
        return false;
    }
    pc = addr;
    return true;
}

uint16_t CPU::getPC() const {
    return pc;
}

uint16_t CPU::getIR() const {
    return ir;
}

bool CPU::isHalted() const {
    return halted;
}

void CPU::clearHalt() {
    halted = false;
}

const RegisterFile& CPU::getRegisters() const {
    return registers;
}

bool CPU::setRegister(uint8_t index, uint8_t value) {
    if (index > 15) {
        return false;
    }
    registers.set(index, value);
    return true;
}

void CPU::reset() {
    registers.reset();
    pc = 0;
    ir = 0;
    halted = false;
}