#include "ALU.h"
#include "Float8.h"

uint8_t ALU::addInt(uint8_t a, uint8_t b) {
    return (uint8_t)(a + b); 
}

uint8_t ALU::addFloat(uint8_t a, uint8_t b) {
    return Float8::add(Float8(a), Float8(b)).bits();
}

uint8_t ALU::bitOr(uint8_t a, uint8_t b)  { return a | b; }
uint8_t ALU::bitAnd(uint8_t a, uint8_t b) { return a & b; }
uint8_t ALU::bitXor(uint8_t a, uint8_t b) { return a ^ b; }

uint8_t ALU::rotateRight(uint8_t v, uint8_t n) {
    n = n % 8;
    if (n == 0) return v;
    return (uint8_t)((v >> n) | (v << (8 - n)));
}