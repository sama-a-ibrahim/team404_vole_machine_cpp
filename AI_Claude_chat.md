I am designing an OOP simulator for the Vole Machine. I am attaching our best base class diagram from Deep Seek. Act as the final-stage AI reviewer and improve this design until it is fully compliant with the assignment, clean for a later C++ implementation (separate compilation), ready for a GUI later, and not over-engineered.

Do not start over. Keep this design as the base. Do not add one class per opcode, a dispatcher, or a large handler hierarchy. A switch in CPU::execute that calls a stateless ALU is acceptable if you say so clearly.

Checklist to validate and fix

1.Memory:

.256 cells (bytes), addressed in hex (00 to FF).
.Support loading programs into even memory locations (00, 02, 04, …, < FF).
.Matrix display representation.
.Screen output mapping: location FF is mapped to the screen (writing to FF displays output). Ensure Memory or the machine façade handles this explicitly, including menu overwrite of FF (no bypass).

2.Registers:
.16 general-purpose registers R0 to RF.
.R0 must be clearly highlighted as having a special role used for comparisons in conditional jumps.

3.CPU and control (the machine):

.Contains Program Counter (PC) and Instruction Register (IR).
.Manages the fetch–decode–execute cycle (step-by-step and run-at-once).
.Instruction validation must reject invalid instructions (at load and again at fetch).

4.ALU:

.Must support all required operations cleanly: two's complement addition (5RST), floating-point addition (6RST), bitwise OR (7RST), AND (8RST), XOR (9RST), and right circular rotation (AR0X).

5.Instruction set — methods must account for:

.1RXY, 2RXY, 3RXY, 3RFF (screen), 40RS, 5RST, 6RST, 7RST, 8RST, 9RST, AR0X, BRXY (jump if Reg == R0), C000 (halt).

Weaknesses found in comparison (must fix):
.Memory is coupled to Screen.
.display() on domain classes leaks I/O.
.No opcode polymorphism (keep a simple execute/ALU solution; do not over-engineer this).

Keep these settled decisions unless you have a strictly simpler fix:
.Screen is a history of values written to FF; default display is unsigned decimal (example: 5 + 7 = 12 shows 12).
.read(FF) returns cells[FF], not the screen history.
.Last legal fetch start is FE; PC_OUT_OF_RANGE only when pc == FF.
.ProgramLoader checks even start, start < FF, program fit, and invalid lines. Failed load writes nothing.
.Successful load sets PC to the start address. reset() sets PC to 00 and clears memory, registers, and screen.
.One façade: MachineController. One execution enum: StepStatus. Menu is UI only.
.Ready for GUI later: UI must not own hardware; domain must not print. A small MachineState snapshot is preferred.

If you need one extra detail to make the best design, ask. Otherwise produce the final result.

Required answer order:

(a) Classes (and enums/structs): responsibility, attributes with types, methods with parameters and return types, relationships.

(b) What you changed from Deep Seek and why (especially the three weaknesses).

(c) How the classes work together for load, fetch–decode–execute, FF write, run vs step, display, reset, and invalid instructions.

(d) One complete PlantUML class diagram I can paste into plantuml.com, with multiplicities, no duplicated arrows, compact and readable.

Then a short checklist against the 5 requirement groups above. English. UML, not a full C++ dump.

PlantUML (Deep Seek):

@startuml
skinparam classAttributeIconSize 0
hide empty members

' ==================== Enums / results ====================

enum ScreenMode {
  DECIMAL
  HEX
}

enum StepStatus {
  OK
  HALTED
  INVALID_INSTRUCTION
  PC_OUT_OF_RANGE
  STEP_LIMIT_REACHED
}

enum LoadStatus {
  OK
  INVALID_START_ADDRESS
  FILE_NOT_FOUND
  PARSE_ERROR
  INVALID_INSTRUCTION
  DOES_NOT_FIT
}

class LoadOutcome <<struct>> {
  + status : LoadStatus
  + message : std::string
  + instructions : std::vector<Instruction>
}

' ==================== Storage & output ====================

class Screen {
  - buffer : std::vector<uint8_t>
  - mode : ScreenMode
  + Screen()
  + write(v : uint8_t) : void
  + setMode(m : ScreenMode) : void
  + clear() : void
  + isEmpty() const : bool
  + display(out : std::ostream&) const : void
}

class Memory {
  - cells : std::array<uint8_t, 256>
  - screen : Screen*
  + Memory(screen : Screen&)
  + read(address : uint8_t) const : uint8_t
  + writeRaw(address : uint8_t, value : uint8_t) : void
  + write(address : uint8_t, value : uint8_t) : void
  + reset() : void
  + display(out : std::ostream&) const : void
}

Memory "1" --> "1" Screen : "write(0xFF, v) forwards\nto Screen history"

note bottom of Memory
  Address 0xFF is memory-mapped to the Screen.
  writeRaw(addr, v):  cells[addr] = v;  NO side effects.
                      Used by loadProgram and overwriteMemory
                      (setup / debug writes).
  write(addr, v):     cells[addr] = v;  if addr == 0xFF -> screen->write(v).
                      Used ONLY by CPU STORE (runtime writes).
  read(0xFF):         returns cells[0xFF]  (last value written, any path).
  Screen is write-only from the machine's view.
end note

' ==================== CPU core ====================

class RegisterFile {
  - regs : std::array<uint8_t, 16>
  + get(index : uint8_t) const : uint8_t
  + set(index : uint8_t, value : uint8_t) : void
  + reset() : void
  + display(out : std::ostream&) const : void
}

