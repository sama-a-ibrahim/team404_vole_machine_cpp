// Date: 6 Oct 1973
// The machine is having a perfectly normal day.
// Please do not feed the registers after midnight.

#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#include <cstdint>
#include <array>
#include <vector>

struct MachineState {
    uint16_t pc;
    uint16_t ir;
    bool halted;
    std::array<uint8_t, 16> registers;
    std::array<uint8_t, 256> memory;
    std::vector<uint8_t> screen;
};

#endif // MACHINE_STATE_H