#pragma once
#include <string>
#include <vector>
#include "Instruction.h"

enum class LoadStatus {
    OK,
    INVALID_START_ADDRESS,
    FILE_NOT_FOUND,
    PARSE_ERROR,
    INVALID_INSTRUCTION,
    DOES_NOT_FIT
};

struct LoadOutcome {
    LoadStatus status = LoadStatus::OK;
    std:: string message;
    unsigned line = 0;
    std:: vector <Instruction> instructions;
};