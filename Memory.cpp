#include "Memory.h"

Memory::Memory() {
   memory.fill(0);
}
uint8_t Memory::read(uint8_t address) const {
   return memory[address];
}
void Memory::write(uint8_t address, uint8_t value) {
   memory[address] = value;
   if (address == ScreenAddress && screenListener != nullptr) {
      screenListener(value);
   }
}
void Memory::setScreenListner(void (*listener)(uint8_t)) {
   screenListener = listener;
}
void Memory::reset() {
   memory.fill(0);
   if (screenListener != nullptr) {
      screenListener(0);
   }
}
const std::array <uint8_t, 256> &Memory::cells() const {
   return memory;
}