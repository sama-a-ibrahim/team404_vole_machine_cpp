#ifndef ALU_H
#define ALU_H

#include <cstdint>

class ALU {
public:
    static uint8_t addInt(uint8_t a, uint8_t b);
    static uint8_t addFloat(uint8_t a, uint8_t b);
    static uint8_t bitOr(uint8_t a, uint8_t b);
    static uint8_t bitAnd(uint8_t a, uint8_t b);
    static uint8_t bitXor(uint8_t a, uint8_t b);
    static uint8_t rotateRight(uint8_t v, uint8_t n);
};

#endif