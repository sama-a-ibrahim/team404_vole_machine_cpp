#ifndef REGISTERFILE_H
#define REGISTERFILE_H
#include <array>
#include <cstdint>
using namespace std;

class RegisterFile {
private:
    array<uint8_t, 16> registers;

public:
    RegisterFile();
   uint8_t get(uint8_t index) const;
    void set(uint8_t index, uint8_t value);
    void reset();
    const array<uint8_t, 16>& values() const;
    
};

#endif