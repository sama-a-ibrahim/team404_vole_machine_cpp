#include "Screen.h"

void Screen::write(uint8_t value) {
   history.push_back(value);
}
void Screen::clear() {
   history.clear();
}
const std::vector<uint8_t>& Screen::gethistory() const {
   return history;
}