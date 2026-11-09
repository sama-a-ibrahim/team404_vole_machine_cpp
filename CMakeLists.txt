cmake_minimum_required(VERSION 3.19)
project(Volemachine-gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Volemachine-gui
    WIN32
    main_gui.cpp
    MainWindow.cpp
    MainWindow.h
    ALU.cpp ALU.h
    Float8.cpp Float8.h
    CPU.cpp CPU.h
    Instruction.cpp Instruction.h
    LoadOutcome.h
    MachineController.cpp MachineController.h
    MachineState.h
    Memory.cpp Memory.h
    ProgramLoader.cpp ProgramLoader.h
    RegisterFile.cpp RegisterFile.h
    Screen.cpp Screen.h
    StepStatus.h
)

target_link_libraries(Volemachine-gui
    PRIVATE
        Qt::Core
        Qt::Widgets
)