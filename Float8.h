#ifndef FLOAT8_H
#define FLOAT8_H

#include <cstdint>

class Float8 {
public:
    Float8(uint8_t bits = 0);

    static Float8 fromDouble(double v);
    static Float8 add(Float8 a, Float8 b);

    double toDouble() const;
    uint8_t bits() const;

private:
    uint8_t value;
};

#endif