#include "Menu.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <cctype>

static bool parseHex(const std::string& input, unsigned int& value) {

    if (input.empty()) {
        return false;
    }

    std::string hex = input;

    if (hex.size() > 2 && hex[0] == '0' &&
        (hex[1] == 'x' || hex[1] == 'X')) {
        hex = hex.substr(2);
    }

    if (hex.empty()) {
        return false;
    }

    for (char c : hex) {
        if (!std::isxdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    try {
        size_t pos;
        unsigned long result = std::stoul(hex, &pos, 16);

        if (pos != hex.size() || result > 0xFFFF) {
            return false;
        }

        value = static_cast<unsigned int>(result);
        return true;
    }
    catch (...) {
        return false;
    }
}

Menu::Menu(MachineController& controller)
    : controller(controller), screenMode(ScreenMode::DECIMAL) {
}
void Menu::showMainMenu() const {

    std::cout << "\n=== Vole Machine Simulator ===\n";

    std::cout << "1. Load Program from File\n";
    std::cout << "2. Edit Register\n";
    std::cout << "3. Edit Memory\n";
    std::cout << "4. Set Program Counter\n";
    std::cout << "5. Run Program\n";
    std::cout << "6. Run Step by Step\n";
    std::cout << "7. Display Machine State\n";
    std::cout << "8. Change Screen Mode\n";
    std::cout << "9. Reset Machine\n";
    std::cout << "10. Exit\n";

    std::cout << "Enter your choice: ";
}
void Menu::handleLoad() {

    std::string filename;
    std::string startInput;
    unsigned int start;

    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);

    std::cout << "Enter start address (hex): ";
    std::getline(std::cin, startInput);

    if (!parseHex(startInput, start)) {
        std::cout << "Invalid hexadecimal address.\n";
        return;
    }

    if (start >= 0xFF || start % 2 != 0) {
        std::cout << "Invalid start address.\n";
        return;
    }

    LoadOutcome result =
        controller.loadProgram(filename, static_cast<uint16_t>(start));

    if (result.status != LoadStatus::OK) {

        std::cout << "Loading failed: " << result.message << "\n";

        if (result.line != 0) {
            std::cout << "Line: " << result.line << "\n";
        }

        return;
    }

    std::cout << "Program loaded successfully.\n";

    handleDisplay();
}
void Menu::handleEditRegister() {

    std::string indexInput;
    std::string valueInput;

    unsigned int index;
    unsigned int value;

    std::cout << "Enter register index (0-F in hex): ";
    std::getline(std::cin, indexInput);

    if (!parseHex(indexInput, index) || index > 0xF) {
        std::cout << "Invalid register index.\n";
        return;
    }

    std::cout << "Enter value (00-FF in hex): ";
    std::getline(std::cin, valueInput);

    if (!parseHex(valueInput, value) || value > 0xFF) {
        std::cout << "Invalid register value.\n";
        return;
    }

    if (controller.overwriteRegister(
        static_cast<uint8_t>(index),
        static_cast<uint8_t>(value))) {

        std::cout << "Register updated successfully.\n";
    }
    else {
        std::cout << "Failed to update register.\n";
    }

    handleDisplay();
}
void Menu::handleEditMemory() {

    std::string addressInput;
    std::string valueInput;

    unsigned int address;
    unsigned int value;

    std::cout << "Enter memory address (00-FF in hex): ";
    std::getline(std::cin, addressInput);

    if (!parseHex(addressInput, address) || address > 0xFF) {
        std::cout << "Invalid memory address.\n";
        return;
    }

    std::cout << "Enter value (00-FF in hex): ";
    std::getline(std::cin, valueInput);

    if (!parseHex(valueInput, value) || value > 0xFF) {
        std::cout << "Invalid memory value.\n";
        return;
    }

    controller.overwriteMemory(
        static_cast<uint8_t>(address),
        static_cast<uint8_t>(value)
    );

    std::cout << "Memory updated successfully.\n";

    handleDisplay();
}
void Menu::handleSetPC() {

    std::string addressInput;
    unsigned int address;

    std::cout << "Enter PC address (00-FE in hex): ";
    std::getline(std::cin, addressInput);

    if (!parseHex(addressInput, address) || address > 0xFE) {
        std::cout << "Invalid PC address.\n";
        return;
    }

    if (controller.setPC(static_cast<uint16_t>(address))) {
        std::cout << "PC updated successfully.\n";
    }
    else {
        std::cout << "Failed to update PC.\n";
    }

    handleDisplay();
}
void Menu::handleRun() {

    StepStatus status = controller.run();

    if (status == StepStatus::HALTED) {
        std::cout << "Program halted successfully.\n";
    }
    else if (status == StepStatus::STEP_LIMIT_REACHED) {
        std::cout << "Step limit reached.\n";
    }
    else if (status == StepStatus::INVALID_INSTRUCTION) {
        std::cout << "Invalid instruction.\n";
    }
    else if (status == StepStatus::PC_OUT_OF_RANGE) {
        std::cout << "Program counter out of range.\n";
    }

    handleDisplay();

    std::cout << "Returning to main menu. "
              << "You can load another program or exit.\n";
}
void Menu::handleStep() {

    while (true) {

        StepStatus status = controller.step();

        if (status == StepStatus::OK) {

            handleDisplay();

            std::cout << "Press Enter for next step...";

            std::string input;
            std::getline(std::cin, input);
        }
        else if (status == StepStatus::HALTED) {

            std::cout << "Program halted.\n";
            handleDisplay();
            break;
        }
        else if (status == StepStatus::INVALID_INSTRUCTION) {

            std::cout << "Invalid instruction.\n";
            handleDisplay();
            break;
        }
        else if (status == StepStatus::PC_OUT_OF_RANGE) {

            std::cout << "Program counter out of range.\n";
            handleDisplay();
            break;
        }
        else {
            break;
        }
    }

    std::cout << "Returning to main menu.\n";
}
void Menu::handleDisplay() {

    MachineState state = controller.snapshot();

    std::cout << "\n========== MACHINE STATE ==========\n";

    std::cout << "PC: 0x"
              << std::hex << std::uppercase
              << std::setw(2) << std::setfill('0')
              << state.pc << "\n";

    std::cout << "IR: 0x"
              << std::setw(4) << state.ir << "\n";

    std::cout << "Halted: "
              << (state.halted ? "Yes" : "No") << "\n";

    std::cout << std::dec;

    printRegisters(state);
    printMemory(state);
    printScreen(state);

    std::cout << std::setfill(' ');

    std::cout << "===================================\n";
}
void Menu::printRegisters(const MachineState& s) const {

    std::cout << "\n--- Registers ---\n";

    for (int i = 0; i < 16; i++) {

        std::cout << "R"
                  << std::hex << std::uppercase << i
                  << ": 0x"
                  << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(s.registers[i])
                  << "  ";

        if ((i + 1) % 4 == 0) {
            std::cout << "\n";
        }
    }

    std::cout << std::dec;
}
void Menu::printMemory(const MachineState& s) const {

    std::cout << "\n--- Memory ---\n";

    std::cout << "    ";

    for (int i = 0; i < 16; i++) {
        std::cout << std::setw(2) << std::hex
                  << std::uppercase << i << " ";
    }

    std::cout << "\n";

    for (int i = 0; i < 256; i++) {

        if (i % 16 == 0) {
            std::cout << std::hex << std::uppercase
                      << std::setw(2) << std::setfill('0')
                      << i << ": ";
        }

        std::cout << std::setw(2)
                  << static_cast<unsigned int>(s.memory[i])
                  << " ";

        if ((i + 1) % 16 == 0) {
            std::cout << "\n";
        }
    }

    std::cout << std::dec;
}
void Menu::printScreen(const MachineState& s) const {

    std::cout << "\n--- Screen Output ---\n";

    if (s.screen.empty()) {
        std::cout << "Screen is empty.\n";
        return;
    }

    for (uint8_t value : s.screen) {

        if (screenMode == ScreenMode::HEX) {

            std::cout << "0x"
                      << std::hex << std::uppercase
                      << std::setw(2) << std::setfill('0')
                      << static_cast<unsigned int>(value)
                      << " ";
        }
        else {

            std::cout << static_cast<unsigned int>(value) << " ";
        }
    }

    std::cout << std::dec << "\n";
}
void Menu::handleScreenMode() {

    std::string choice;

    std::cout << "\n1. Decimal mode\n";
    std::cout << "2. Hexadecimal mode\n";
    std::cout << "Choose screen mode: ";

    std::getline(std::cin, choice);

    if (choice == "1") {

        screenMode = ScreenMode::DECIMAL;
        std::cout << "Screen mode set to DECIMAL.\n";
    }
    else if (choice == "2") {

        screenMode = ScreenMode::HEX;
        std::cout << "Screen mode set to HEX.\n";
    }
    else {

        std::cout << "Invalid choice.\n";
    }
}
void Menu::run() {

    std::string choice;

    bool running = true;

    while (running) {

        showMainMenu();

        std::getline(std::cin, choice);

        if (!std::cin) {
            break;
        }

        if (choice == "1") {
            handleLoad();
        }
        else if (choice == "2") {
            handleEditRegister();
        }
        else if (choice == "3") {
            handleEditMemory();
        }
        else if (choice == "4") {
            handleSetPC();
        }
        else if (choice == "5") {
            handleRun();
        }
        else if (choice == "6") {
            handleStep();
        }
        else if (choice == "7") {
            handleDisplay();
        }
        else if (choice == "8") {
            handleScreenMode();
        }
        else if (choice == "9") {

            controller.reset();

            std::cout << "Machine reset successfully.\n";
        }
        else if (choice == "10") {

            std::cout << "Exiting...\n";
            running = false;
        }
        else {

            std::cout << "Invalid choice. Try again.\n";
        }
    }
}