#include "Instruction.h"
using namespace std;

Instruction::Instruction(uint16_t value){
    raw = value;
    opcode = value / 4096;
    r = (value / 256) %16;
    s = (value / 16) %16;
    t = value %16;
    xy = value %256;
}

optional <Instruction> Instruction:: fromHexString(const string& line){
    if(line.size() != 4) {
        return nullopt;
    }
    for(char c : line) {
        bool isDigit = (c >= '0' && c <= '9');
        bool isBig = (c >='A' && c <= 'F');
        bool isSmall = (c >= 'a' && c <= 'f');
        if(!isDigit && !isBig && !isSmall) {
            return nullopt;
        }
    }

    int value = stoi(line , nullptr , 16);
    return Instruction(static_cast<uint16_t>(value));
}

bool Instruction:: isValid() const {
    if(opcode < 1 || opcode > 12) {
        return false;
    }
    if(opcode == 4 && r!=0){
        return false;
    }
    if(opcode == 10 && s!=0){
        return false;
    }
    if(opcode == 12 && (raw % 4096 !=0))
    {
        return false;
    }
    return true;
}

uint8_t Instruction :: getOpcode() const {return opcode; }
uint8_t Instruction :: getR() const {return r; }
uint8_t Instruction :: getS() const {return s; }
uint8_t Instruction :: getT() const {return t; }
uint8_t Instruction :: getXY() const {return xy; }
uint16_t Instruction :: getRaw() const {return raw; }

string Instruction::tostring() const {
    string digits = "0123456789ABCDEF" ;
    string result = "" ;
    result += digits[opcode];
    result += digits[r];
    result += digits[s];
    result += digits[t];
    return result;
}
