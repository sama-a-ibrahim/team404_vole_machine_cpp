#include "RegisterFile.h"

RegisterFile::RegisterFile() {
    registers.fill(0);
}

uint8_t RegisterFile::get(uint8_t index) const {
    return registers[index];
}

void RegisterFile::set(uint8_t index, uint8_t value) {
    registers[index] = value;
}

void RegisterFile::reset() {
    registers.fill(0);
}

const array<uint8_t, 16>& RegisterFile::values() const {
    return registers;
}