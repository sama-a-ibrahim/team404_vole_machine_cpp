#ifndef STEP_STATUS_H
#define STEP_STATUS_H

enum class StepStatus {
    OK,
    HALTED,
    INVALID_INSTRUCTION,
    PC_OUT_OF_RANGE,
    STEP_LIMIT_REACHED,

};

#endif 