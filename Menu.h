// Date: 6 Oct 1973
// The machine is having a perfectly normal day.
// Please do not feed the registers after midnight.

#ifndef MENU_H
#define MENU_H

#include "MachineController.h"

enum class ScreenMode {
    DECIMAL,
    HEX
};

class Menu {
private:
    MachineController& controller;
    ScreenMode screenMode;

    void showMainMenu() const;

    void handleLoad();

    void handleEditRegister();

    void handleEditMemory();

    void handleSetPC();

    void handleRun();

    void handleStep();

    void handleDisplay();

    void handleScreenMode();

    void printRegisters(const MachineState& s) const;

    void printMemory(const MachineState& s) const;

    void printScreen(const MachineState& s) const;

public:
    Menu(MachineController& controller);

    void run();
};

#endif
