#pragma once
#include <cstdint>
#include <array>
#include <functional>

class Memory {
public:
    Memory();

    uint8_t read(uint8_t address) const;

    void write(uint8_t address, uint8_t value);

    void writeRaw(uint8_t address, uint8_t value);

    static const uint8_t ScreenAddress = 0xFF;

    void setScreenListener(std::function<void(uint8_t)> listener);

    void reset();

    const std::array<uint8_t, 256>& cells() const;

private:
    std::array<uint8_t, 256> memory;

    std::function<void(uint8_t)> screenListener = nullptr;
};