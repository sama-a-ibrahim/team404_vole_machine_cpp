#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <cstdint>
#include <optional>
#include <string>
class Instruction {
    public:
    Instruction(uint16_t raw);
    static std::optional <Instruction> fromHexString(const std::string& line);
    bool isValid() const;
    uint8_t getOpcode() const;
    uint8_t getR() const;
    uint8_t getS() const;
    uint8_t getT() const;
    uint8_t getXY() const;
    uint16_t getRaw() const;
    std:: string tostring() const;

    private:
    uint16_t raw;
    uint8_t opcode;
    uint8_t r;
    uint8_t s;
    uint8_t t;
    uint8_t xy;

};
#endif