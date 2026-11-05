#pragma once
#include <cstdint>
#include <vector>

class Screen {
public:
   void write(uint8_t value);
   void clear();
   const std::vector<uint8_t>& gethistory() const;
private:
   std::vector<uint8_t> history;
};