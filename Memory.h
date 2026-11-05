#pragma once
#include <cstdint>
#include <array>

class Memory {
public:
   Memory();
   uint8_t read(uint8_t address) const;
   void write(uint8_t address, uint8_t value);
   static const uint8_t ScreenAddress = 0xFF;
   void setScreenListner (void (*listener)(uint8_t));
   void reset ();
   const std::array <uint8_t, 256> &cells() const;

private:
   std::array <uint8_t, 256> memory;
   void (*screenListener)(uint8_t)= nullptr;
};