class Float8 {
  - bits : uint8_t
  + Float8(bits : uint8_t)
  + {static} fromBits(bits : uint8_t) : Float8
  + {static} fromDouble(v : double) : Float8
  + {static} add(a : Float8, b : Float8) : Float8
  + toDouble() const : double
  + bits() const : uint8_t
}

note bottom of Float8
  8-bit float (Brookshear layout):
    bit 7      : sign (0 = +, 1 = -)
    bits 6..4  : exponent, excess-4 (bias 4); real exp = E - 4, range -4..+3
    bits 3..0  : mantissa in normalized form .1xxx
                 (leading 1 stored explicitly; no implicit 1)
    value = ± 0.1mmm(2) × 2^(E-4)
  Zero: 0x00 = +0, 0x80 = -0 (arithmetic treats both as 0).
  add():
    overflow  -> saturate to largest finite magnitude of the sign
                 (0x7F = +max, 0xFF = -max).  No inf/NaN.
    underflow -> flush to signed zero.             No denormals.
    rounding  -> truncate extra mantissa bits.
    zero rules: x + 0 = x; 0 + 0 = +0.
  These rules are a spec gap and are CHOICES — documented, not given.
end note

class ALU <<utility>> {
  + {static} addInt(a : uint8_t, b : uint8_t) : uint8_t
  + {static} addFloat(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitOr(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitAnd(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitXor(a : uint8_t, b : uint8_t) : uint8_t
  + {static} rotateRight(v : uint8_t, n : uint8_t) : uint8_t
}

ALU ..> Float8 : addFloat delegates

class Instruction {
  - raw : uint16_t
  - opcode : uint8_t
  - r : uint8_t
  - s : uint8_t
  - t : uint8_t
  - xy : uint8_t
  + Instruction(raw : uint16_t)
  + {static} fromHexString(line : const std::string&) : std::optional<Instruction>
  + isValid() const : bool
  + getOpcode() const : uint8_t
  + getR() const : uint8_t
  + getS() const : uint8_t
  + getT() const : uint8_t
  + getXY() const : uint8_t
  + getRaw() const : uint16_t
  + toString() const : std::string
}

note right of Instruction
  fromHexString: single text->Instruction converter.
  Strips comment after first token, requires exactly
  4 hex digits, constructs Instruction, runs isValid.
  isValid checks:
    opcode in 1..C
    opcode 4: second nibble == 0  (40RS)
    opcode A: second nibble == 0  (R0X)
    opcode C: operand == 000
  No memory/register-bounds checks here.
end note

class CPU {
  - pc : uint16_t
  - ir : uint16_t
  - halted : bool
  - registers : RegisterFile
  - memory : Memory*
  + CPU(memory : Memory&)
  + setPC(addr : uint16_t) : bool
  + getPC() const : uint16_t
  + getIR() const : uint16_t
  + isHalted() const : bool
  + clearHalt() : void
  + step() : StepStatus
  + reset() : void
  + displayState(out : std::ostream&) const : void
  - fetch() : uint16_t
  - execute(instr : const Instruction&) : StepStatus
}

CPU "1" *-- "1" RegisterFile
CPU "1" --> "1" Memory : "read / write"
CPU ..> Instruction : "creates per fetch"
CPU ..> ALU
CPU ..> StepStatus

note right of CPU
  pc is uint16_t so pc+2 reaches 0x0100
  without wrapping.
  setPC(addr): accepts 0x00..0xFE only; returns false
  and leaves pc unchanged for 0xFF / >=0x100. No clamping.
  step(): if pc > 0xFE -> PC_OUT_OF_RANGE.
  Last valid start is 0xFE (bytes 0xFE/0xFF; 0xFF readable).
  STORE uses memory->write(xy, v); FF side effect in Memory::write.
  Opcode dispatch is a switch in execute() (see Design Notes).
end note

' ==================== Loading ====================

class ProgramLoader {
  + loadFromFile(path : const std::string&) : LoadOutcome
}

ProgramLoader ..> Instruction
ProgramLoader ..> LoadOutcome

note right of ProgramLoader
  Pure parser: iterates lines, skips blanks and
  lines starting with ';', calls Instruction::fromHexString
  per line, records the failing line number in message.
  Returns LoadOutcome with instructions filled and status
  OK / FILE_NOT_FOUND / PARSE_ERROR / INVALID_INSTRUCTION.
  Performs NO placement / fit checks.
end note

' ==================== Façade ====================

class MachineController {
  - memory : Memory
  - screen : Screen
  - cpu : CPU
  - loader : ProgramLoader
  + MachineController()
  + loadProgram(path : const std::string&, start : uint8_t) : LoadOutcome
  + overwriteRegister(index : uint8_t, value : uint8_t) : void
  + overwriteMemory(address : uint8_t, value : uint8_t) : void
  + setPC(address : uint16_t) : bool
  + run(maxSteps : unsigned = 1000000) : StepStatus
  + step() : StepStatus
  + reset() : void
  + displayAll(out : std::ostream&) const : void
  + isHalted() const : bool
}

MachineController "1" *-- "1" Memory
MachineController "1" *-- "1" Screen
MachineController "1" *-- "1" CPU
MachineController "1" *-- "1" ProgramLoader
MachineController ..> StepStatus
MachineController ..> LoadOutcome
MachineController ..> LoadStatus

note bottom of MachineController
  loadProgram order:
    1. start even and < 0xFF           -> INVALID_START_ADDRESS
    2. loader.loadFromFile(path)        -> FILE_NOT_FOUND / PARSE_ERROR /
                                           INVALID_INSTRUCTION
    3. start + 2*N <= 0x100             -> DOES_NOT_FIT
    4. commit bytes via memory.writeRaw (NO screen),
       setPC(start), clearHalt           -> OK
  overwriteMemory: memory.writeRaw  (setup; does NOT log to Screen).
  setPC: forwards to CPU::setPC; returns bool.
  run(): loops cpu.step() until status != OK;
         returns terminal StepStatus (never OK);
         step budget -> STEP_LIMIT_REACHED.
  reset(): memory.reset(); screen.clear(); cpu.reset() -> PC = 0x00.
end note

' ==================== UI ====================

class Menu {
  - controller : MachineController&
  + Menu(controller : MachineController&)
  + run() : void
  - showMainMenu() const : void
  - handleLoad() : void
  - handleEditRegister() : void
  - handleEditMemory() : void
  - handleSetPC() : void
  - handleRun() : void
  - handleStep() : void
  - handleDisplay() : void
}

Menu "1" --> "1" MachineController : uses
@enduml


Updated memory
Updated memory
Vole Machine: final design review
I kept the DeepSeek design as the base and made targeted fixes. The one clarification I need is stated as an assumption: overwriting FF from the menu goes to the screen, as your checklist says. DeepSeek’s version bypassed it.

(a) Classes
Enums and structs

Type	Responsibility
StepStatus (enum)	The only execution result: OK, HALTED, INVALID_INSTRUCTION, PC_OUT_OF_RANGE, STEP_LIMIT_REACHED.
LoadStatus (enum)	OK, INVALID_START_ADDRESS, FILE_NOT_FOUND, PARSE_ERROR, INVALID_INSTRUCTION, DOES_NOT_FIT.
ScreenMode (enum)	DECIMAL, HEX. It is now a UI-only concern, owned by Menu.
LoadOutcome (struct)	status : LoadStatus, message : std::string, line : unsigned (0 if not applicable), instructions : std::vector<Instruction>.
MachineState (struct)	A read-only snapshot for any front end: pc, ir : uint16_t, halted : bool, registers : std::array<uint8_t,16>, memory : std::array<uint8_t,256>, screen : std::vector<uint8_t>.
Screen is a pure output log.

Attribute: history : std::vector<uint8_t>.
Methods: write(v : uint8_t) : void, clear() : void, getHistory() const : const std::vector<uint8_t>&.
It has no mode, no formatting and no streams.
Memory holds 256 byte cells and knows the memory-mapped address, but it does not know Screen.

Attributes:
cells : std::array<uint8_t,256>
screenListener : std::function<void(uint8_t)>
static constexpr SCREEN_ADDRESS = 0xFF
Methods:
setScreenListener(l) : void
read(address : uint8_t) const : uint8_t
write(address : uint8_t, value : uint8_t) : void (mapped write: store the value, and if address is FF, also call the listener)
writeRaw(address : uint8_t, value : uint8_t) : void (store only, used only to commit a loaded program)
reset() : void
cells() const : const std::array<uint8_t,256>&
uint8_t addresses make out-of-range memory addresses impossible by construction.
RegisterFile holds the 16 general-purpose registers R0..RF.

Attribute: regs : std::array<uint8_t,16>.
Constant: static constexpr R0 = 0, documented as the comparison register for BRXY.
Methods: get(index : uint8_t) const : uint8_t, set(index : uint8_t, value : uint8_t) : void, reset() : void, values() const : const std::array<uint8_t,16>&.
Precondition: index < 16. Instruction nibbles always satisfy it, and the controller checks menu input.
Float8 is a pure value type for the 8-bit float.

Attribute: bits : uint8_t.
Methods: fromBits, fromDouble, static add(a, b), toDouble, bits.
The layout and rounding rules from DeepSeek’s note stay.
Addition must work on sign-magnitude values, so opposite signs mean subtracting magnitudes.
ALU is stateless, with static methods only and no instances.

addInt, addFloat, bitOr, bitAnd, bitXor, rotateRight(v, n). All take and return uint8_t.
rotateRight uses n mod 8.
addFloat delegates to Float8.
Instruction is an immutable decoded 16-bit word.

Attributes: raw : uint16_t, opcode, r, s, t, xy : uint8_t. These are nibbles 0, 1, 2, 3 and the low byte.
Methods:
Instruction(raw : uint16_t) accepts any raw value.
static fromHexString(line : const std::string&) : std::optional<Instruction> is syntax only: first token, exactly 4 hex digits, otherwise nullopt.
isValid() const : bool, getOpcode/getR/getS/getT/getXY/getRaw, toString() const : std::string (pure string, no I/O).
isValid rules:
opcode is in 1..C
opcode 4 has second nibble 0
opcode A has second nibble 0
opcode C has operand 000
CPU holds PC, IR and the halted flag, and runs the cycle.

Attributes: pc : uint16_t, ir : uint16_t, halted : bool, registers : RegisterFile, memory : Memory&.
Methods:
CPU(memory : Memory&)
step() : StepStatus
setPC(addr : uint16_t) : bool
getPC, getIR, isHalted, clearHalt
getRegisters() const : const RegisterFile&
setRegister(index : uint8_t, value : uint8_t) : bool (false if index > 15)
reset() : void
private fetch() const : uint16_t
private execute(instr : const Instruction&) : StepStatus
execute is a plain switch on opcode that calls the stateless ALU. There are no opcode classes and no dispatcher.
ProgramLoader is a pure parser.

Methods: loadFromFile(path : const std::string&) : LoadOutcome, loadFromStream(in : std::istream&) : LoadOutcome.
The stream version is the real parser, so you can unit-test it without files.
It skips blank lines and lines starting with ;, calls fromHexString (failure gives PARSE_ERROR), then isValid (failure gives INVALID_INSTRUCTION). Both record the line number.
An empty program is a PARSE_ERROR. It does no placement.
MachineController is the single façade and owns all the hardware.

Members, in this construction order: screen, memory, cpu, loader.
Methods:
loadProgram(path, start : uint16_t) : LoadOutcome (uint16_t so that 0x100 and above can be reported as INVALID_START_ADDRESS)
overwriteRegister(index, value) : bool
overwriteMemory(address, value) : void
setPC(address : uint16_t) : bool
step() : StepStatus
run(maxSteps = 1000000) : StepStatus
reset() : void
isHalted() const : bool
snapshot() const : MachineState
The constructor wires memory.setScreenListener([this](uint8_t v){ screen.write(v); }).
The class is non-copyable, because the CPU holds a reference and the lambda captures this.
Menu is UI only.

Attributes: controller : MachineController&, screenMode : ScreenMode (default DECIMAL).
Methods: run, showMainMenu, handleLoad, handleEditRegister, handleEditMemory, handleSetPC, handleRun, handleStep, handleDisplay, handleScreenMode.
It also has formatting helpers printRegisters, printMemory (16×16 matrix) and printScreen, each taking const MachineState&.
It owns all cin/cout, input parsing and range checks.
Relationships

Menu → MachineController
MachineController ◆→ Memory, Screen, CPU, ProgramLoader
CPU ◆→ RegisterFile
CPU → Memory (reference)
CPU ⇢ Instruction, ALU, StepStatus
ALU ⇢ Float8
ProgramLoader ⇢ LoadOutcome, Instruction
LoadOutcome ◇→ Instruction
MachineController ⇢ MachineState, LoadOutcome
Menu ⇢ MachineState
There is no Memory → Screen link any more.
(b) What changed from DeepSeek, and why
Weakness 1: Memory coupled to Screen.

Memory no longer holds a Screen*. It exposes only a listener hook, and MachineController connects it to the Screen.
FF mapping stays explicit and lives in one place, Memory::write.
Menu overwrite no longer bypasses FF. overwriteMemory now calls Memory::write, the same path as STORE. DeepSeek used writeRaw there, which contradicted your checklist.
Only program loading uses writeRaw. It is not a runtime write, and the last instruction of a program can legally occupy FE and FF.
A std::function hook is the simplest decoupling that needs no new class or interface. The alternative of putting the check in both CPU and controller would duplicate the mapping.
Weakness 2: display() leaking I/O.

All display(std::ostream&) and displayState/displayAll methods are removed from Memory, RegisterFile, Screen, CPU and MachineController. No domain class includes <ostream>.
MachineController::snapshot() returns a plain MachineState. The Menu formats it as a 16×16 matrix, highlights R0, and prints the screen history. A GUI can use the same snapshot.
ScreenMode moved from Screen to Menu, since decimal or hex is presentation. Screen keeps only raw values.
Weakness 3: opcode polymorphism.

I deliberately kept a switch in CPU::execute that calls the stateless ALU. There are 13 opcodes and each case is one or two lines, so handler classes would add files and dependencies with no benefit.
fromHexString is now syntax-only. Before, one nullopt could not tell PARSE_ERROR from INVALID_INSTRUCTION. Now the loader calls isValid() separately, and the CPU calls the same isValid() at fetch, so validation exists at load and again at fetch.
Smaller fixes

CPU now holds Memory& instead of Memory*.
CPU gained getRegisters and setRegister. Before, nothing let the controller reach the registers.
overwriteRegister returns bool.
loadProgram’s start is uint16_t, so an out-of-range start can be detected.
LoadOutcome gained a line number.
Screen::isEmpty was dropped, and Screen gained getHistory.
R0 is a named constant in RegisterFile.
PC range clarification, which is not a change to your decision: PC_OUT_OF_RANGE applies when pc > 0xFE. That covers pc == 0xFF, reachable only through a BRXY jump to FF. It also covers pc == 0x100, reached by falling off the end after the instruction at FE. setPC still rejects anything above FE.
Operand mapping for 40RS and AR0X: Instruction keeps positional nibbles r, s, t. For 40RS the source is getS() and the destination is getT(). For AR0X the register is getR() and the rotate count is getT().
(c) How the classes work together
Load.

Menu::handleLoad calls controller.loadProgram(path, start).
The controller checks that start is even and below FF, otherwise INVALID_START_ADDRESS.
ProgramLoader parses the file and returns FILE_NOT_FOUND, PARSE_ERROR or INVALID_INSTRUCTION.
The controller checks start + 2N <= 0x100, otherwise DOES_NOT_FIT.
Only now does it commit the bytes with writeRaw (two bytes per instruction), call setPC(start) and clearHalt(). A failed load writes nothing.
Fetch–decode–execute in CPU::step.

If halted, return HALTED.
If pc > 0xFE, return PC_OUT_OF_RANGE.
fetch() reads the bytes at pc and pc+1, and the result goes into ir.
Build Instruction(ir). If !isValid(), return INVALID_INSTRUCTION with PC left at the bad instruction.
Set pc += 2.
execute runs the switch and returns OK, or HALTED for C000. A BRXY jump simply overwrites pc.
Opcode	Action
1RXY	R = memory.read(XY)
2RXY	R = XY
3RXY	memory.write(XY, R)
40RS	reg[t] = reg[s]
5RST	reg[r] = ALU::addInt(reg[s], reg[t])
6RST	reg[r] = ALU::addFloat(reg[s], reg[t])
7RST	reg[r] = ALU::bitOr(reg[s], reg[t])
8RST	reg[r] = ALU::bitAnd(reg[s], reg[t])
9RST	reg[r] = ALU::bitXor(reg[s], reg[t])
AR0X	reg[r] = ALU::rotateRight(reg[r], t)
BRXY	if reg[r] == reg[0] then pc = XY
C000	halted = true, return HALTED
FF write. 3RFF has no special case in the CPU. memory.write(0xFF, v) stores the value and fires the listener, which calls Screen::write(v), which appends to the history. The menu’s overwriteMemory(0xFF, v) takes exactly the same path. read(0xFF) returns cells[0xFF], never the history.

Step vs run. step() calls cpu.step() once and returns its status. run(maxSteps) loops cpu.step() while the status is OK, and returns the terminal status (HALTED, INVALID_INSTRUCTION or PC_OUT_OF_RANGE). If it reaches the budget, it returns STEP_LIMIT_REACHED. It never returns OK.

Display. Menu::handleDisplay calls controller.snapshot(), then prints the 16×16 memory matrix, the registers with R0 highlighted, PC and IR, and the screen history in screenMode.

Reset. MachineController::reset calls memory.reset(), screen.clear() and cpu.reset(). The CPU reset clears registers and IR, sets PC to 0x00 and clears the halted flag. Memory::reset does not fire the listener.

Invalid instructions. They are caught in two places: at load (the loader reports the line number) and again at fetch (for values created by memory edits or jumps into data). In both cases the same isValid() is used.

(d) PlantUML
plantuml
@startuml
skinparam classAttributeIconSize 0
hide empty members

enum StepStatus {
  OK
  HALTED
  INVALID_INSTRUCTION
  PC_OUT_OF_RANGE
  STEP_LIMIT_REACHED
}

enum LoadStatus {
  OK
  INVALID_START_ADDRESS
  FILE_NOT_FOUND
  PARSE_ERROR
  INVALID_INSTRUCTION
  DOES_NOT_FIT
}

enum ScreenMode {
  DECIMAL
  HEX
}

class LoadOutcome <<struct>> {
  + status : LoadStatus
  + message : std::string
  + line : unsigned
  + instructions : std::vector<Instruction>
}

class MachineState <<struct>> {
  + pc : uint16_t
  + ir : uint16_t
  + halted : bool
  + registers : std::array<uint8_t, 16>
  + memory : std::array<uint8_t, 256>
  + screen : std::vector<uint8_t>
}

class Screen {
  - history : std::vector<uint8_t>
  + write(v : uint8_t) : void
  + clear() : void
  + getHistory() const : const std::vector<uint8_t>&
}

class Memory {
  - cells : std::array<uint8_t, 256>
  - screenListener : std::function<void(uint8_t)>
  + {static} SCREEN_ADDRESS : uint8_t = 0xFF
  + setScreenListener(l : std::function<void(uint8_t)>) : void
  + read(address : uint8_t) const : uint8_t
  + write(address : uint8_t, value : uint8_t) : void
  + writeRaw(address : uint8_t, value : uint8_t) : void
  + reset() : void
  + cells() const : const std::array<uint8_t, 256>&
}

note bottom of Memory
  256 cells, addresses 00..FF.
  write(): store; if address == FF also call listener.
           Used by CPU STORE and menu overwrite (no bypass).
  writeRaw(): store only. Used ONLY to commit a loaded program.
  read(FF) returns cells[FF], not the screen history.
  Memory does not know Screen; MachineController wires the listener.
end note

class RegisterFile {
  - regs : std::array<uint8_t, 16>
  + {static} R0 : uint8_t = 0
  + get(index : uint8_t) const : uint8_t
  + set(index : uint8_t, value : uint8_t) : void
  + reset() : void
  + values() const : const std::array<uint8_t, 16>&
}

note bottom of RegisterFile
  16 general-purpose registers R0..RF.
  R0 is SPECIAL: BRXY jumps if register R == R0.
end note

class Float8 {
  - bits : uint8_t
  + Float8(bits : uint8_t)
  + {static} fromBits(bits : uint8_t) : Float8
  + {static} fromDouble(v : double) : Float8
  + {static} add(a : Float8, b : Float8) : Float8
  + toDouble() const : double
  + bits() const : uint8_t
}

note bottom of Float8
  bit 7 sign, bits 6..4 exponent (excess-4), bits 3..0 mantissa .1xxx
  add(): overflow -> saturate (0x7F / 0xFF), underflow -> signed zero,
  truncate extra bits. These are documented CHOICES (spec gap).
end note

class ALU <<utility, stateless>> {
  + {static} addInt(a : uint8_t, b : uint8_t) : uint8_t
  + {static} addFloat(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitOr(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitAnd(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitXor(a : uint8_t, b : uint8_t) : uint8_t
  + {static} rotateRight(v : uint8_t, n : uint8_t) : uint8_t
}

class Instruction {
  - raw : uint16_t
  - opcode : uint8_t
  - r : uint8_t
  - s : uint8_t
  - t : uint8_t
  - xy : uint8_t
  + Instruction(raw : uint16_t)
  + {static} fromHexString(line : const std::string&) : std::optional<Instruction>
  + isValid() const : bool
  + getOpcode() const : uint8_t
  + getR() const : uint8_t
  + getS() const : uint8_t
  + getT() const : uint8_t
  + getXY() const : uint8_t
  + getRaw() const : uint16_t
  + toString() const : std::string
}

note right of Instruction
  fromHexString: syntax only (4 hex digits), no validity check.
  isValid: opcode 1..C; 4 -> nibble1 == 0; A -> nibble1 == 0;
  C -> operand == 000. Used at load AND at fetch.
  Nibbles: r=n1, s=n2, t=n3. 40RS: source=s, dest=t. AR0X: reg=r, count=t.
end note

class CPU {
  - pc : uint16_t
  - ir : uint16_t
  - halted : bool
  - registers : RegisterFile
  - memory : Memory&
  + CPU(memory : Memory&)
  + step() : StepStatus
  + setPC(addr : uint16_t) : bool
  + getPC() const : uint16_t
  + getIR() const : uint16_t
  + isHalted() const : bool
  + clearHalt() : void
  + getRegisters() const : const RegisterFile&
  + setRegister(index : uint8_t, value : uint8_t) : bool
  + reset() : void
  - fetch() const : uint16_t
  - execute(instr : const Instruction&) : StepStatus
}

note right of CPU
  step(): halted -> HALTED; pc > 0xFE -> PC_OUT_OF_RANGE;
  fetch -> ir; invalid -> INVALID_INSTRUCTION (pc unchanged);
  pc += 2; execute.
  execute() is ONE switch on opcode calling the stateless ALU.
  No opcode classes, no dispatcher.
  setPC accepts 00..FE only (returns false otherwise).
end note

class ProgramLoader {
  + loadFromFile(path : const std::string&) : LoadOutcome
  + loadFromStream(in : std::istream&) : LoadOutcome
}

note right of ProgramLoader
  Pure parser. Skips blank and ';' lines, parses with
  fromHexString (PARSE_ERROR), checks isValid (INVALID_INSTRUCTION).
  Records line number. No placement or fit checks.
end note

class MachineController {
  - screen : Screen
  - memory : Memory
  - cpu : CPU
  - loader : ProgramLoader
  + MachineController()
  + loadProgram(path : const std::string&, start : uint16_t) : LoadOutcome
  + overwriteRegister(index : uint8_t, value : uint8_t) : bool
  + overwriteMemory(address : uint8_t, value : uint8_t) : void
  + setPC(address : uint16_t) : bool
  + step() : StepStatus
  + run(maxSteps : unsigned = 1000000) : StepStatus
  + reset() : void
  + isHalted() const : bool
  + snapshot() const : MachineState
}

note bottom of MachineController
  Ctor wires memory.setScreenListener(-> screen.write). Non-copyable.
  loadProgram: 1 start even and < 0xFF (INVALID_START_ADDRESS)
    2 loader parse  3 start + 2N <= 0x100 (DOES_NOT_FIT)
    4 commit via writeRaw, setPC(start), clearHalt. Failure writes nothing.
  overwriteMemory: memory.write (FF reaches the screen).
  run(): loop cpu.step() while OK; returns terminal status,
         STEP_LIMIT_REACHED if the budget runs out.
  reset(): memory, screen, cpu cleared; PC = 0x00.
end note

class Menu {
  - controller : MachineController&
  - screenMode : ScreenMode
  + Menu(controller : MachineController&)
  + run() : void
  - showMainMenu() const : void
  - handleLoad() : void
  - handleEditRegister() : void
  - handleEditMemory() : void
  - handleSetPC() : void
  - handleRun() : void
  - handleStep() : void
  - handleDisplay() : void
  - handleScreenMode() : void
  - printRegisters(s : const MachineState&) const : void
  - printMemory(s : const MachineState&) const : void
  - printScreen(s : const MachineState&) const : void
}

Menu "1" --> "1" MachineController : uses
Menu ..> MachineState
MachineController "1" *-- "1" Screen
MachineController "1" *-- "1" Memory
MachineController "1" *-- "1" CPU
MachineController "1" *-- "1" ProgramLoader
MachineController ..> MachineState : snapshot
MachineController ..> LoadOutcome
CPU "1" *-- "1" RegisterFile
CPU "1" --> "1" Memory : read / write
CPU ..> Instruction : decodes
CPU ..> ALU
CPU ..> StepStatus
ALU ..> Float8 : addFloat
ProgramLoader ..> LoadOutcome
ProgramLoader ..> Instruction
LoadOutcome "1" o-- "0..*" Instruction
@enduml
Checklist
1. Memory: met.

It has 256 cells, addressed 00..FF.
Even placement is enforced by the controller (even start, start < FF, fit).
The matrix comes from MachineState.memory, printed 16×16 by the Menu.
FF mapping is explicit in Memory::write and covers both CPU STORE and menu overwrite. Memory no longer knows Screen.
2. Registers: met.

There are 16 registers R0..RF.
RegisterFile::R0 is a named constant documented as the BRXY comparison register, and the Menu highlights it.
3. CPU and control: met.

The CPU holds PC and IR.
step() and run() implement the fetch–decode–execute cycle, step by step or all at once.
Invalid instructions are rejected at load (INVALID_INSTRUCTION with line number) and again at fetch.
4. ALU: met.

It covers two’s complement add, float add via Float8, OR, AND, XOR and right rotation.
The ALU is stateless and called from the one switch in execute.
5. Instruction set: met.

All of 1RXY, 2RXY, 3RXY, 3RFF, 40RS, 5RST, 6RST, 7RST, 8RST, 9RST, AR0X, BRXY and C000 are covered by the execute switch, with 3RFF handled by the FF mapping.
Two points the assignment spec has to confirm. Float8 overflow, underflow and rounding are documented choices, as in the original note. Jump targets to odd addresses are allowed, with no alignment check at runtime.

I accept your last design as the frozen base. Do not start over and do not add classes.

I only need you to lock two remaining points, then give ONE final PlantUML (the same structure as your last diagram, with those two points reflected in notes if needed).

Load checks
Keep your split: ProgramLoader is a pure parser (file/syntax/isValid, with line numbers).
MachineController does placement: start even and < 0xFF, then start + 2N <= 0x100.
Failed load writes nothing. Successful load commits with writeRaw, setPC(start), clearHalt.

PC bound
Last legal fetch start is 0xFE (bytes FE/FF).
CPU::step returns PC_OUT_OF_RANGE when pc > 0xFE (covers jump to 0xFF and fall-off to 0x100).
setPC accepts 00..FE only.

Do not change anything else:
- Memory has no Screen*; MachineController wires the FF listener.
- overwriteMemory uses Memory::write (FF goes to screen history).
- writeRaw is load-only.
- read(FF) returns cells[FF].
- Screen is a history of uint8_t; DECIMAL/HEX live in Menu only.
- One façade: MachineController. One execution enum: StepStatus (keep STEP_LIMIT_REACHED).
- Domain has no display(ostream&); UI uses MachineState snapshot.
- CPU::execute stays one switch calling a stateless ALU. No opcode classes.

Reply with only:
(a) two-sentence confirmation of those two locks
(b) the complete PlantUML, no duplicated arrows, with multiplicities
No extra classes, no C++ dump, no new weaknesses list.

(a) Both locks match the frozen design. ProgramLoader stays a pure parser (file, syntax, isValid, line numbers), and MachineController::loadProgram alone does placement (start even and < 0xFF, then start + 2N <= 0x100); a failed load writes nothing, and success commits via writeRaw, then setPC(start) and clearHalt(). The last legal fetch start is 0xFE, so CPU::step returns PC_OUT_OF_RANGE when pc > 0xFE (jump to 0xFF, fall-off to 0x100), and setPC accepts 00..FE only.

(b)

plantuml
@startuml
skinparam classAttributeIconSize 0
hide empty members

enum StepStatus {
  OK
  HALTED
  INVALID_INSTRUCTION
  PC_OUT_OF_RANGE
  STEP_LIMIT_REACHED
}

enum LoadStatus {
  OK
  INVALID_START_ADDRESS
  FILE_NOT_FOUND
  PARSE_ERROR
  INVALID_INSTRUCTION
  DOES_NOT_FIT
}

enum ScreenMode {
  DECIMAL
  HEX
}

class LoadOutcome <<struct>> {
  + status : LoadStatus
  + message : std::string
  + line : unsigned
  + instructions : std::vector<Instruction>
}

class MachineState <<struct>> {
  + pc : uint16_t
  + ir : uint16_t
  + halted : bool
  + registers : std::array<uint8_t, 16>
  + memory : std::array<uint8_t, 256>
  + screen : std::vector<uint8_t>
}

class Screen {
  - history : std::vector<uint8_t>
  + write(v : uint8_t) : void
  + clear() : void
  + getHistory() const : const std::vector<uint8_t>&
}

class Memory {
  - cells : std::array<uint8_t, 256>
  - screenListener : std::function<void(uint8_t)>
  + {static} SCREEN_ADDRESS : uint8_t = 0xFF
  + setScreenListener(l : std::function<void(uint8_t)>) : void
  + read(address : uint8_t) const : uint8_t
  + write(address : uint8_t, value : uint8_t) : void
  + writeRaw(address : uint8_t, value : uint8_t) : void
  + reset() : void
  + cells() const : const std::array<uint8_t, 256>&
}

note bottom of Memory
  256 cells, addresses 00..FF. Memory does not know Screen.
  write(): store; if address == FF also call listener.
           Used by CPU STORE and by menu overwrite (no bypass).
  writeRaw(): store only. LOAD-ONLY (commit of a loaded program).
  read(FF) returns cells[FF], not the screen history.
  MachineController wires the listener to Screen::write.
end note

class RegisterFile {
  - regs : std::array<uint8_t, 16>
  + {static} R0 : uint8_t = 0
  + get(index : uint8_t) const : uint8_t
  + set(index : uint8_t, value : uint8_t) : void
  + reset() : void
  + values() const : const std::array<uint8_t, 16>&
}

note bottom of RegisterFile
  16 general-purpose registers R0..RF.
  R0 is SPECIAL: BRXY jumps if register R == R0.
end note

class Float8 {
  - bits : uint8_t
  + Float8(bits : uint8_t)
  + {static} fromBits(bits : uint8_t) : Float8
  + {static} fromDouble(v : double) : Float8
  + {static} add(a : Float8, b : Float8) : Float8
  + toDouble() const : double
  + bits() const : uint8_t
}

note bottom of Float8
  bit 7 sign, bits 6..4 exponent (excess-4), bits 3..0 mantissa .1xxx
  add(): overflow -> saturate (0x7F / 0xFF), underflow -> signed zero,
  truncate extra bits. Documented CHOICES (spec gap).
end note

class ALU <<utility, stateless>> {
  + {static} addInt(a : uint8_t, b : uint8_t) : uint8_t
  + {static} addFloat(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitOr(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitAnd(a : uint8_t, b : uint8_t) : uint8_t
  + {static} bitXor(a : uint8_t, b : uint8_t) : uint8_t
  + {static} rotateRight(v : uint8_t, n : uint8_t) : uint8_t
}

class Instruction {
  - raw : uint16_t
  - opcode : uint8_t
  - r : uint8_t
  - s : uint8_t
  - t : uint8_t
  - xy : uint8_t
  + Instruction(raw : uint16_t)
  + {static} fromHexString(line : const std::string&) : std::optional<Instruction>
  + isValid() const : bool
  + getOpcode() const : uint8_t
  + getR() const : uint8_t
  + getS() const : uint8_t
  + getT() const : uint8_t
  + getXY() const : uint8_t
  + getRaw() const : uint16_t
  + toString() const : std::string
}

note right of Instruction
  fromHexString: syntax only (4 hex digits), no validity check.
  isValid: opcode 1..C; 4 -> nibble1 == 0; A -> nibble1 == 0;
  C -> operand == 000. Used at load AND at fetch.
  Nibbles: r=n1, s=n2, t=n3. 40RS: source=s, dest=t. AR0X: reg=r, count=t.
end note

class CPU {
  - pc : uint16_t
  - ir : uint16_t
  - halted : bool
  - registers : RegisterFile
  - memory : Memory&
  + CPU(memory : Memory&)
  + step() : StepStatus
  + setPC(addr : uint16_t) : bool
  + getPC() const : uint16_t
  + getIR() const : uint16_t
  + isHalted() const : bool
  + clearHalt() : void
  + getRegisters() const : const RegisterFile&
  + setRegister(index : uint8_t, value : uint8_t) : bool
  + reset() : void
  - fetch() const : uint16_t
  - execute(instr : const Instruction&) : StepStatus
}

note right of CPU
  PC bound: last legal fetch start is 0xFE (bytes FE/FF).
  step(): halted -> HALTED; pc > 0xFE -> PC_OUT_OF_RANGE
  (covers jump to 0xFF and fall-off to 0x100);
  fetch -> ir; invalid -> INVALID_INSTRUCTION (pc unchanged);
  pc += 2; execute.
  setPC accepts 00..FE only (returns false otherwise, pc unchanged).
  execute() is ONE switch on opcode calling the stateless ALU.
  No opcode classes, no dispatcher.
end note

class ProgramLoader {
  + loadFromFile(path : const std::string&) : LoadOutcome
  + loadFromStream(in : std::istream&) : LoadOutcome
}

note right of ProgramLoader
  Pure parser: file, syntax, isValid, line numbers.
  Skips blank and ';' lines. fromHexString failure -> PARSE_ERROR;
  isValid failure -> INVALID_INSTRUCTION.
  NO placement or fit checks.
end note

class MachineController {
  - screen : Screen
  - memory : Memory
  - cpu : CPU
  - loader : ProgramLoader
  + MachineController()
  + loadProgram(path : const std::string&, start : uint16_t) : LoadOutcome
  + overwriteRegister(index : uint8_t, value : uint8_t) : bool
  + overwriteMemory(address : uint8_t, value : uint8_t) : void
  + setPC(address : uint16_t) : bool
  + step() : StepStatus
  + run(maxSteps : unsigned = 1000000) : StepStatus
  + reset() : void
  + isHalted() const : bool
  + snapshot() const : MachineState
}

note bottom of MachineController
  Ctor wires memory.setScreenListener(-> screen.write). Non-copyable.
  loadProgram (placement is done HERE, not in the loader):
    1. start even and < 0xFF          -> INVALID_START_ADDRESS
    2. loader parse                   -> FILE_NOT_FOUND / PARSE_ERROR /
                                         INVALID_INSTRUCTION
    3. start + 2*N <= 0x100           -> DOES_NOT_FIT
    4. commit via memory.writeRaw, then setPC(start), clearHalt -> OK
  Any failure returns early: nothing is written.
  overwriteMemory: memory.write (FF goes to screen history).
  run(): loop cpu.step() while OK; returns terminal StepStatus (never OK);
         STEP_LIMIT_REACHED if the budget runs out.
  reset(): memory, registers, screen cleared; PC = 0x00.
end note

class Menu {
  - controller : MachineController&
  - screenMode : ScreenMode
  + Menu(controller : MachineController&)
  + run() : void
  - showMainMenu() const : void
  - handleLoad() : void
  - handleEditRegister() : void
  - handleEditMemory() : void
  - handleSetPC() : void
  - handleRun() : void
  - handleStep() : void
  - handleDisplay() : void
  - handleScreenMode() : void
  - printRegisters(s : const MachineState&) const : void
  - printMemory(s : const MachineState&) const : void
  - printScreen(s : const MachineState&) const : void
}

Menu "1" --> "1" MachineController : uses
Menu ..> MachineState
MachineController "1" *-- "1" Screen
MachineController "1" *-- "1" Memory
MachineController "1" *-- "1" CPU
MachineController "1" *-- "1" ProgramLoader
MachineController ..> MachineState : snapshot
MachineController ..> LoadOutcome
CPU "1" *-- "1" RegisterFile
CPU "1" --> "1" Memory : read / write
CPU ..> Instruction : decodes
CPU ..> ALU
CPU ..> StepStatus
ALU ..> Float8 : addFloat
ProgramLoader ..> LoadOutcome
ProgramLoader ..> Instruction
LoadOutcome "1" o-- "0..*" Instruction
@enduml
