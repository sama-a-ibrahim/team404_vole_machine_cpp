#include "Float8.h"
#include <cmath>

Float8::Float8(uint8_t bits) : value(bits) {}

uint8_t Float8::bits() const { return value; }

double Float8::toDouble() const {
    int sign     = value >> 7;
    int exponent = (value >> 4) & 7;
    int mantissa = value & 0xF;
    double result = (mantissa / 16.0) * std::pow(2, exponent - 4);
    return sign ? -result : result;
}

Float8 Float8::fromDouble(double v) {
    if (v == 0) return Float8(0);

    int sign = 0;
    if (v < 0) { sign = 1; v = -v; }

    int e = 0;
    while (v >= 1)  { v /= 2; e++; }
    while (v < 0.5) { v *= 2; e--; }

    int exponent = e + 4;
    if (exponent > 7) return Float8((sign << 7) | 0x7F);  
    if (exponent < 1) return Float8(sign << 7);          

    int mantissa = (int)(v * 16);
    return Float8((sign << 7) | (exponent << 4) | mantissa);
}

Float8 Float8::add(Float8 a, Float8 b) {
    return fromDouble(a.toDouble() + b.toDouble());
}