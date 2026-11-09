Shared Conversation

I am working on an object-oriented design for a Vole Machine Simulator, to be implemented later in C++. Below are the machine description, the required task, and my suggested initial design. Please act as a senior OOP software designer.

1. Machine Description
The Vole Machine is a simple computer that executes instructions written in hexadecimal machine language. Each instruction consists of four hexadecimal digits (16 bits).

The machine has 256 memory locations, addressed from 00 to FF, and 16 general-purpose registers, named R0 to RF. It also has a Program Counter (PC), which stores the address of the next instruction to be executed, and an Instruction Register (IR), which holds the current instruction.

Supported instructions (op-code, operand, description):

1 RXY: LOAD register R with the bit pattern found in memory cell XY.
2 RXY: LOAD register R with the value XY.
3 RXY: STORE the content of register R in memory cell XY.
3 R00 variant for screen: when XY = FF, the store writes to the screen (FF is a memory mapping for the screen).
4 0RS: MOVE the content of register R to register S.
5 RST: ADD the contents of registers S and T as two's complement integers and put the result in register R.
6 RST: ADD the contents of registers S and T as floating-point values and put the result in register R.
7 RST: OR registers S and T, result in register R.
8 RST: AND registers S and T, result in register R.
9 RST: XOR registers S and T, result in register R.
A R0X: ROTATE the content of register R one bit to the right X times (circular).
B RXY: JUMP to address XY if register R equals register 0; otherwise continue normally (implemented by copying XY into the PC).
C 000: HALT execution.
The simulator must support:

A menu of choices.
Loading a new program from a text file into a chosen memory location, which must be an even address (00, 02, 04, ..., F0, F2, ...) and less than FF. In the file, lines starting with ";" and empty lines are ignored; each instruction is on its own line as four hex digits, and the rest of the line is ignored.
Overwriting the content of a given register or memory cell with a new value.
Setting the PC to the start address of the program.
Displaying the memory as a matrix, the IR, the PC, and the 16 registers.
Fetching instructions step by step into the IR, validating each one, decoding it, executing it, and updating the PC.
Letting the user run the program either all at once or step by step.
If run at once, displaying the memory, PC, IR, 16 registers and the screen output after the program finishes.
If step by step, displaying them after each step, then waiting for the user to press Enter to run the next step.
Rejecting programs that contain invalid instructions.
Asking the user at the end whether to exit or load another program.
2. Required Task
Model the Vole Machine domain by identifying the classes needed to operate the simulator, the responsibility of each class, the suggested attributes and methods of each class, and the relationships between the classes. The final result must be a UML class diagram written in PlantUML.

3. My Suggested Initial Design (PlantUML)
@startuml class Memory {

data
size
read()
write()
display() }
class Registers {

registers: array[16]
getRegister()
setregister()
reset() }
class CPU {

programCounter: int
instructionRegister: string
fetchInstruction()
decodeInstruction()
executeInstruction()
updatePC()
displayState() }
class Instruction {

rawCode
opcode: string
operand1: char
operand2: char
operand3: char
parse()
validate()
getOpcode() }
class VoleMachine {

memory
cpu: CPU
screen
loadProgram()
run()
step()
reset() }
class Screen {

displayValue
display() }
VoleMachine *-- Memory VoleMachine *-- CPU VoleMachine *-- Screen CPU --> Memory CPU --> Instruction CPU *-- Registers @enduml

4. Your Task
Using the machine description, the required task, and my suggested initial design, create your own object-oriented design for the Vole Machine Simulator.

Consider my suggested design as a starting point. You may keep, change, split, merge, add or remove classes where appropriate, but explain every significant design decision and why it improves on my version.
For each class give: its responsibility, its attributes (with types), its methods (with parameters and return types), and its relationships with other classes.
Explain how the classes work together during a full run (loading, fetch, decode, execute, PC update, display, and handling invalid instructions).
Make sure every feature in section 1 is covered by some class.
Apply good OOP principles (single responsibility, encapsulation, low coupling), and keep the design suitable for an intermediate-level C++ project, not over-engineered.
Provide the final design as complete PlantUML code that I can paste into plantuml.com to generate the UML class diagram, with correct relationships (composition, aggregation, association, dependency, inheritance) and multiplicities where useful.
Please answer in this order: (a) list of classes and their responsibilities, (b) explanation of your design decisions compared to my initial design, (c) how the classes interact, (d) the PlantUML code.

After you answer, I will ask you follow-up questions about your choices and we will improve the design together, so please be ready to justify and refine it. Vole Machine Simulator — OOP Design (a) Classes and Responsibilities Below is the class catalogue. I split responsibilities more finely than your draft so that each class has a single reason to change, while keeping the total count small enough for an intermediate C++ project.

Memory Responsibility: Store and expose the 256 memory cells; handle the screen-mapped cell (FF) transparently by delegating to Screen.
Attributes:

cells : std::array<uint8_t, 256>
Methods:

read(uint8_t address) : uint8_t

write(uint8_t address, uint8_t value) : void (routes address FF to Screen)

reset() : void

display(std::ostream&) : void

Relationships: Aggregated by VoleMachine; associated with Screen (pointer used only for address FF).

Screen Responsibility: Hold and render the output produced by writing to memory address FF.
Attributes:

output : std::string
Methods:

write(char c) : void

clear() : void

display(std::ostream&) : void

isEmpty() const : bool

Relationships: Owned by VoleMachine; referenced by Memory.

RegisterFile Responsibility: Store the 16 general-purpose registers.
Attributes:

regs : std::array<uint8_t, 16>
Methods:

get(uint8_t index) : uint8_t

set(uint8_t index, uint8_t value) : void

reset() : void

display(std::ostream&) : void

Relationships: Composed by CPU.

Instruction Responsibility: A value object representing one decoded 16-bit instruction. Parses and validates its own fields; holds no machine state.
Attributes:

raw : uint16_t

opcode : uint8_t

r : uint8_t

s : uint8_t

t : uint8_t

xy : uint8_t

Methods:

Instruction(raw : uint16_t)

getOpcode() const : uint8_t

getR() const / getS() const / getT() const : uint8_t

getXY() const : uint8_t

getRaw() const : uint16_t

isValid() const : bool

static fromHexString(const std::string&) : std::optional

toString() const : std::string

Relationships: Dependency of CPU (created per fetch); produced by ProgramLoader.

ALU (Arithmetic/Logic Unit) Responsibility: Pure functions for integer add, floating-point add, OR, AND, XOR, rotate. No state, no memory/register access.
Methods (all static):

addInt(a, b) : uint8_t

addFloat(a, b) : uint8_t

bitOr(a, b) : uint8_t

bitAnd(a, b) : uint8_t

bitXor(a, b) : uint8_t

rotateRight(value, times) : uint8_t

Relationships: Dependency of CPU.

CPU Responsibility: Own the PC, IR, register file; orchestrate fetch → decode → execute → PC update. It does not own memory or screen — it receives them by reference/pointer.
Attributes:

pc : uint8_t

ir : uint16_t

registers : RegisterFile

halted : bool

memory : Memory*

Methods:

CPU(Memory&)

setPC(uint8_t) : void

getPC() const : uint8_t

getIR() const : uint16_t

step() : StepResult (fetch + decode + execute + PC update; returns enum: OK, HALTED, INVALID)

reset() : void

isHalted() const : bool

displayState(std::ostream&) const : void

fetch() : uint16_t

execute(const Instruction&) : StepResult

Relationships: Composed by VoleMachine; aggregates RegisterFile; associates Memory; depends on Instruction and ALU.

ProgramLoader Responsibility: Read a program text file, validate each line, produce a list of Instructions. Knows nothing about memory layout policy beyond being told a start address.
Methods:

loadFromFile(path : const std::string&, startAddress : uint8_t) : LoadResult
LoadResult = { std::vector instructions; std::string error; bool ok; }

isValidStartAddress(uint8_t) const : bool

parseLine(const std::string&) : std::optional

Relationships: Depends on Instruction; used by VoleMachine.

MachineController (the VoleMachine façade) Responsibility: Owns memory, screen, CPU. Presents the operations the menu needs: load, edit register/memory, set PC, run, step, reset, display.
Attributes:

memory : Memory

screen : Screen

cpu : CPU

loader : ProgramLoader

Methods:

loadProgram(path, startAddress) : bool

overwriteRegister(idx, value) : void

overwriteMemory(addr, value) : void

setPC(addr) : void

run() : RunResult

step() : StepResult

reset() : void

displayAll(std::ostream&) const : void

isHalted() const : bool

Relationships: Composition of Memory, Screen, CPU, ProgramLoader.

Menu (user interface) Responsibility: Present the menu, read user input, invoke MachineController methods, handle "load another program / exit" loop. This separates I/O from the domain model.
Attributes:

controller : MachineController&
Methods:

run() : void
showMainMenu() : void

handleLoad() / handleEditRegister() / handleEditMemory() / handleSetPC() / handleRun() / handleStep() / handleDisplay() : void

Relationships: Association (uses) MachineController.

StepResult / RunResult (small enums/structs) enum class StepStatus { OK, HALTED, INVALID_INSTRUCTION }
enum class RunStatus { COMPLETED, HALTED, INVALID_INSTRUCTION }

Free-standing, used by CPU and MachineController.

(b) Design Decisions vs. Your Initial Design Your design My design Why Registers class Renamed RegisterFile Clearer name; it's a file/array of registers, not the registers themselves. CPU owns Registers, Instruction CPU composes RegisterFile, depends on Instruction Instruction is a transient value object created per fetch — it should not be a composed member of CPU. Composition would falsely imply CPU stores one Instruction permanently. Instruction had parse()/validate() mutating its own state Instruction is an immutable value object; validation happens in constructor/isValid(); parsing from text is a static factory fromHexString Immutability removes invalid intermediate states and makes the class easy to test. No ALU Added ALU (stateless) Keeps CPU::execute short; every arithmetic/bit op is in one testable place. SRP. VoleMachine mixed domain + I/O concerns Split into MachineController (domain façade) and Menu (UI) Separating the menu allows headless unit tests and a future GUI without touching domain logic. File loading inside VoleMachine New ProgramLoader class File parsing/validation is a distinct responsibility (and reusable for "load another program"). Screen as a field on VoleMachine but not linked to Memory Memory holds a pointer to Screen and routes writes to address FF Encapsulates the memory-mapping rule inside Memory where it belongs, instead of leaking it into CPU's execute. CPU had updatePC() as a public method PC update is inside step() (private detail) PC update is an invariant of the fetch-decode-execute cycle; exposing it invites bugs. decodeInstruction public Decoding is implicit in Instruction's constructor/factory Fewer moving parts; the "decode" step is "construct an Instruction from the IR value." No result types Added StepStatus / RunStatus / LoadResult Needed to signal HALTED and INVALID_INSTRUCTION up to the menu for feature #10 and #11. reset() on VoleMachine reset() on MachineController + CPU::reset + RegisterFile::reset Each object resets its own state; controller only coordinates. display() scattered Each class exposes display(std::ostream&); MachineController::displayAll composes them Cohesive printing per class, no giant print-everything method. Kept from your design: Memory, Screen, CPU, Instruction, a façade (VoleMachine → MachineController), and the composition relationships MachineController *-- Memory/CPU/Screen, CPU *-- RegisterFile, CPU --> Memory.

(c) How the Classes Interact Loading a program (features 2, 10, 11): Menu::handleLoad() asks the user for a file path and start address → calls MachineController::loadProgram(path, start). The controller asks ProgramLoader::loadFromFile(...). The loader:

Rejects odd addresses or >= 0x100 (isValidStartAddress).

Reads each line, strips comments after the first token, ignores blank/; lines.

Calls Instruction::fromHexString per line; if any returns nullopt or isValid() is false, returns {ok=false, error=...}.

Returns LoadResult with the vector of validated Instructions.

On success the controller writes each instruction's two bytes into Memory::write(start + 2*i, …). On failure nothing is written, satisfying requirement 10 ("rejecting invalid programs").

Setting PC (feature 4): Menu → MachineController::setPC(addr) → CPU::setPC(addr).

Step (feature 6): Menu::handleStep() → MachineController::step() → CPU::step():

fetch(): ir = (mem[pc] << 8) | mem[pc+1].

Decode: construct Instruction(ir).

if (!instr.isValid()) return INVALID_INSTRUCTION;

execute(instr) — switch on opcode, use RegisterFile::get/set, Memory::read/write, ALU::*. JUMP (opcode B) sets PC directly; other instructions leave PC at pc+2. HALT (C) sets halted = true.

Return StepStatus.

Menu then calls MachineController::displayAll() and, for step mode, waits for Enter.

Run all (feature 7, 8): Menu::handleRun() loops controller.step() until HALTED or INVALID_INSTRUCTION; then displayAll(). Invalid instruction during a run is reported to the user (requirement 10 during execution).

Display (feature 5, 8, 9): MachineController::displayAll calls Memory::display, CPU::displayState (which calls RegisterFile::display and prints PC/IR), and Screen::display.

Reset (for "load another program"): MachineController::reset() → memory.reset(), screen.clear(), cpu.reset().

Menu loop (feature 11): Menu::run() loop; after a program finishes it asks "exit / load another?".

(d) PlantUML Code plantuml @startuml VoleMachine

skinparam classAttributeIconSize 0

' ---------- Small value types ---------- enum StepStatus { OK HALTED INVALID_INSTRUCTION }

enum RunStatus { COMPLETED HALTED INVALID_INSTRUCTION }

class LoadResult <> {

instructions : std::vector
ok : bool
error : std::string }
' ---------- Memory & Screen ---------- class Memory {

cells : std::array<uint8_t, 256>
screen : Screen*
Memory(screen : Screen&)
read(address : uint8_t) : uint8_t
write(address : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
class Screen {

output : std::string
write(c : char) : void
clear() : void
isEmpty() const : bool
display(out : std::ostream&) const : void }
Memory --> Screen : "address FF"

' ---------- Registers ---------- class RegisterFile {

regs : std::array<uint8_t, 16>
get(index : uint8_t) const : uint8_t
set(index : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
' ---------- Instruction ---------- class Instruction {

raw : uint16_t
opcode : uint8_t
r : uint8_t
s : uint8_t
t : uint8_t
xy : uint8_t
Instruction(raw : uint16_t)
{static} fromHexString(line : const std::string&) : std::optional
isValid() const : bool
getOpcode() const : uint8_t
getR() const : uint8_t
getS() const : uint8_t
getT() const : uint8_t
getXY() const : uint8_t
getRaw() const : uint16_t
toString() const : std::string }
' ---------- ALU ---------- class ALU <> {

{static} addInt(a : uint8_t, b : uint8_t) : uint8_t
{static} addFloat(a : uint8_t, b : uint8_t) : uint8_t
{static} bitOr(a : uint8_t, b : uint8_t) : uint8_t
{static} bitAnd(a : uint8_t, b : uint8_t) : uint8_t
{static} bitXor(a : uint8_t, b : uint8_t) : uint8_t
{static} rotateRight(v : uint8_t, n : uint8_t) : uint8_t }
' ---------- CPU ---------- class CPU {

pc : uint8_t
ir : uint16_t
halted : bool
registers : RegisterFile
memory : Memory*
CPU(memory : Memory&)
setPC(addr : uint8_t) : void
getPC() const : uint8_t
getIR() const : uint16_t
isHalted() const : bool
step() : StepStatus
reset() : void
displayState(out : std::ostream&) const : void
fetch() : uint16_t
execute(instr : const Instruction&) : StepStatus }
CPU *-- RegisterFile CPU --> Memory CPU ..> Instruction : creates per fetch CPU ..> ALU CPU ..> StepStatus

' ---------- Program Loader ---------- class ProgramLoader {

loadFromFile(path : const std::string&, startAddress : uint8_t) : LoadResult
isValidStartAddress(addr : uint8_t) const : bool
parseLine(line : const std::string&) : std::optional }
ProgramLoader ..> Instruction ProgramLoader ..> LoadResult

' ---------- Controller (façade) ---------- class MachineController {

memory : Memory
screen : Screen
cpu : CPU
loader : ProgramLoader
MachineController()
loadProgram(path : const std::string&, startAddress : uint8_t) : bool
overwriteRegister(index : uint8_t, value : uint8_t) : void
overwriteMemory(address : uint8_t, value : uint8_t) : void
setPC(address : uint8_t) : void
run() : RunStatus
step() : StepStatus
reset() : void
displayAll(out : std::ostream&) const : void
isHalted() const : bool }
MachineController *-- Memory MachineController *-- Screen MachineController *-- CPU MachineController *-- ProgramLoader MachineController ..> StepStatus MachineController ..> RunStatus

' ---------- Menu (UI) ---------- class Menu {

controller : MachineController&
Menu(controller : MachineController&)
run() : void
showMainMenu() const : void
handleLoad() : void
handleEditRegister() : void
handleEditMemory() : void
handleSetPC() : void
handleRun() : void
handleStep() : void
handleDisplay() : void }
Menu --> MachineController : uses

' ---------- Notes on multiplicities ---------- MachineController "1" *-- "1" Memory MachineController "1" *-- "1" Screen MachineController "1" *-- "1" CPU MachineController "1" *-- "1" ProgramLoader CPU "1" *-- "1" RegisterFile CPU "1" --> "1" Memory : reads/writes

@enduml Coverage check against Section 1 Feature Handled by

Menu Menu
Load program from file, even address < FF, ignore comments/blanks ProgramLoader, MachineController::loadProgram
Overwrite register / memory MachineController::overwriteRegister/overwriteMemory
Set PC MachineController::setPC → CPU::setPC
Display memory matrix, IR, PC, registers Memory::display, CPU::displayState, RegisterFile::display, composed by MachineController::displayAll
Fetch/decode/validate/execute/update PC CPU::step + Instruction::isValid
Run all / step MachineController::run/step
Display after full run + screen displayAll (includes Screen::display)
Display after each step, wait Enter Menu::handleStep
Reject invalid programs (load and during execution) ProgramLoader returns ok=false; CPU::step returns INVALID_INSTRUCTION
Ask exit / load another Menu::run loop I'm ready for your follow-up questions — some areas we might want to discuss next are whether Instruction should stay fully immutable, whether Memory holding a Screen* is the cleanest form of the memory-mapping, and whether MachineController should own Screen or let Memory own it directly.
Why does Memory hold a pointer to Screen? Doesn't this couple two classes that should be independent? Would it be better to handle the address FF mapping in the CPU or in the STORE instruction logic instead? Why Memory holds a Screen* — and why I'd reconsider it Short answer: you're right to push back. It's the weakest part of my design. Let me explain what I was trying to achieve, why it's genuinely problematic, and what the better alternatives are.

What I was trying to achieve The machine description says FF is a memory-mapped cell: "when XY = FF, the store writes to the screen (FF is a memory mapping for the screen)." The word memory-mapped strongly suggests that from the CPU's point of view, address FF is memory — the CPU shouldn't need a special case. So I pushed the mapping down into Memory::write, so CPU::execute for opcode 3 could stay uniform:

cpp // in CPU::execute, opcode 3 (STORE): memory->write(instr.getXY(), registers.get(instr.getR())); No if (xy == 0xFF) in the CPU. That was the appeal: the CPU treats all 256 addresses identically, and the "FF is special" rule lives in exactly one place — Memory.

Why it's a real coupling problem You've identified a legitimate design smell. The issues:

Memory now has two reasons to change. It is supposed to model "256 bytes of storage." Now it also knows about the screen — a peripheral. If the screen's interface changes (say it becomes a std::vector buffer, or later a framebuffer with cursor position), Memory has to change too. That violates SRP.

It drags Screen into the domain's storage layer. Memory is the most reusable, testable class in the design — you can unit-test reads/writes with no peripherals at all. Making it hold a Screen* means every Memory test now needs a Screen, or a null pointer, which invites null-check noise and a lifetime question: who guarantees the Screen outlives the Memory?

The dependency direction is questionable. Peripherals (Screen) are usually clients of memory or peers of memory, not things memory knows about. The arrow Memory --> Screen reads oddly: storage depending on an output device.

It's a form of feature envy / hidden policy. The STORE-to-screen rule is instruction semantics, not storage semantics. It belongs with the instruction execution logic, not with the byte array.

So: the goal (one place for the FF rule, CPU stays uniform) is good; the placement (inside Memory) is not.

The three alternatives Option A — Handle it in the CPU's STORE logic cpp // CPU::execute, opcode 3: uint8_t xy = instr.getXY(); if (xy == 0xFF) screen->write(static_cast(registers.get(instr.getR()))); else memory->write(xy, registers.get(instr.getR())); Pros: Memory becomes a pure, dependency-free byte store. The special case sits where the instruction semantics live. Easy to test both paths.

Cons: CPU now needs a Screen* (or a reference), so CPU knows about a peripheral. It also means the CPU has a if (xy == FF) branch — the uniformity I was chasing is lost. And strictly, FF is no longer "real" memory: Memory::read(0xFF) would return garbage/zero, which is inconsistent with "memory-mapped."

Verdict: Simple and honest, but it couples CPU to Screen, which is arguably a worse coupling than Memory→Screen, because CPU is the core of the domain and Screen is a peripheral.

Option B — A dedicated IoBus / MemoryMappedIO layer Introduce a small intermediary that both CPU and the peripherals route through:

plantuml class MemoryBus {

ram : Memory
screen : Screen*
read(addr) : uint8_t
write(addr, value) : void ' routes 0xFF -> screen } CPU talks only to MemoryBus. Memory stays pure. Screen stays pure.
Pros: Clean separation; the mapping policy lives in one focused class; Memory and Screen are both independently testable; CPU doesn't know about Screen. Extensible if you ever add more mapped addresses.

Cons: One more class — for a 16-bit toy machine with exactly one mapped address, this is arguably over-engineering (you explicitly asked me not to over-engineer). It also means the "memory-mapped" illusion is provided by the bus, not by Memory, which is fine but slightly more indirection than the problem warrants.

Verdict: The "textbook correct" answer, but probably too much machinery for one special address in an intermediate project.

Option C — Keep the mapping in Memory, but inject it as an interface (my mild preference) If we want to preserve the "CPU sees uniform memory" property and avoid Memory depending on the concrete Screen, invert the dependency with a tiny abstraction:

cpp class MappedDevice { // interface public: virtual void write(uint8_t value) = 0; virtual ~MappedDevice() = default; };

class Screen : public MappedDevice { ... };

class Memory { std::array<uint8_t,256> cells; MappedDevice* mapped = nullptr; // device mapped at 0xFF public: void setMappedDevice(MappedDevice* d) { mapped = d; } void write(uint8_t addr, uint8_t v) { if (addr == 0xFF && mapped) { mapped->write(v); return; } cells[addr] = v; } }; Pros: Memory depends on an abstraction, not on Screen. It's still a pure byte store for all other addresses. The FF rule is in one place. CPU stays uniform. You can test Memory with a fake MappedDevice. No new "bus" class.

Cons: Still a Memory --> MappedDevice dependency; still a lifetime/ownership question (mitigated by MachineController owning both and wiring them in its constructor). Slightly more code than a raw Screen*.

Verdict: Good balance — but honestly, for a single mapped cell in a teaching simulator, this is arguably ceremony. It's the right answer if you value the abstraction; the raw Screen* is the pragmatic version of the same idea.

My honest recommendation Given your constraints ("intermediate-level, not over-engineered"), I'd rank:

Option A (CPU handles FF) — if you're comfortable with CPU knowing about Screen. It's the most direct reading of "STORE to FF writes to the screen," keeps Memory pure, and adds zero classes. The cost is a CPU --> Screen association, which is defensible because the CPU is the thing executing I/O instructions.

Option C with a raw Screen* (my original, minus the abstraction) — if you want the CPU to stay uniform. Accept that Memory has one peripheral dependency, document it, and own both objects in MachineController so lifetime is trivial. This is what my draft did; it's pragmatic, just not maximally decoupled.

Option B (MemoryBus) — only if you anticipate more mapped devices, or if the assignment rewards clean layering over minimalism.

Option C with MappedDevice interface — if you want the decoupling of B without the extra bus class.

What I'd actually change in the design I think Option A is the cleanest for this specific problem, because:

The FF rule is fundamentally instruction semantics ("STORE to FF means write to screen"), not storage semantics. Putting it in CPU::execute's STORE branch is where a reader looks for it.

Memory reverts to a pure, trivially testable byte array — which is the class most likely to be unit-tested in isolation.

The CPU --> Screen association is honest: the CPU executes I/O. It's one association, not a hidden policy inside storage.

It costs one if and no new classes.

The trade-off I accept: Memory::read(0xFF) no longer magically returns screen content — but note the spec never says reading FF returns the screen; only STORE (opcode 3 with XY=FF) is defined as writing to the screen. So there's no inconsistency to worry about. If a future requirement said "LOAD from FF reads the screen," then the mapping would genuinely be a memory-address concern and Option B/C would become the right call.

Concrete change if you pick Option A plantuml class Memory {

cells : std::array<uint8_t, 256>
read(address : uint8_t) : uint8_t
write(address : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
class CPU {

pc : uint8_t
ir : uint16_t
halted : bool
registers : RegisterFile
memory : Memory*
screen : Screen* ' new: for STORE to FF ... }
CPU --> Memory CPU --> Screen : "STORE when XY == FF" ' Memory no longer knows about Screen at all MachineController still owns Memory, Screen, CPU and wires CPU(memory, screen) in its constructor. The Memory --> Screen arrow disappears entirely.

Bottom line: you're correct that Memory --> Screen is an avoidable coupling, and my justification ("keep CPU uniform") doesn't outweigh the SRP/lifetime costs. I'd move the FF rule into CPU's STORE path (Option A) for this project, and keep Option B in your back pocket only if the spec later adds more mapped addresses or a LOAD-from-FF behavior. Want me to regenerate the full PlantUML with Option A applied so you can compare the two diagrams side by side?

The example program writes the number 12 to the screen. Why does Screen::write take a char? Should it store the raw value and print it as a number or hex? Screen::write(char) vs raw value — what does "write 12 to the screen" actually mean? Good catch. This is a specification question disguised as a type question, and my char signature quietly made a choice the spec doesn't actually make for us. Let me lay out the ambiguity, then give a concrete recommendation.

The ambiguity The machine description says:

STORE the content of register R in memory cell XY. When XY = FF, the store writes to the screen (FF is a memory mapping for the screen).

And your example program "writes the number 12 to the screen." But "12" could mean at least three different things:

The byte 0x12 — i.e., the program does STORE R, FF where R holds 0x12. The question is then: does the screen display 0x12 as a character (whatever glyph code 0x12 is — a non-printing control character), as hex (12), or as decimal (18)?

The value 12 decimal — register holds 0x0C, and the screen shows 12.

A character — register holds the ASCII code of some printable char, and the screen accumulates text.

The phrase "the number 12" in your example suggests the screen is being used as a numeric output, not as a character terminal. But the spec itself never says the screen renders characters — it only says "writes to the screen." That's the gap.

Why my char signature was an unforced assumption By writing Screen::write(char c), I silently committed to interpretation #3 (a character terminal). That's a common convention for memory-mapped screens (think a UART / text VDU), and it's what most "write to address FF" examples in toy machines do. But it has two problems here:

A control byte like 0x12 prints nothing visible in most terminals — so a program that "writes 0x12 to the screen" would appear to do nothing. That's a confusing behavior to ship in a teaching simulator.

It bakes a display policy into the type. Whether a byte is rendered as a glyph, a decimal number, or a hex number is a presentation decision. Putting char in the method signature says "this is text" — which may not match what the grader/user expects.

The real question: is Screen a character device or a numeric log? These are genuinely different models, and the right choice depends on what the course/spec intends. Let me spell both out.

Model 1 — Character terminal (my original) cpp class Screen { std::string output; public: void write(uint8_t value); // treat value as an ASCII/character code void clear(); void display(std::ostream&) const; }; write(0x48) appends 'H'.

write(0x12) appends a non-printing control char (invisible / odd).

Display: prints output as text.

Use when: the spec's example programs clearly emit printable ASCII (e.g., "write 65 to screen → shows 'A'"). Typical of memory-mapped text VDUs.

Model 2 — Numeric log / hex dump cpp class Screen { std::vector<uint8_t> values; // raw bytes written to FF, in order public: void write(uint8_t value); // store raw value, no interpretation void clear(); void display(std::ostream&) const; // print e.g. "12 0C FF ..." in hex }; write(0x12) records the byte 0x12; display shows 12.

The interpretation (hex vs decimal) lives in display(), not in write(), so you can change presentation without touching the storage contract.

Use when: the spec's "write 12 to screen" literally means "the screen shows 12", and there's no mention of characters/ASCII. This matches your example most directly if "12" means hex 0x12.

Model 3 — Hybrid (store raw, render per a display mode) cpp enum class ScreenMode { ASCII, HEX, DECIMAL };

class Screen { std::vector<uint8_t> buffer; ScreenMode mode = ScreenMode::HEX; public: void write(uint8_t value) { buffer.push_back(value); } void setMode(ScreenMode m) { mode = m; } void display(std::ostream&) const; // render buffer according to mode }; write is policy-free (just stores the byte). display applies a mode.

Lets the grader/tester choose; also the most testable (you can assert on the raw buffer).

Use when: you want to be safe against an ambiguous spec, or the spec allows both. Slight over-engineering for a one-line requirement, but tiny.

What I'd actually recommend Two honest options, depending on how much you trust the spec:

If the spec is genuinely ambiguous (which it reads like): use Model 3 with two modes, defaulting to HEX — because "write the number 12 to the screen" most naturally means "the screen shows 12", i.e. hex of 0x12, and hex is the machine's native notation (everything else in the machine is hex). Then a program that stores 0x12 to FF displays 12, matching your example. If a later requirement says "it's a text terminal," flip the default to ASCII without changing any other class.

cpp class Screen { - buffer : std::vector<uint8_t> - mode : ScreenMode + write(value : uint8_t) : void // stores raw byte + setMode(m : ScreenMode) : void + clear() : void + isEmpty() const : bool + display(out : std::ostream&) const : void // renders per mode }; If you're confident it's a character terminal (common in these simulators): keep write(uint8_t) but name it honestly — writeChar(uint8_t) or keep write(uint8_t value) with a doc comment "value is interpreted as an ASCII code." The important change from my draft is: uint8_t, not char, because the register holds a byte and the implicit char conversion is a silent policy decision. Let the method name / docs / display mode express the interpretation, not the parameter type.

Why uint8_t beats char regardless The register content is uint8_t. Screen::write(char) forces a narrowing/static_cast at the call site — noise, and a signedness trap (char may be signed; 0xFF becomes -1).

The interpretation (glyph vs number) is a presentation concern; the value being written is a byte. Keeping the parameter as uint8_t separates "what was written" from "how it's shown."

It makes Screen testable: you can assert screen.buffer() == {0x12} without worrying about char encoding.

Concrete change to the design Replace the char-based method with a byte-based one plus a display mode:

plantuml enum ScreenMode { ASCII HEX DECIMAL }

class Screen {

buffer : std::vector<uint8_t>
mode : ScreenMode
write(value : uint8_t) : void
setMode(m : ScreenMode) : void
clear() : void
isEmpty() const : bool
display(out : std::ostream&) const : void } And in CPU::execute (under the Option-A change we discussed, where CPU handles FF):
cpp // STORE, XY == 0xFF: screen->write(registers.get(instr.getR())); // raw byte; rendering is Screen's job So STORE R, FF with R = 0x12 records the byte 0x12, and Screen::display with mode == HEX prints 12 — matching your example. If the grader expects an ASCII terminal, setMode(ASCII) makes STORE R, FF with R = 0x41 print A. Same write contract either way.

The one thing to confirm with the spec owner Before committing: does "write 12 to the screen" mean the screen shows the two characters 1 2, or the single character with code 0x12? My recommendation (hex mode, store raw byte, render in display) satisfies "shows 12" and is the least committal. If the intended semantics are "character terminal," the only change is the default ScreenMode — no other class is affected. That's exactly the kind of decision you want isolated in one small class, which is the argument for keeping the raw byte + mode split rather than baking char into the signature.

Want me to fold this Screen revision (plus the Option-A Memory/CPU change) into the full PlantUML so you have one consolidated diagram?

In the assignment's example program, the registers hold 5 and 7, and the sum 12 is stored to FF. So register 2 holds 0x0C, not 0x12. With your default HEX mode, the screen would show "0C", not "12". Doesn't this mean the default should be DECIMAL? Also, if the program stores to FF multiple times, should the screen show all values or only the last one? And should STORE to FF also update memory cell FF so the memory matrix shows it? Three good questions — and one of them invalidates my default You're right on the first point, and it exposes a flaw in my reasoning. Let me take all three in turn.

Default mode: you're correct, HEX is wrong here Your trace is exactly right:
R1 = 5, R2 = 7

ADD R3, R1, R2 → R3 = 12 = 0x0C

STORE R3, FF → writes byte 0x0C to the screen.

Under my HEX default, display() would render 0x0C as the two characters 0C. But the assignment's own narration says the screen shows 12. So the spec's intended reading of "the number 12" is decimal, not hex. My default was backwards.

Worse, I justified HEX with "everything else in the machine is hex" — but that argument applies to instructions and addresses, not to screen output. The screen is a user-facing output device; the spec narrates it in decimal. So:

Default should be DECIMAL, not HEX.

Keep HEX and ASCII as selectable modes (cheap, and covers other possible programs), but the default must match the spec's example.

cpp enum class ScreenMode { DECIMAL, HEX, ASCII }; // default: ScreenMode::DECIMAL With that, STORE R3, FF where R3 = 0x0C displays 12. Matches the assignment.

One caveat worth flagging: decimal rendering of a byte gives 0–255, so 0x0C → 12 is unambiguous. If the intended semantics were signed (two's complement) decimal, 0xFF would show -1 rather than 255. The spec doesn't say, and the example (12) is positive so it doesn't disambiguate. I'd default to unsigned decimal (0–255) unless the spec says otherwise — it's the simpler contract, and matches how the register is described as a bit pattern. This is a one-line change in display() if you later discover the spec wants signed.

Multiple stores to FF: last-value or full history? This is a genuine spec gap, and the two answers produce different Screen designs:
Model H (history / log):

cpp class Screen { std::vector<uint8_t> buffer; // every byte ever written to FF ... }; display() prints all of them, in order, separated by spaces (or newlines). STORE FF three times with 1, 2, 3 → screen shows 1 2 3.

Model L (last value only):

cpp class Screen { uint8_t last = 0; bool hasValue = false; ... }; display() prints only the most recent byte. Three stores → screen shows 3.

Which does the spec want? It doesn't say. But we can infer from the assignment's framing:

The example writes once and narrates a single number. It doesn't demonstrate accumulation.

The spec calls FF a "memory mapping for the screen" — a memory cell holds one value, not a history. If the mental model is "FF is a one-byte output port," then Model L is the faithful reading: each store overwrites the port, and the screen shows what's currently in it.

If the intent were a text terminal (typing out a string one char at a time), Model H would be required — but then the example's 12 would be a two-char decimal rendering, which contradicts the "one store → one number" reading.

So the spec's own example leans toward Model L: last value only. That also keeps Screen a trivial one-field class, consistent with "FF is just a memory cell that happens to be displayed."

However, there's a subtlety: if Screen holds only the last value, the screen output after a program is just one number, which makes the "display the screen" feature (#8, #9) nearly trivial. That's fine — the spec asks for it, not for a rich terminal. And it means Screen::display and Memory::display(0xFF) would show the same value, which brings us to your third question.

My recommendation: default to Model L (last value), because it matches "FF is a memory cell" and the single-write example. If the grader's hidden tests write multiple values and expect accumulation, the only change is swapping uint8_t last for std::vector<uint8_t> buffer inside Screen — no other class changes. That's the payoff of keeping the rendering policy inside Screen.

cpp class Screen { - value : uint8_t - hasValue : bool - mode : ScreenMode + write(v : uint8_t) : void // overwrites value, sets hasValue + setMode(m : ScreenMode) : void + clear() : void + isEmpty() const : bool + display(out : std::ostream&) const : void }; If you later decide you want a log, write becomes buffer.push_back(v) and display iterates — the public contract (write, display, clear, isEmpty) is unchanged.

Should STORE to FF also update memory cell FF? This is the most interesting question, and it's where the "memory-mapped" wording bites.
Two defensible semantics:

Semantics A — FF is a real cell that also drives the screen (dual write). Memory::write(0xFF, v) stores v in cells[0xFF] and forwards to Screen. The memory matrix shows 0C at FF, and the screen shows 12 (decimal render of the same byte). LOAD R, FF would then read back 0x0C — consistent, because FF behaves like ordinary memory that happens to have an output side-effect.

Semantics B — FF is a port, not storage (screen-only). STORE to FF goes only to Screen; cells[0xFF] stays 0x00 (or whatever). The memory matrix shows 00 at FF, and the screen is the only place the value appears. LOAD R, FF reads 0x00 — the value is write-only.

Which is right? The spec says FF is "a memory mapping for the screen" and describes only the store direction ("the store writes to the screen"). It never says LOAD from FF reads the screen, and it never says FF holds the value. That silence is meaningful:

If FF were a real cell, the spec would likely have said "STORE ... and also display it" rather than calling it a memory mapping.

The word "mapping" suggests the address is redirected to the device, i.e., Semantics B.

But Semantics A is what most students implement, and it makes the memory matrix "show the screen value too," which is visually satisfying and matches a naive reading of "memory-mapped."

My recommendation: Semantics B (port / screen-only), with a clear display note. Reasons:

It's the honest reading of "mapping." A mapped address is a window onto a device, not a duplicate store.

It avoids a hidden inconsistency. Under Semantics A, cells[0xFF] and Screen must be kept in sync forever — two sources of truth for one logical value. If someone calls MachineController::overwriteMemory(0xFF, 0x42), does the screen update too? Under A you must decide; under B the answer is "FF is the screen, so yes, writing FF via any path drives the screen." Cleaner: all writes to FF go through one function that targets Screen, and cells[0xFF] is simply unused.

It makes the display rule simple: Memory::display shows cells[0x00..0xFE]; address 0xFF is shown as a marker (e.g., FF: [screen]) or omitted, and Screen::display shows the actual value. No duplication, no sync bug.

But — and this matters for grading — Semantics A is safer against a grader that checks "memory[FF] equals the stored value." If the hidden tests do STORE R, FF then inspect memory[0xFF], Semantics B fails them. So:

If you control the tests or the spec is loose → B (cleaner).

If a grader may inspect memory[0xFF] → A (defensive), and make Memory::write(0xFF, v) do cells[0xFF] = v; screen->write(v); so both agree by construction. The "two sources of truth" worry disappears if the only way to write FF is through that one function — which it is, as long as you don't expose cells directly.

Given this is a course assignment and you don't know the grader, I'd lean A, implemented defensively:

cpp void Memory::write(uint8_t addr, uint8_t value) { if (addr == 0xFF) { cells[0xFF] = value; // keep matrix consistent for graders if (screen) screen->write(value); return; } cells[addr] = value; } This means we're back to Memory holding a Screen* — which is exactly the coupling you flagged last round. So there's a real tension:

If you chose Option A (CPU handles FF) from the previous round, then the dual-write would live in CPU::execute's STORE branch: memory->write(0xFF, v); screen->write(v); — Memory stays pure, FF cell still updated, screen driven. This is probably the best of both: CPU does the dual write, Memory knows nothing about Screen, and memory[0xFF] is kept in sync for graders.

If you kept the Memory --> Screen pointer, put the dual write in Memory::write.

Either way, the behavior is: cells[0xFF] == value and Screen shows value. Only the location of the dual-write differs.

Updated answers summary Question My answer Change from before Default screen mode DECIMAL (unsigned, 0–255) was HEX — wrong per your trace Multiple stores to FF Last value only (Model L) by default new decision; log is a one-field swap if needed STORE to FF updates cells[0xFF]? Yes, defensively (dual write), but located in CPU::execute (Option A), not Memory, to avoid the Memory --> Screen coupling reconciles your earlier coupling objection with grader safety Updated Screen and Memory/CPU sketch plantuml enum ScreenMode { DECIMAL HEX ASCII }

class Screen {

value : uint8_t
hasValue : bool
mode : ScreenMode
write(v : uint8_t) : void
setMode(m : ScreenMode) : void
clear() : void
isEmpty() const : bool
display(out : std::ostream&) const : void }
class Memory {

cells : std::array<uint8_t, 256>
read(address : uint8_t) : uint8_t
write(address : uint8_t, value : uint8_t) : void ' pure store; no Screen knowledge
reset() : void
display(out : std::ostream&) const : void }
class CPU { ...

memory : Memory*
screen : Screen* ... } CPU --> Memory CPU --> Screen : "STORE when XY == FF (dual write)"
' In CPU::execute, STORE branch: ' if (xy == 0xFF) { memory->write(0xFF, v); screen->write(v); } ' else { memory->write(xy, v); } This gives you: decimal screen output (12 for 0x0C), last-value semantics, memory[0xFF] consistent for graders, and no Memory --> Screen dependency.

The one thing I'd genuinely verify with whoever owns the spec: is the screen a one-value port (last write wins) or a log? My answer assumes port. If their hidden tests expect a log, only Screen changes — write appends instead of overwrites, display iterates — and every other class stays identical. That isolation is the whole point of keeping the buffer policy inside Screen.

Want me to produce the consolidated final PlantUML with all three rounds of changes (Option A CPU, uint8_t + DECIMAL/last-value Screen, dual-write in CPU) so you have a single diagram to review?

The assignment says the screen content is displayed after the program finishes running, and the program may store to FF several times (for example, printing several results). Last-value-only would lose earlier outputs. Please switch Screen to a history model (a vector of all values written, displayed in order), and keep DECIMAL as default. Also: MachineController::overwriteMemory(0xFF, v) from the menu would write directly to Memory and bypass the screen. How should this case be handled? Finally, do you really need the ASCII mode, or are DECIMAL and HEX enough? Screen as a history log, the manual-FF-write bypass, and pruning ASCII Three good corrections. The first two change the design; the third lets us delete code. Let me take them in order.

Screen becomes a history log (Model H) You're right — I over-weighted the single-write example. The assignment explicitly says the screen is shown after the program finishes, and a program "may store to FF several times (for example, printing several results)." Last-value-only silently discards everything but the final store, which contradicts that. So:
cpp enum class ScreenMode { DECIMAL, HEX }; // ASCII removed — see §3

class Screen { - buffer : std::vector<uint8_t> // every value written to FF, in order - mode : ScreenMode // default: ScreenMode::DECIMAL public: void write(uint8_t v); // buffer.push_back(v) void setMode(ScreenMode m); void clear(); // buffer.clear() bool isEmpty() const; void display(std::ostream&) const;// render all entries, space-separated, per mode }; display under DECIMAL for the example (5 + 7 = 12, stored once) prints 12. For a program that stores 0x0C, 0x0C, 0x0C it prints 12 12 12. Under HEX it prints 0C 0C 0C. The separator is a display concern — keep it in display, not in write, so the buffer stays a pure record of what was written.

Two small design notes on the history model:

write stays policy-free. It appends the raw byte; rendering (decimal/hex, separator, trailing newline) lives entirely in display. This is why the mode can be flipped without touching write or any caller.

Empty screen renders as empty, not 0. isEmpty() exists so displayAll can print something like Screen: (empty) rather than a bare blank line — a tiny UX nicety, and it keeps Screen::display from inventing output when nothing was written.

The public contract (write, clear, isEmpty, display, setMode) is identical to the last-value version, so nothing outside Screen changes. That was the point of isolating the buffer policy — swapping uint8_t value for std::vector<uint8_t> buffer is a one-class edit.

The overwriteMemory(0xFF, v) bypass — the real problem This is the sharpest issue you've raised, and it's a consequence of the Option-A decision (CPU does the dual write) from two rounds ago. Trace it:
CPU::execute's STORE branch handles FF by calling screen->write(v) and memory->write(0xFF, v). Good.

But MachineController::overwriteMemory(addr, v) (menu feature #3) calls memory->write(addr, v) directly. If addr == 0xFF, that updates cells[0xFF] but never touches Screen. So the menu's "overwrite memory" and the program's STORE disagree about what writing FF means. The screen silently misses a value that the memory matrix shows.

That's a genuine inconsistency: two code paths write FF, only one drives the screen. The fix has to guarantee that every write to FF — from the CPU, from the menu, from anywhere — goes through the same "store and log to screen" logic. There are three clean ways to get there; they differ in where the FF rule lives.

Fix 1 — Put the FF rule back in Memory (revisit the coupling decision) cpp void Memory::write(uint8_t addr, uint8_t value) { cells[addr] = value; // FF cell kept in sync for graders if (addr == 0xFF && screen) screen->write(value); } Then CPU::execute's STORE branch is just memory->write(xy, v) — uniform again — and overwriteMemory(0xFF, v) works for free. This is exactly the Memory --> Screen coupling you objected to two rounds ago. The objection was valid then, when only the CPU wrote FF and the mapping was pure instruction semantics. Now that the menu is also a writer of FF, the "FF means store-and-screen" rule is genuinely a property of the address, not of any one caller — and the address is Memory's domain. The coupling is more defensible now because the rule has two clients, not one.

Fix 2 — Put the FF rule in MachineController and route everything through it cpp void MachineController::writeByte(uint8_t addr, uint8_t v) { memory.write(addr, v); if (addr == 0xFF) screen.write(v); } // CPU doesn't get memory/screen directly for writes; it calls back through a // small interface (or MachineController injects a write-callback into CPU). This keeps Memory pure and CPU ignorant of Screen, and makes the controller the single chokepoint. Cost: CPU can no longer call memory->write directly for STORE — it needs a callback / interface (IByteSink) or the controller must own the execute loop. That's more machinery than this project warrants, and it partially undoes the clean CPU/Memory split.

Fix 3 — Keep the dual write in CPU, and special-case the menu path cpp // CPU::execute STORE branch: if (xy == 0xFF) { memory->write(0xFF, v); screen->write(v); } else { memory->write(xy, v); }

// MachineController::overwriteMemory: void overwriteMemory(uint8_t addr, uint8_t v) { if (addr == 0xFF) { memory.write(0xFF, v); screen.write(v); } else { memory.write(addr, v); } } Works, but now the FF rule is duplicated in two places. The moment a third writer appears (a future "load raw bytes" feature, a test harness), someone forgets the second half and the bug returns. This is the "two sources of truth" smell in a different guise.

Recommendation: Fix 1 The deciding argument is the one that flipped between rounds. When I argued for Option A (CPU handles FF), the menu wasn't a writer of FF — the rule had exactly one client (the STORE instruction), so it belonged with instruction semantics. You've now shown that the menu is a second client of "write FF" (feature #3 lets the user overwrite any cell, including FF). A rule with two clients that share only the address belongs with the address's owner, which is Memory. So:

Memory::write(0xFF, v) stores v in cells[0xFF] and forwards to Screen. One chokepoint, both writers covered, matrix stays consistent for graders.

CPU::execute's STORE branch reverts to the uniform memory->write(xy, v) — the if (xy == 0xFF) disappears from CPU entirely.

MachineController::overwriteMemory(addr, v) is just memory.write(addr, v) — FF handled transparently.

The CPU --> Screen association from Option A is removed; the dependency moves back to Memory --> Screen (or, better, Memory --> MappedDevice if you want the abstraction — but a raw Screen* is fine at this scale).

In other words: your very first objection to my original design was right for the wrong reason at the time, and has become right for the right reason now. The Memory --> Screen pointer I originally drew is the correct design once FF has multiple writers. I'd restore it, with a doc comment explaining that address FF is memory-mapped to the screen and that Memory::write is the single chokepoint for it.

One consequence to state plainly: Memory now depends on Screen, so Memory unit tests need a Screen (or the MappedDevice fake). That's the price of the chokepoint. Given the alternative is duplicated FF logic in two classes with a latent bug, it's the right trade for this project.

Do we need ASCII mode? — No, drop it You're right to question it. Two reasons to remove it:
Nothing in the spec calls for character output. The example writes 12 (decimal) to the screen; there's no mention of ASCII, text, terminals, or character codes anywhere in the machine description. ASCII was me hedging against an ambiguity that the history-model + decimal-default decision has now resolved. Keeping a mode nothing uses is dead code — and dead code in a teaching design is a liability, because the grader asks "what's this for?" and the honest answer is "nothing."

HEX earns its place; ASCII doesn't. HEX is genuinely useful for debugging (display the raw bytes as written) and is one line in display. ASCII would be useful only if a future requirement says "the screen is a text terminal" — and if that happens, adding a third enum value to ScreenMode and one branch in display is a five-minute change that touches one class. YAGNI applies: don't ship a mode for a spec that might exist.

So: enum class ScreenMode { DECIMAL, HEX }; with DECIMAL default. If the grader later wants ASCII, the change is isolated to Screen — which is exactly the property we've been preserving across all three rounds.

Updated pieces (deltas only) plantuml enum ScreenMode { DECIMAL HEX }

class Screen {

buffer : std::vector<uint8_t>
mode : ScreenMode
Screen()
write(v : uint8_t) : void
setMode(m : ScreenMode) : void
clear() : void
isEmpty() const : bool
display(out : std::ostream&) const : void }
class Memory {

cells : std::array<uint8_t, 256>
screen : Screen*
Memory(screen : Screen&)
read(address : uint8_t) : uint8_t
write(address : uint8_t, value : uint8_t) : void ' FF -> cells[FF] + screen->write
reset() : void
display(out : std::ostream&) const : void }
class CPU { ...

memory : Memory* ' no Screen* anymore ... } CPU --> Memory Memory --> Screen : "address FF is memory-mapped" ' CPU no longer knows about Screen; STORE is uniform memory->write(xy, v) And the two call sites that matter:
cpp // Memory::write — the single chokepoint for FF void Memory::write(uint8_t addr, uint8_t value) { cells[addr] = value; // keep matrix consistent if (addr == 0xFF && screen) screen->write(value); // history log }

// CPU::execute, STORE — uniform, no FF branch memory->write(instr.getXY(), registers.get(instr.getR()));

// MachineController::overwriteMemory — no FF branch either void MachineController::overwriteMemory(uint8_t addr, uint8_t v) { memory.write(addr, v); // FF handled inside Memory } Summary of the three decisions Question Answer Why Screen model History log (std::vector<uint8_t>, display in order) Assignment shows screen after the program and allows multiple stores — last-value loses outputs Default mode DECIMAL (kept) Your trace: 0x0C must show 12 overwriteMemory(0xFF, v) bypass Move FF rule into Memory::write (single chokepoint); remove CPU --> Screen; revert STORE to uniform memory->write FF now has two writers (CPU and menu); a rule with multiple clients belongs with the address's owner. Your original coupling objection is now the correct design, because the premise changed ASCII mode Drop it; keep DECIMAL + HEX No spec support; YAGNI; trivially re-addable inside Screen if a future requirement appears The through-line across all three rounds: every change has been confined to Screen and the single FF chokepoint (Memory::write), and no other class's contract moved. That's the payoff of having isolated the rendering policy and the mapping rule in the first place — and it's the strongest argument for the final shape.

Ready to fold all four rounds into one consolidated PlantUML when you are — or if you want to keep stress-testing first, the next thing I'd poke at is whether MachineController::reset() should also reset the screen buffer (I think yes) and whether the PC should reset to 0x00 or to the last loaded program's start address (the spec is silent, and it affects what "reset" means for feature #11's "load another program" flow).

Good, I accept Fix 1. Now a few more questions:

Who owns the run-all loop: Menu::handleRun or MachineController::run? What is the difference between RunStatus COMPLETED and HALTED? Please simplify if one of them is redundant.

After loading a program, who sets the PC to the start address automatically (the assignment requires this)? And in reset(), should the screen buffer be cleared, and should the PC go to 00 or to the last start address?

What happens if the PC reaches FF or passes the end of memory without a HALT? Who detects it?

Please unify naming: you used VoleMachine and MachineController, and StepResult and StepStatus in different places. Pick one. Four cleanup questions — and one naming inconsistency you caught All four are good. The first three are about who owns what invariant, and the fourth is a real sloppiness on my part. Let me answer each, then flag where I'd simplify.

Who owns the run-all loop? And COMPLETED vs HALTED The loop belongs in MachineController::run(). Reasons:

Menu is a user-interface class. Its job is to read input, call the controller, and print. If Menu::handleRun owns the while (!halted) step() loop, then the UI class encodes a domain loop condition — and any future front-end (a GUI, a test harness, a batch runner) has to re-implement it. That's duplicated policy in the wrong layer.

MachineController::run() can loop over CPU::step() and return a single RunStatus. Menu::handleRun then just calls controller.run(), displays, and reports. One loop, one place.

So handleRun becomes trivial:

cpp void Menu::handleRun() { RunStatus status = controller.run(); controller.displayAll(std::cout); report(status); // "program halted" / "invalid instruction at PC=.." } COMPLETED vs HALTED — you're right, one is redundant. In a machine with no exceptions and no concurrency, "the program finished" means "it hit HALT (opcode C)". There is no third way to finish. So COMPLETED is just HALTED under another name. Collapse to:

cpp enum class RunStatus { HALTED, // program reached HALT (C 000) INVALID_INSTRUCTION, // hit an invalid instruction during the run PC_OUT_OF_RANGE // ran off the end of memory (see §3) }; There's no RUNNING/COMPLETED because run() is synchronous — it either stops at a HALT, at an invalid instruction, or at a PC overflow, and those are exactly the three outcomes. If you later add a step-limit ("max N steps" guard), you'd add STEP_LIMIT_REACHED — but that's not in the spec, so don't pre-add it.

One consequence worth stating: CPU::step()'s result and run()'s result now overlap. step() returns the per-step outcome; run() returns the terminal outcome. They're different in scope, so both exist — but they should share the enum if the values are identical. Which brings us to §4.

Who sets PC after load? And what does reset() clear? Who sets the PC: MachineController::loadProgram should set it automatically on success, because the assignment requires it:
"Loading a new program ... Setting the PC to the start address of the program."

So loadProgram does:

cpp bool MachineController::loadProgram(const std::string& path, uint8_t start) { LoadResult r = loader.loadFromFile(path, start); if (!r.ok) return false; // nothing written, PC untouched for (size_t i = 0; i < r.instructions.size(); ++i) memory.write(start + 2i, (r.instructions[i].getRaw() >> 8) & 0xFF), memory.write(start + 2i + 1, r.instructions[i].getRaw() & 0xFF); cpu.setPC(start); // <-- automatic, as required cpu.resetHalt(); // clear halted flag from any prior run return true; } Note this means the menu's "set PC" feature (#4) is still useful — the user can override the PC manually after loading — but loading sets it for you. That matches the spec: load sets it; the user may also set it. Don't remove setPC from the menu.

What reset() does — and this is where I'd be opinionated:

Screen buffer: clear it. Yes. reset() means "return to a clean machine." If the screen buffer survived a reset, "load another program" (feature #11) would show the previous program's output mixed with the new one. Clear it.

PC: go to 0x00, not the last start address. Reasons:

"Reset" has a conventional meaning: all state returns to its power-on value. The PC's power-on value is 00. Going to "the last start address" is not a reset — it's a reload, and it requires reset() to remember a piece of state (lastStart) that exists only to make reset non-uniform. The spec already handles "PC should be at the start of the program" at load time (§2 first half). So reset() doesn't need to duplicate that. If the user wants to re-run the same program, they either step from the current PC (not resetting) or reload (which re-sets PC to the start). Making reset() remember lastStart couples it to the load flow and creates a subtle bug: reset → PC = last start, but registers/memory are now zeroed, so the program at that address no longer exists in memory. Pointless. Simpler code: pc = 0; ir = 0; halted = false; registers.reset(); memory.reset(); screen.clear(); — no stored lastStart field at all. So: reset() → PC = 0x00, clear screen, clear registers, clear memory, clear halted. Uniform, matches power-on, no hidden state. And loadProgram is the only thing that moves the PC to a program's start — exactly as the spec describes.

cpp void MachineController::reset() { memory.reset(); screen.clear(); cpu.reset(); // pc=0, ir=0, halted=false, registers zeroed } 3. PC reaching 0xFF / past end of memory without HALT This is a real edge case the spec doesn't mention, and it will happen with a malformed program (no HALT, or a JUMP off the end). Who detects it?

CPU::step() must detect it, because CPU owns the PC. The check belongs at the top of step(), before fetch:

cpp StepStatus CPU::step() { if (halted) return StepStatus::HALTED; if (pc > 0xFD) { // need 2 bytes: pc and pc+1 must both be < 0x100 halted = true; return StepStatus::PC_OUT_OF_RANGE; } // fetch, decode, execute, update pc ... } Why 0xFD, not 0xFF? Because an instruction is two bytes: it occupies pc and pc+1. If pc == 0xFE, the high byte is at 0xFE but the low byte would be at 0xFF — which is the screen-mapped cell, so you'd be reading the screen as half an instruction. If pc == 0xFF, the high byte itself is the screen cell. Neither is a valid fetch. So the last valid instruction start is 0xFD (occupying 0xFD and 0xFE). Anything > 0xFD is out of range.

Two sub-decisions:

pc > 0xFD vs pc > 0xFE: 0xFD is the correct bound for a 2-byte instruction (last start = 0xFD, bytes at 0xFD–0xFE). 0xFE would allow a fetch whose second byte is the screen cell — wrong. Use 0xFD.

What state does the CPU end in? Set halted = true (so run() stops) and return PC_OUT_OF_RANGE. The MachineController::run loop sees a non-OK status and stops; Menu reports "PC ran past end of memory at 0xFE." This is a runtime failure, symmetric to INVALID_INSTRUCTION during execution — both mean "the program is broken, stop and tell the user." It does not need to be detected at load time, because whether the PC overflows depends on control flow (a JUMP could send it anywhere), which only execution knows.

So: CPU detects it, run() propagates it, Menu reports it. And add it to RunStatus (§1) and StepStatus (§4) as PC_OUT_OF_RANGE.

A related edge the spec also doesn't cover: JUMP (opcode B) to an odd address. B RXY copies XY into the PC. If XY is odd, the next fetch is misaligned (the two-byte instruction straddles two logical cells). The spec doesn't forbid odd jump targets. Two options: (a) allow it — the fetch just reads two consecutive bytes starting at an odd address, which is how real machines with byte-addressable memory behave; or (b) reject it. I'd allow it (it's not wrong for a byte-addressable machine, and rejecting it would need a per-JUMP check the spec doesn't ask for) — but note that the "PC out of range" check is the only PC validity check, and it's about bounds, not alignment. Worth a comment in execute's JUMP branch, not new code.

Naming unification — you caught real inconsistency You're right; I was sloppy across rounds. Pick one name per concept:
Concept Name I used in different rounds Pick Façade class VoleMachine (round 1), MachineController (rounds 2–4) MachineController — it describes the role (controller/façade) and won't clash with the project name "Vole Machine." VoleMachine reads like "the whole machine," which is ambiguous with CPU+Memory+Screen. Per-step result StepResult (round 1), StepStatus (rounds 2–4) StepStatus — it's an enum of statuses, not a result object with data. StepResult implies a struct. Per-run result RunResult (round 1), RunStatus (rounds 2–4) RunStatus — same reasoning. Register file Registers (round 1), RegisterFile (rounds 2–4) RegisterFile — more precise. And with §1's collapse, the two status enums become:

cpp enum class StepStatus { OK, // executed one instruction, PC updated, not halted HALTED, // executed HALT (C 000) INVALID_INSTRUCTION, // fetched something that isn't a valid instruction PC_OUT_OF_RANGE // PC can't hold a full 2-byte instruction (pc > 0xFD) };

enum class RunStatus { HALTED, // run stopped at HALT INVALID_INSTRUCTION, // run stopped at an invalid instruction PC_OUT_OF_RANGE // run stopped because PC ran off the end }; Note StepStatus has OK (one step succeeded, keep going) but RunStatus doesn't — a run never ends "OK"; it ends at one of the three terminal conditions. That's the only difference between the two enums, and it's meaningful: step() is a single transition (which may or may not be terminal), run() is a loop that always ends terminally. If you want to reduce to one enum, you could drop OK and have step() return std::optional (nullopt = "still running, not terminal") — but that's a worse API than a plain enum for a teaching project. Two enums, one extra value (OK), is clearer. Keep both.

One more naming nit: in round 1 I wrote Registers::setregister() (lowercase r) — that was a typo; the final design uses RegisterFile::set. Also Instruction::getOpcode() vs the field opcode — fine, the getter is the public contract and the field is private.

What I'd add to the spec-ambiguity list for the grader Since §2 and §3 resolve gaps the spec leaves open, worth writing these down so your grader sees you made deliberate, defensible choices:

reset() → PC = 0x00, screen cleared, memory/registers cleared. (Spec silent on reset; power-on convention.)

loadProgram sets PC = start on success; leaves PC untouched on load failure. (Spec requires "set PC to start" — this is where it happens.)

PC out of range = pc > 0xFD (can't hold a 2-byte instruction; 0xFE/0xFF would make the low byte the screen cell). Detected in CPU::step, reported via PC_OUT_OF_RANGE.

JUMP to odd address is allowed (byte-addressable fetch), only bounds-checked, not alignment-checked. (Spec silent.)

Ready to consolidate At this point all four rounds of decisions are settled:

MachineController owns Memory, Screen, CPU, ProgramLoader; owns the run-all loop; sets PC on load; resets uniformly to 0x00 with screen cleared.

Memory is the single chokepoint for FF (Fix 1): write(0xFF, v) updates cells[0xFF] and appends to Screen's history buffer. No CPU --> Screen.

Screen is a history log (std::vector<uint8_t>), default DECIMAL, HEX optional, ASCII dropped.

CPU detects INVALID_INSTRUCTION and PC_OUT_OF_RANGE; STORE is uniform memory->write(xy, v).

Enums: StepStatus { OK, HALTED, INVALID_INSTRUCTION, PC_OUT_OF_RANGE }, RunStatus { HALTED, INVALID_INSTRUCTION, PC_OUT_OF_RANGE }.

Naming: MachineController, RegisterFile, StepStatus, RunStatus — consistently.

Say the word and I'll produce the single consolidated PlantUML reflecting all of this, with multiplicities and the FF-mapping note, ready to paste into plantuml.com. Or if you want to keep poking, the next thing I'd stress-test is whether ProgramLoader should return std::vector or std::vector<uint16_t> (raw words) — the former validates at load time (good for feature #10), the latter defers validation to execution (simpler loader, but a malformed file wouldn't be rejected until run). I'd keep Instruction (validate at load), but it's worth a sentence in your writeup.

A few follow-ups:

You set the PC limit at pc > 0xFD, but the assignment says the program start address must be even and less than FF, so FE is a valid start. Since cells[0xFF] now stores a normal byte (Fix 1), why is an instruction at FE/FF invalid? Please reconsider the bound.

In section 3 you said reading FF would return the screen, but earlier you decided cells[0xFF] stores the written value. Which is it? Please state clearly what Memory::read(0xFF) returns.

Why do we need two enums, StepStatus and RunStatus? Could run() simply return the StepStatus that stopped the loop?

Who checks that the program fits in memory (start + 2*numberOfInstructions <= 256)? Is it ProgramLoader or MachineController?

Now please give me ONE consolidated, final PlantUML code with all the decisions Five follow-ups — including a genuine contradiction I introduced You caught a real inconsistency in §1 and §2, and you're right to force both issues. Let me resolve them cleanly, then give the consolidated diagram.

The PC bound — you're right, 0xFE is a valid start, so my > 0xFD was wrong My pc > 0xFD bound assumed "the two bytes at pc and pc+1 must both be ordinary memory, and 0xFF is the screen so it can't hold the low byte." That was correct only under the old Semantics B (FF is a write-only port and reading FF doesn't give you a real byte). But we settled on Fix 1: cells[0xFF] holds a normal byte (the dual-write keeps it in sync), and Memory::read(0xFF) returns that byte. So under the current design, 0xFF is ordinary readable memory. There is no reason an instruction can't occupy 0xFE/0xFF.

Re-derive the bound from the current semantics:

An instruction occupies bytes at pc and pc+1.

Both must be valid addresses: pc + 1 <= 0xFF, i.e. pc <= 0xFE.

The assignment also says the start address must be even and < 0xFF — so valid starts are 0x00, 0x02, …, 0xFE. Consistent with pc <= 0xFE.

The last valid instruction start is therefore 0xFE (occupying 0xFE and 0xFF), not 0xFD.

So the check is:

cpp if (pc > 0xFE) { halted = true; return StepStatus::PC_OUT_OF_RANGE; } Equivalently, before fetch: "if pc + 1 > 0xFF, out of range." (Use pc > 0xFE to avoid uint8_t overflow in pc + 1.)

Two things this changes vs. my previous answer:

The valid PC range is 0x00–0xFE (even/odd both allowed for jump targets, as I noted; only the start address is required even by the spec — the CPU shouldn't enforce evenness on a JUMP, only on the load start).

0xFF can be a valid low byte of the last instruction, because under Fix 1 it's ordinary readable memory. Good.

I need to state clearly: this bound depends on Fix 1. If you ever revert to "FF is a write-only port" (Semantics B), then cells[0xFF] would be 0x00-forever or undefined, an instruction reading it as its low byte would be wrong, and 0xFE would not be a valid start. Under the design we actually chose (Fix 1, cells[0xFF] holds the byte), 0xFE is valid. The bound and the FF semantics must move together; I'll write that down as a design note so it doesn't drift again.

Final bound: pc > 0xFE → PC_OUT_OF_RANGE.

What does Memory::read(0xFF) return? — one unambiguous answer I contradicted myself: in §3 of the last round I wrote "if pc == 0xFF, the high byte itself is the screen cell" — which described the old port semantics (reading the screen). But we chose Fix 1, where cells[0xFF] holds a byte. Here is the single, final answer:
Memory::read(0xFF) returns cells[0xFF] — the last value written to address FF by any writer (CPU STORE or menu overwrite). It does not read the Screen history buffer.

Rationale, stated once:

Under Fix 1, Memory::write(0xFF, v) does cells[0xFF] = v; screen->write(v); — so cells[0xFF] always reflects the most recent write to FF.

Screen holds the history of all values written; cells[0xFF] holds only the latest. They are deliberately different: cells[0xFF] is "what's at the cell now" (so the memory matrix and any LOAD R, FF are well-defined), Screen is "everything ever printed."

Memory::read never touches Screen. Screen is write-only from the machine's perspective (no instruction reads it). The spec never defines a read-from-screen; only STORE writes. So there is no "read the screen" behavior to implement.

Consequence for the diagram and writeup: Memory --> Screen is a one-way write path (Memory calls Screen::write; Screen calls nothing on Memory). State that explicitly. And LOAD R, FF returns the last byte written to FF — which is a sensible, well-defined behavior, and matches "FF is an ordinary readable cell that happens to also forward to the screen on write."

So, to be explicit about the earlier contradiction: the "reading FF returns the screen" line was a leftover from the pre-Fix-1 port semantics and is wrong under the final design. read(0xFF) returns cells[0xFF], not the screen buffer.

Can run() return the StepStatus that stopped it? — almost, but not quite You're right to push. Let's look at the value sets:
cpp enum class StepStatus { OK, HALTED, INVALID_INSTRUCTION, PC_OUT_OF_RANGE }; enum class RunStatus { HALTED, INVALID_INSTRUCTION, PC_OUT_OF_RANGE }; RunStatus is literally StepStatus minus OK. Since run() stops only on a non-OK step, run() can return StepStatus and simply never return OK — the OK case is unreachable by construction. So RunStatus is redundant if we accept that run()'s contract is "returns a StepStatus that is guaranteed non-OK."

Two clean ways to express that:

(a) One enum, document the postcondition. StepStatus run(); with the doc comment "returns the non-OK status that terminated the run; never returns OK." Simple, one enum, no new type. The cost is that the signature doesn't enforce "non-OK" — the caller has to trust the doc.

(b) One enum, encode the distinction in the type. Have step() return std::optional (nullopt = "still running"), and run() return StepStatus (always terminal). Then StepStatus has no OK at all — it's purely the set of terminal outcomes — and "still running" is nullopt. This is the cleanest typing, but it changes step()'s call site (the step-by-step menu must handle nullopt = "ran one instruction, keep going").

(c) Keep two enums as before. Most explicit, slightly redundant, no optional machinery in step()'s hot path.

For an intermediate C++ project, I'd pick (a): one enum, StepStatus, used by both step() and run(); delete RunStatus. Reasons:

Removes a redundant type (your point).

step() returning OK for "keep going" is natural and needs no optional.

run() returning the terminating StepStatus needs no separate enum, and the "never OK" postcondition is a one-line doc comment.

optional (option b) is fine but adds a small API shift for step() for no real benefit at this scale — and OK is a clearer "one step succeeded, still running" signal than nullopt to a reader of the step-by-step menu.

So: one enum StepStatus { OK, HALTED, INVALID_INSTRUCTION, PC_OUT_OF_RANGE }; both CPU::step() and MachineController::run()/CPU-driven loop return it; run() documents that OK is never returned. The MachineController::run() loop:

cpp StepStatus MachineController::run() { StepStatus s; do { s = cpu.step(); } while (s == StepStatus::OK); return s; // never OK } This also removes the previous RunStatus mentions from the façade and menu. Delete RunStatus entirely.

Who checks the program fits in memory? Neither ProgramLoader nor MachineController alone — and the split matters:
ProgramLoader owns the file-to-Instruction parsing and per-line validity (hex well-formedness, opcode validity). It does not know the full memory layout policy in a way that should reject on fit — its job is "is each line a valid instruction," not "does the whole program fit at this start address." Keeping it fit-agnostic means you can reuse it to load into any target (a future assembler, a test that loads to a scratch buffer) without dragging memory-size rules along.

MachineController owns memory and the load operation, so it is the right place to check start + 2 * instructions.size() <= 0x100 — the same place it checks the start address is even and < 0xFF, and the same place it writes the bytes. This keeps "does this program fit where the user asked to put it" next to "write it there."

So the division:

Check Owner Start address is even MachineController::loadProgram (it's a placement rule, not a parsing rule) Start address < 0xFF MachineController::loadProgram Each line is a valid hex instruction ProgramLoader start + 2*N <= 0x100 (fits in memory) MachineController::loadProgram Nothing written on any failure MachineController::loadProgram (validate everything, then commit) ProgramLoader::loadFromFile returns LoadResult { std::vector instructions; std::string error; bool ok; } with no fit logic. MachineController::loadProgram does:

cpp bool MachineController::loadProgram(const std::string& path, uint8_t start) { if (start % 2 != 0 || start >= 0xFF) return false; // placement rule LoadResult r = loader.loadFromFile(path, start); if (!r.ok) return false; // parse/validity error if (start + 2u * r.instructions.size() > 0x100u) return false; // fit check // commit: write bytes, set PC, clear halt ... } Note the loader can drop its own isValidStartAddress helper — start-address validation is a placement concern and now lives entirely in loadProgram. (It can keep it as a convenience, but the controller must re-check anyway, so I'd remove it from the loader to avoid two sources of truth for the same rule.)

This is the same principle as the FF-rule move: rules with a single owner live with that owner; the loader stays a pure parser; the controller owns placement and memory-layout policy.

One consolidated final PlantUML All decisions folded in: MachineController (name), RegisterFile, single StepStatus (no RunStatus), Screen as history log with DECIMAL default and no ASCII, Memory as the FF chokepoint with one-way Memory --> Screen, CPU with no Screen dependency, PC bound > 0xFE, fit-check in loadProgram, loader as pure parser, MachineController::run() returning StepStatus (never OK).
plantuml @startuml VoleMachineSimulator

skinparam classAttributeIconSize 0 hide empty members

' ==================== Value types ====================

enum ScreenMode { DECIMAL HEX }

enum StepStatus { OK HALTED INVALID_INSTRUCTION PC_OUT_OF_RANGE }

class LoadResult <> {

instructions : std::vector
ok : bool
error : std::string }
' ==================== Storage & output ====================

class Screen {

buffer : std::vector<uint8_t>
mode : ScreenMode
Screen()
write(v : uint8_t) : void
setMode(m : ScreenMode) : void
clear() : void
isEmpty() const : bool
display(out : std::ostream&) const : void }
class Memory {

cells : std::array<uint8_t, 256>
screen : Screen*
Memory(screen : Screen&)
read(address : uint8_t) : uint8_t
write(address : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
Memory --> Screen : "write to 0xFF\nappends to history"

note bottom of Memory Address 0xFF is memory-mapped to the Screen. write(0xFF, v): cells[0xFF] = v; screen->write(v) read(0xFF): returns cells[0xFF] (last value written) The Screen is write-only from the machine's view; read() never consults the Screen history buffer. end note

' ==================== CPU core ====================

class RegisterFile {

regs : std::array<uint8_t, 16>
get(index : uint8_t) const : uint8_t
set(index : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
class ALU <> {

{static} addInt(a : uint8_t, b : uint8_t) : uint8_t
{static} addFloat(a : uint8_t, b : uint8_t) : uint8_t
{static} bitOr(a : uint8_t, b : uint8_t) : uint8_t
{static} bitAnd(a : uint8_t, b : uint8_t) : uint8_t
{static} bitXor(a : uint8_t, b : uint8_t) : uint8_t
{static} rotateRight(v : uint8_t, n : uint8_t) : uint8_t }
class Instruction {

raw : uint16_t
opcode : uint8_t
r : uint8_t
s : uint8_t
t : uint8_t
xy : uint8_t
Instruction(raw : uint16_t)
{static} fromHexString(line : const std::string&) : std::optional
isValid() const : bool
getOpcode() const : uint8_t
getR() const : uint8_t
getS() const : uint8_t
getT() const : uint8_t
getXY() const : uint8_t
getRaw() const : uint16_t
toString() const : std::string }
class CPU {

pc : uint8_t
ir : uint16_t
halted : bool
registers : RegisterFile
memory : Memory*
CPU(memory : Memory&)
setPC(addr : uint8_t) : void
getPC() const : uint8_t
getIR() const : uint16_t
isHalted() const : bool
clearHalt() : void
step() : StepStatus
reset() : void
displayState(out : std::ostream&) const : void
fetch() : uint16_t
execute(instr : const Instruction&) : StepStatus }
CPU *-- RegisterFile CPU --> Memory CPU ..> Instruction : creates per fetch CPU ..> ALU CPU ..> StepStatus

note right of CPU step(): if pc > 0xFE -> PC_OUT_OF_RANGE (last valid start is 0xFE, bytes 0xFE/0xFF) fetch -> decode -> execute -> update PC STORE is uniform: memory->write(xy, v) (FF handling lives in Memory::write) end note

' ==================== Loading ====================

class ProgramLoader {

loadFromFile(path : const std::string&) : LoadResult
parseLine(line : const std::string&) : std::optional }
ProgramLoader ..> Instruction ProgramLoader ..> LoadResult

note right of ProgramLoader Pure parser. Validates hex lines and instruction validity. Performs NO placement / fit checks (those are MachineController's job). end note

' ==================== Façade ====================

class MachineController {

memory : Memory
screen : Screen
cpu : CPU
loader : ProgramLoader
MachineController()
loadProgram(path : const std::string&, start : uint8_t) : bool
overwriteRegister(index : uint8_t, value : uint8_t) : void
overwriteMemory(address : uint8_t, value : uint8_t) : void
setPC(address : uint8_t) : void
run() : StepStatus
step() : StepStatus
reset() : void
displayAll(out : std::ostream&) const : void
isHalted() const : bool }
note bottom of MachineController loadProgram(): start even and < 0xFF; start + 2*N <= 0x100; commit bytes, then setPC(start), clearHalt. run(): loops cpu.step() until status != OK; returns the terminal StepStatus (never OK). reset(): memory.reset(); screen.clear(); cpu.reset() -> PC = 0x00. end note

MachineController *-- Memory MachineController *-- Screen MachineController *-- CPU MachineController *-- ProgramLoader MachineController ..> StepStatus

' ==================== UI ====================

class Menu {

controller : MachineController&
Menu(controller : MachineController&)
run() : void
showMainMenu() const : void
handleLoad() : void
handleEditRegister() : void
handleEditMemory() : void
handleSetPC() : void
handleRun() : void
handleStep() : void
handleDisplay() : void }
Menu --> MachineController : uses

' ==================== Multiplicities ====================

MachineController "1" *-- "1" Memory MachineController "1" *-- "1" Screen MachineController "1" *-- "1" CPU MachineController "1" *-- "1" ProgramLoader CPU "1" *-- "1" RegisterFile CPU "1" --> "1" Memory : reads/writes Memory "1" --> "1" Screen : "forwards writes to 0xFF" Menu "1" --> "1" MachineController : uses

@enduml Notes on what changed in the final diagram RunStatus is gone. CPU::step(), MachineController::step(), and MachineController::run() all return StepStatus; run() documents that it never returns OK. One enum.

Memory --> Screen is one-way ("forwards writes to 0xFF"), with the note stating read(0xFF) = cells[0xFF] (last value written), not the screen buffer. No CPU --> Screen.

PC bound is > 0xFE in the CPU note, with the reason (last valid start 0xFE, bytes 0xFE/0xFF; valid because Fix 1 makes 0xFF ordinary readable memory).

ProgramLoader is a pure parser (loadFromFile(path), no start address); placement/fit checks live in MachineController::loadProgram (start even, < 0xFF, start + 2*N <= 0x100, commit-then-set-PC).

reset() → memory.reset(); screen.clear(); cpu.reset() → PC = 0x00.

Screen is a history log (std::vector<uint8_t>), DECIMAL default, HEX optional, no ASCII.

No duplicated relationships: each composition/association appears once, with multiplicity labels.

This is the consolidated design. If you want, next round we can write the one-page design-notes section for your report that records the four spec ambiguities we resolved (read(0xFF), PC bound 0xFE, reset-to-0x00, fit-check ownership) — that's usually what graders reward most after the diagram itself.

I found some issues in the consolidated design:

You said "no duplicated relationships", but the final PlantUML still declares the same relationships twice (the "Multiplicities" section repeats MachineController *-- Memory/Screen/CPU/ProgramLoader, CPU *-- RegisterFile, CPU --> Memory, Memory --> Screen, Menu --> MachineController). Please put the multiplicities directly on the original relationships and remove the duplicates.

pc is uint8_t. If pc = 0xFE and the instruction completes, pc + 2 wraps to 0x00, so the check "pc > 0xFE" only catches pc == 0xFF and a program without HALT would loop forever instead of returning PC_OUT_OF_RANGE. How should the PC update be designed to avoid this wrap-around?

loadProgram writes the program bytes using Memory::write. With Fix 1, a program loaded at 0xFE would write its second byte to 0xFF and forward it to the Screen history before the program even runs. How do you separate "loading bytes" from "STORE/overwrite to FF"?

loadProgram returns bool, so the menu cannot tell the user WHY loading failed (odd start, doesn't fit, invalid instruction at line N). What should it return instead?

Where is the 8-bit floating-point format (1 sign, 3 exponent, 4 mantissa) for opcode 6 implemented? Should it be a separate class (e.g., Float8) used by ALU::addFloat?

Why a switch in CPU::execute and not an Instruction class hierarchy (one subclass per opcode)? What are the trade-offs for an intermediate project? Also, which exact checks does Instruction::isValid perform, and why do both ProgramLoader::parseLine and Instruction::fromHexString exist?

Should run() have protection against infinite loops (e.g., a program that jumps to itself with B000)?

Please then give me ONE final PlantUML with all of these fixed. Seven issues — all valid; the PC wrap-around and the load/FF interaction are the two real bugs You've found two genuine correctness bugs (2 and 3) and five design issues (1, 4, 5, 6, 7). Let me resolve each precisely, then give the final consolidated diagram.

Duplicated relationships in the PlantUML Correct — I declared them once in the body and again in a "Multiplicities" block. PlantUML renders both. Fix: declare each relationship exactly once, with the multiplicity label on that single declaration, and delete the trailing block.

PC wrap-around — the real bug With pc : uint8_t, pc = 0xFE, executing a 2-byte instruction does pc += 2 → 0x00 (wraps). The next fetch reads 0x00 and the program continues from the start. A program with no HALT loops forever instead of reporting PC_OUT_OF_RANGE. The check pc > 0xFE at the top of step() only catches pc == 0xFF (which can't legally be a start), not the wrap.

There are two clean fixes; they differ in when you detect the overflow.

Fix A — detect before the wrap (recommended). Keep pc : uint8_t, but compute the next PC in a wider type and check the would-be next PC before committing it:

cpp // in execute(), after computing the next value (nextPC = pc + 2, or XY for JUMP): if (nextPC > 0xFE) { // nextPC is uint16_t here halted = true; return StepStatus::PC_OUT_OF_RANGE; } pc = static_cast<uint8_t>(nextPC); But this is awkward because execute would need to return a "PC to set" rather than mutating pc, so it can validate before writing. The cleanest form: execute returns the next PC as a uint16_t (or std::optional<uint8_t>), and step validates and commits it.

Fix B — widen pc to uint16_t internally. Then pc += 2 can reach 0x0100 without wrapping, and step()'s top check is simply if (pc > 0xFE) return PC_OUT_OF_RANGE;. setPC/getPC still take/return uint8_t (or clamp), but the internal counter is wide enough to represent "past the end." This is the simplest correct design: the PC register is conceptually wider than 8 bits during the "one past the end" moment, which is exactly how real CPUs detect overflow.

I recommend Fix B for an intermediate project: widen pc to uint16_t, keep the public setPC(uint8_t)/getPC() (getter returns uint16_t so callers can see 0x0100 = "out of range", or add isPCOutOfRange()). Then:

cpp StepStatus CPU::step() { if (halted) return StepStatus::HALTED; if (pc > 0xFE) { halted = true; return StepStatus::PC_OUT_OF_RANGE; } // fetch, decode, execute -> execute sets pc = next (may be 0x0100) } And execute for a normal 2-byte instruction does pc += 2 (no wrap now, since pc is uint16_t), JUMP does pc = instr.getXY(), HALT sets halted. The pc > 0xFE check catches both the "next instruction would start past the end" case and 0xFE/0xFF being the last instruction's bytes. Note: execute must not commit pc past 0x100 — pc += 2 from 0xFE gives 0x0100, which is fine in uint16_t and is caught next iteration. No wrap, no infinite loop.

So: pc : uint16_t; top-of-step check pc > 0xFE → PC_OUT_OF_RANGE; execute mutates pc freely (JUMP or +2). getPC() returns uint16_t; setPC takes uint8_t but guards addr > 0xFE (clamp or reject). Document the widening.

This also fixes the earlier 0xFD/0xFE confusion: with pc : uint16_t and the check pc > 0xFE, 0xFE is a valid start (bytes 0xFE, 0xFF — both readable under Fix 1), and 0x0100 is the sentinel "past end."

loadProgram writing to 0xFF would pollute the screen history — real bug Right. loadProgram uses Memory::write, and Memory::write(0xFF, v) forwards to Screen. So a program loaded at 0xFE writes its second byte to 0xFF and the screen logs a value before the program runs — nonsense output. More generally, loading is not storing; loading must never trigger the FF→screen side-effect.
Fix: separate raw memory access from mapped I/O writes. Two methods on Memory:

cpp class Memory { - cells : std::array<uint8_t, 256> - screen : Screen* public: uint8_t read(uint8_t addr) const; void writeRaw(uint8_t addr, uint8_t v); // plain store, NO side effects void write(uint8_t addr, uint8_t v); // mapped: FF -> cells + screen ... }; writeRaw is used only by loadProgram (and by reset). It never touches Screen.

write is used by CPU::execute (STORE) and by overwriteMemory (menu) — the two runtime writers that must drive the screen.

But wait — if overwriteMemory(0xFF, v) from the menu uses write, and write forwards to Screen, then manual overwrite of FF also logs to the screen history. Is that right? Yes — that's exactly what Fix 1 wanted: any runtime write to FF drives the screen. The menu's "overwrite memory" is a runtime write (the user is simulating a STORE-like effect), so it should log. Only program loading is exempt, because it's not a STORE — it's placing code into memory. So:

loadProgram → writeRaw (no screen).

CPU::execute STORE and overwriteMemory → write (drives screen for FF).

That cleanly separates "load bytes" from "store/overwrite to FF." Document it: writeRaw = placement; write = runtime store (mapped).

One edge: after loadProgram loads a program whose last byte is at 0xFF, cells[0xFF] holds that program byte. If the program later does STORE R, FF, write overwrites cells[0xFF] with R and logs it — fine, program code at 0xFF was already consumed as the low byte of the last instruction. No conflict, because instructions are fetched before they'd be overwritten by a store to FF (and if a store to FF overwrites the byte of a future instruction, that's the program's own bug — same as any self-modifying code; the spec doesn't forbid it).

loadProgram returns bool — no reason on failure You're right; the menu must report why. Replace bool with a richer result:
cpp enum class LoadStatus { OK, INVALID_START_ADDRESS, // odd or >= 0xFF FILE_NOT_FOUND, // couldn't open PARSE_ERROR, // a line isn't 4 hex digits / blank INVALID_INSTRUCTION, // opcode/operands invalid; line number in message DOES_NOT_FIT // start + 2*N > 0x100 };

struct LoadOutcome { LoadStatus status; std::string message; // human-readable, includes line number for PARSE/INVALID };

LoadOutcome MachineController::loadProgram(const std::string& path, uint8_t start); The menu does switch (outcome.status) and prints outcome.message. LoadStatus also makes tests assert on why loading failed, not just that it failed. Keep LoadResult (from ProgramLoader) separate — that's the parser's result (instructions + ok + error), and MachineController maps it into LoadOutcome after applying placement/fit checks.

The 8-bit floating-point format (1 sign, 3 exponent, 4 mantissa) Yes — a dedicated Float8 value class is the right call, used by ALU::addFloat. Reasons:
The format is a representation concern: encode/decode sign/exponent/mantissa, handle exponent bias, denormals (or not), rounding on add. Burying that inside ALU::addFloat would make one function do bit-packing and arithmetic.

It's independently testable: Float8(0x3A).toDouble() etc.

It keeps ALU a thin dispatcher: addFloat(a,b) { return Float8::from(a) + Float8::from(b); }.

Minimal Float8 API (intermediate level — pick explicit rules and document them; the spec doesn't define bias/denormals, so you must choose and record it):

cpp class Float8 { - bits : uint8_t public: explicit Float8(uint8_t bits); static Float8 fromBits(uint8_t bits); static Float8 fromDouble(double v); // for testing / construction double toDouble() const; uint8_t bits() const; static Float8 add(Float8 a, Float8 b); // or operator+ }; Document the chosen layout: bit 7 = sign, bits 6–4 = exponent (3 bits, bias 3 or 4 — pick and state it), bits 3–0 = mantissa (4 bits, implicit leading 1 or not — pick and state it). Since the spec doesn't specify, an "obvious" choice is sign-magnitude with bias 3, no denormals, no infinity/NaN (simplest for a toy); but whatever you pick, write it down in the design notes so the grader can follow. ALU::addFloat then delegates:

cpp uint8_t ALU::addFloat(uint8_t a, uint8_t b) { return Float8::add(Float8(a), Float8(b)).bits(); } Add Float8 to the diagram as a dependency of ALU (and optionally of tests).

Switch vs. opcode class hierarchy; isValid; parseLine vs fromHexString Switch vs. hierarchy — for an intermediate project, keep the switch. Trade-offs:
Hierarchy (one subclass per opcode, e.g. LoadInstruction, AddInstruction, …): classic "replace conditional with polymorphism." Pros: adding an opcode doesn't touch CPU::execute; each opcode's execute logic is isolated and testable; open/closed principle. Cons: a lot of boilerplate (16 subclasses + a factory + a dispatch map) for 13 opcodes in a toy machine; each subclass needs access to Memory, RegisterFile, PC, halted — so you either pass a big ExecutionContext struct or give each subclass a back-reference to CPU, which couples them tightly anyway; and Instruction becomes a hierarchy and a value type (parsing, getRaw) at once, which is two responsibilities. At this scale the indirection cost outweighs the extensibility benefit — you're not going to add opcodes, and the spec fixes them.

Switch: one execute method, ~13 short cases, each 1–5 lines. Easy to read top-to-bottom, easy to debug, no factory, no dispatch table, no context-passing. The "closed for modification" cost is real but irrelevant when the opcode set is fixed by the spec.

Recommendation: switch. State in the design notes: "A per-opcode class hierarchy was considered and rejected for an intermediate project: 13 fixed opcodes, heavy context-passing, and a value-type Instruction that would be split awkwardly. The switch is centralized in CPU::execute, which is the single place opcode semantics live — so it's still cohesive, just not polymorphic." If the assignment later says "add opcodes dynamically," revisit.

Instruction::isValid — exact checks:

opcode is in 1..C (0 and D–F invalid).

For opcode 1, 2, 3, B: operand R and XY are any hex digits — no constraint beyond being hex (already guaranteed by parsing). (Special case: opcode 3 with XY == FF is valid — it's the screen store.)

For opcode 4: the byte is 40RS — first nibble after opcode must be 0 (the 0RS form). Reject 4xRS with x != 0.

For opcodes 5, 6, 7, 8, 9: RST — any hex digits, no further constraint.

For opcode A: R0X — second nibble must be 0. Reject ARxX with x != 0.

For opcode B: RXY — any hex digits.

For opcode C: the whole operand must be 000. Reject C with nonzero operand.

So isValid encodes the structural form of each opcode (the 0 in 40RS/R0X, the 000 in C000), and the range check 1..C. It does not check memory bounds, register bounds (all nibbles are 0–F by construction), or program fit — those are elsewhere.

Why both ProgramLoader::parseLine and Instruction::fromHexString? They should be one, and I had redundant layering. Fix: Instruction::fromHexString is the single text→Instruction converter (it strips the comment after the first token, trims, requires exactly 4 hex digits, constructs the uint16_t, constructs Instruction, calls isValid). ProgramLoader::parseLine disappears; loadFromFile calls Instruction::fromHexString per line and, on nullopt, records the line number in its error. That removes the duplication and keeps "what is a valid instruction line" in one place (the Instruction class), while ProgramLoader only handles file iteration, blank/; lines, and line-numbered error messages. So: drop parseLine; keep fromHexString; loadFromFile reports the failing line number.

Infinite-loop protection in run() Yes — a self-jump (B000) or any loop without HALT would make run() spin forever with no output. Add a step budget:
cpp StepStatus MachineController::run(unsigned maxSteps = 1'000'000) { for (unsigned i = 0; i < maxSteps; ++i) { StepStatus s = cpu.step(); if (s != StepStatus::OK) return s; } return StepStatus::STEP_LIMIT_REACHED; // new enum value } Add STEP_LIMIT_REACHED to StepStatus. The menu reports "program exceeded N steps — possible infinite loop." This is a safety net, not a correctness feature: a legitimate program that needs >1M steps would be misreported, so make the limit a parameter (default generous) and/or configurable from the menu. For step-by-step mode, the user is the loop, so no budget needed there — but you could also cap total steps across a step-by-step session if you want; I'd keep it simple and only guard run().

Document: run() has a step budget (default 1,000,000) to prevent infinite loops; exceeding it returns STEP_LIMIT_REACHED.

Final consolidated PlantUML All fixes applied: single relationship declarations with multiplicities inline; pc : uint16_t; writeRaw vs write; LoadOutcome/LoadStatus; Float8; switch retained (noted); parseLine removed; STEP_LIMIT_REACHED.

plantuml @startuml VoleMachineSimulator

skinparam classAttributeIconSize 0 hide empty members

' ==================== Enums / results ====================

enum ScreenMode { DECIMAL HEX }

enum StepStatus { OK HALTED INVALID_INSTRUCTION PC_OUT_OF_RANGE STEP_LIMIT_REACHED }

enum LoadStatus { OK INVALID_START_ADDRESS FILE_NOT_FOUND PARSE_ERROR INVALID_INSTRUCTION DOES_NOT_FIT }

class LoadOutcome <> {

status : LoadStatus
message : std::string }
class LoadResult <> {

instructions : std::vector
ok : bool
error : std::string }
' ==================== Storage & output ====================

class Screen {

buffer : std::vector<uint8_t>
mode : ScreenMode
Screen()
write(v : uint8_t) : void
setMode(m : ScreenMode) : void
clear() : void
isEmpty() const : bool
display(out : std::ostream&) const : void }
class Memory {

cells : std::array<uint8_t, 256>
screen : Screen*
Memory(screen : Screen&)
read(address : uint8_t) const : uint8_t
writeRaw(address : uint8_t, value : uint8_t) : void
write(address : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
Memory "1" --> "1" Screen : "write(0xFF, v)\nforwards to history"

note bottom of Memory Address 0xFF is memory-mapped to the Screen. writeRaw(addr, v): cells[addr] = v; NO side effects. Used ONLY by loadProgram / reset. write(addr, v): cells[addr] = v; if addr == 0xFF -> screen->write(v). Used by CPU STORE and overwriteMemory. read(0xFF): returns cells[0xFF] (last value written). The Screen is write-only from the machine's view. end note

' ==================== CPU core ====================

class RegisterFile {

regs : std::array<uint8_t, 16>
get(index : uint8_t) const : uint8_t
set(index : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
class Float8 {

bits : uint8_t
Float8(bits : uint8_t)
{static} fromBits(bits : uint8_t) : Float8
{static} fromDouble(v : double) : Float8
{static} add(a : Float8, b : Float8) : Float8
toDouble() const : double
bits() const : uint8_t }
note bottom of Float8 8-bit float: 1 sign | 3 exponent | 4 mantissa. Exact layout (bias, implicit leading 1, denormals) is a spec gap — choose and document it. end note

class ALU <> {

{static} addInt(a : uint8_t, b : uint8_t) : uint8_t
{static} addFloat(a : uint8_t, b : uint8_t) : uint8_t
{static} bitOr(a : uint8_t, b : uint8_t) : uint8_t
{static} bitAnd(a : uint8_t, b : uint8_t) : uint8_t
{static} bitXor(a : uint8_t, b : uint8_t) : uint8_t
{static} rotateRight(v : uint8_t, n : uint8_t) : uint8_t }
ALU ..> Float8 : addFloat delegates

class Instruction {

raw : uint16_t
opcode : uint8_t
r : uint8_t
s : uint8_t
t : uint8_t
xy : uint8_t
Instruction(raw : uint16_t)
{static} fromHexString(line : const std::string&) : std::optional
isValid() const : bool
getOpcode() const : uint8_t
getR() const : uint8_t
getS() const : uint8_t
getT() const : uint8_t
getXY() const : uint8_t
getRaw() const : uint16_t
toString() const : std::string }
note right of Instruction fromHexString: single text->Instruction converter. Strips comment after first token, requires exactly 4 hex digits, constructs Instruction, runs isValid. isValid checks: opcode in 1..C opcode 4: second nibble == 0 (40RS) opcode A: second nibble == 0 (R0X) opcode C: operand == 000 end note

class CPU {

pc : uint16_t
ir : uint16_t
halted : bool
registers : RegisterFile
memory : Memory*
CPU(memory : Memory&)
setPC(addr : uint8_t) : void
getPC() const : uint16_t
getIR() const : uint16_t
isHalted() const : bool
clearHalt() : void
step() : StepStatus
reset() : void
displayState(out : std::ostream&) const : void
fetch() : uint16_t
execute(instr : const Instruction&) : StepStatus }
CPU "1" *-- "1" RegisterFile CPU "1" --> "1" Memory : "read / write" CPU ..> Instruction : "creates per fetch" CPU ..> ALU CPU ..> StepStatus

note right of CPU pc is uint16_t so pc+2 can reach 0x0100 without wrapping. step(): if pc > 0xFE -> PC_OUT_OF_RANGE. Last valid start is 0xFE (bytes 0xFE/0xFF; 0xFF is readable under Fix 1). STORE is uniform memory->write(xy, v); FF side effect lives in Memory::write. Opcode dispatch is a switch in execute() (see design notes) — not a class hierarchy. end note

' ==================== Loading ====================

class ProgramLoader {

loadFromFile(path : const std::string&) : LoadResult }
ProgramLoader ..> Instruction ProgramLoader ..> LoadResult

note right of ProgramLoader Pure parser: iterates lines, skips blanks and lines starting with ';', calls Instruction::fromHexString per line, records the failing line number in error. NO placement / fit checks. end note

' ==================== Façade ====================

class MachineController {

memory : Memory
screen : Screen
cpu : CPU
loader : ProgramLoader
MachineController()
loadProgram(path : const std::string&, start : uint8_t) : LoadOutcome
overwriteRegister(index : uint8_t, value : uint8_t) : void
overwriteMemory(address : uint8_t, value : uint8_t) : void
setPC(address : uint8_t) : void
run(maxSteps : unsigned = 1000000) : StepStatus
step() : StepStatus
reset() : void
displayAll(out : std::ostream&) const : void
isHalted() const : bool }
MachineController "1" *-- "1" Memory MachineController "1" *-- "1" Screen MachineController "1" *-- "1" CPU MachineController "1" *-- "1" ProgramLoader MachineController ..> StepStatus MachineController ..> LoadOutcome MachineController ..> LoadStatus

note bottom of MachineController loadProgram: start even and < 0xFF; start + 2*N <= 0x100; commit bytes via memory.writeRaw (NO screen); then setPC(start), clearHalt. Returns LoadOutcome{status, message}. overwriteMemory: memory.write (FF drives screen). run(): loops cpu.step() until status != OK; returns the terminal StepStatus (never OK); step budget -> STEP_LIMIT_REACHED. reset(): memory.reset(); screen.clear(); cpu.reset() -> PC = 0x00. end note

' ==================== UI ====================

class Menu {

controller : MachineController&
Menu(controller : MachineController&)
run() : void
showMainMenu() const : void
handleLoad() : void
handleEditRegister() : void
handleEditMemory() : void
handleSetPC() : void
handleRun() : void
handleStep() : void
handleDisplay() : void }
Menu "1" --> "1" MachineController : uses

@enduml What each fix changed, in one line each Duplicates removed: every relationship is declared once, with its multiplicity inline; no trailing block.

PC wrap: pc : uint16_t; pc > 0xFE → PC_OUT_OF_RANGE; execute may set pc = 0x0100; no wrap, no infinite loop.

Load vs STORE: Memory::writeRaw (placement, no screen) vs Memory::write (runtime, FF drives screen). loadProgram uses writeRaw; CPU STORE and overwriteMemory use write.

Load feedback: loadProgram returns LoadOutcome { LoadStatus, message }, not bool.

Float8: dedicated value class; ALU::addFloat delegates.

Switch retained (documented in the CPU note); isValid checks spelled out in the Instruction note; parseLine removed — fromHexString is the single text converter.

Infinite loops: run(maxSteps = 1'000'000) returns STEP_LIMIT_REACHED.

Two things I'd flag for your writeup, because they're chosen rather than given: (a) the exact Float8 layout (bias, implicit leading 1, denormal policy) — the spec doesn't say, so state your choice explicitly; (b) the step-limit default (1M) — state it's a safety net, configurable, and why 1M. If you want, next round I can draft the short "Design Notes / Resolved Ambiguities" section listing all the spec gaps we closed (read(0xFF), PC bound 0xFE, reset to 0x00, fit-check ownership, writeRaw vs write, Float8 layout, step limit) — that's usually the section graders reward most alongside the diagram.

This is almost final. Last round:

LoadResult and LoadOutcome look redundant. Why not have ProgramLoader return LoadOutcome directly (with the instructions in it)? Please simplify or justify.

setPC(uint8_t) with pc as uint16_t: you said "clamp or reject". Please decide: what does MachineController::setPC(0xFF) do, and who validates it?

For Float8, the textbook (Brookshear, Computer Science: An Overview) uses 1 sign bit, 3 exponent bits in excess-4 notation (bias 4), and a 4-bit mantissa in normalized form (.1xxx, no implicit leading 1). Please adopt that layout and document it in the Float8 note, including how addFloat handles overflow/underflow and zero.

overwriteMemory(0xFF, v) from the menu logs to the Screen history. Is that desirable? Justify, or use writeRaw for manual overwrite.

Then please give the FINAL PlantUML with these changes, and a short "Design Notes / Resolved Ambiguities" section listing all assumptions we made. Four final decisions — then the final diagram and design notes All four are good closing questions. Two are simplifications (1, 2), one is a spec-conformance decision (3), and one is a semantic judgment call (4). Let me resolve each, then give the final PlantUML and the design notes.

LoadResult vs LoadOutcome — collapse to one You're right; two structs with overlapping fields is redundancy. They exist because of a layering I introduced (parser result vs. controller result), but the information is the same: "did it load, what instructions, if not why." ProgramLoader can return the richer type directly, and MachineController augments the status/message for placement/fit failures without a second type.
Merge into one LoadOutcome carrying both the instructions and the status:

cpp enum class LoadStatus { OK, INVALID_START_ADDRESS, // odd or >= 0xFF (set by MachineController) FILE_NOT_FOUND, // set by ProgramLoader PARSE_ERROR, // set by ProgramLoader (bad hex line; message has line no.) INVALID_INSTRUCTION, // set by ProgramLoader (isValid failed; message has line no.) DOES_NOT_FIT // set by MachineController };

struct LoadOutcome { LoadStatus status = LoadStatus::OK; std::string message; // human-readable; includes line no. when relevant std::vector instructions; // empty unless status == OK }; ProgramLoader::loadFromFile(path) returns LoadOutcome with instructions filled and status = OK / FILE_NOT_FOUND / PARSE_ERROR / INVALID_INSTRUCTION. It does not touch placement rules, so it can't return INVALID_START_ADDRESS or DOES_NOT_FIT — but the type is shared, so the controller just overwrites status/message on those failures.

MachineController::loadProgram(path, start) checks placement and fit first (cheap), then delegates to the loader, then checks fit after it knows instructions.size(). On any failure it returns the LoadOutcome with status/message set; on success it commits bytes via writeRaw, sets PC, and returns {OK, "", instructions}.

One subtlety: the controller must validate start address before calling the loader, otherwise a file-not-found error masks an odd start address. Order: check start → call loader → check fit → commit. Each failure returns the appropriate LoadStatus.

So: one LoadOutcome, returned by both ProgramLoader::loadFromFile and MachineController::loadProgram. Delete LoadResult entirely.

setPC(0xFF) — decide, don't hedge pc : uint16_t internally, but a valid instruction start is 0x00–0xFE. 0xFF is not a valid start (its two-byte instruction would straddle 0xFF and 0x100). So the decision:
MachineController::setPC(0xFF) is rejected. setPC accepts only 0x00–0xFE; out-of-range values are refused and the PC is left unchanged.

Where does validation live? In CPU::setPC (the owner of PC), returning a bool:

cpp bool CPU::setPC(uint16_t addr); // true if 0x00..0xFE, false otherwise (pc unchanged) MachineController::setPC forwards and returns the same bool. Menu::handleSetPC prints an error on false ("PC must be 0x00–0xFE") and does nothing. Do not clamp — silently turning 0xFF into 0xFE hides a user error. Reject and report.

Why setPC(uint16_t) rather than uint8_t now: since pc is uint16_t and valid values are 0x00–0xFE, accepting uint16_t lets the caller pass any value and get a clean rejection for 0xFF/0x100 instead of a wrap. The menu reads input as unsigned and passes it through; setPC is the single validator. (If you prefer uint8_t, then 0xFF is representable and setPC rejects it — same result; I'll use uint16_t for uniformity with pc and getPC().)

Also fix the load path: loadProgram already rejects odd or >= 0xFF starts as a placement rule, so it never calls setPC(0xFF) on success. Consistent.

Float8 — adopt Brookshear's layout (bias 4, .1xxx normalized, no implicit leading 1) Adopt exactly what you described:
Bit 7: sign (0 = positive, 1 = negative).

Bits 6–4: exponent, 3 bits, excess-4 (bias 4); stored value E represents exponent E − 4, so real exponents range −4 … +3.

Bits 3–0: 4-bit mantissa in normalized form .1xxx — i.e., the mantissa is a fraction in [1/2, 1) whose first fractional bit is always 1 (no implicit leading 1; the leading 1 is stored explicitly as the top mantissa bit). Value = ± 0.1mmm × 2^(E−4), where .1mmm is binary.

Zero representation: 0x00 = +0, 0x80 = −0 (sign-magnitude; both behave as 0.0 in arithmetic). Document that −0 == +0 for addFloat results (canonicalize to +0 on output if you want, or leave it — state your choice; I'd canonicalize to +0).

Float8::add behavior for overflow/underflow — the spec doesn't say, so choose and document:

Underflow (result magnitude below the smallest representable normalized value): return zero (+0 or −0 per sign). No denormals — Brookshear's format doesn't define them, and adding them is out of scope. Document: "underflow flushes to signed zero."

Overflow (result magnitude exceeds the largest representable value, i.e. exponent field would exceed 7): saturate to the largest finite value of the correct sign (0x7F = +max, 0xFF = −max). No infinity/NaN — again not in Brookshear's format. Document: "overflow saturates to the maximum finite magnitude."

Rounding: when the exact sum needs more than 4 mantissa bits, round to nearest, ties toward zero (or truncate — pick one and state it). Simplest defensible choice: truncate (drop extra bits), documented as "truncation, no rounding." If you want nearest-even, say so; for a teaching simulator, truncation is defensible and simpler.

Zero operands: x + 0 = x, 0 + 0 = +0. Handle explicitly so the sign of zero doesn't leak through.

ALU::addFloat(a, b) = Float8::add(Float8(a), Float8(b)).bits(). The layout, bias, normalization, overflow/underflow, and rounding rule all live in the Float8 note so the grader can see the choices.

overwriteMemory(0xFF, v) from the menu — should it log to the screen? This is a semantic judgment, and I'll commit: no, manual overwrite should use writeRaw and NOT log to the Screen history.
Justification:

The Screen history models program output — the sequence of values the running program stored to FF. It's the answer to "what did this program print?"

The menu's "overwrite memory" is a debugging/setup operation: the user is poking at machine state before or between runs, like a debugger setting a byte. It is not the program printing.

If manual overwrite logged to the history, then the "screen output after the program runs" (features #8, #9) would be contaminated by the user's setup writes — you'd see values the program never produced. That defeats the purpose of the screen as program output.

Also, overwriteMemory is used for any address, not just FF; making FF special in the menu introduces the same "two paths write FF differently" inconsistency we fixed earlier — except now it's intentional: runtime STORE writes (program) drive the screen; setup writes (user) don't.

So the rule is clean and easy to state:

Runtime writes to FF (CPU STORE) drive the Screen history; setup/debug writes (overwriteMemory, loadProgram) use writeRaw and do not.

But note the consequence: if the user does overwriteMemory(0xFF, 0x0C) manually, cells[0xFF] becomes 0x0C and the memory matrix shows it, but the screen history doesn't. Is that confusing? Slightly, but it's correct: the screen is the program's output, not the machine's cell dump. The memory matrix already shows cells[0xFF]; the user can see the value there. If the user wants to simulate a program store to FF, they run a program that does STORE R, FF — that's the semantic path.

One caveat to document: read(0xFF) returns cells[0xFF] regardless of whether the last write was runtime or setup. So a LOAD R, FF after a manual overwrite sees the manual value. That's fine — cells[0xFF] is "what's at the cell now," and the screen is "what the program printed." Two different questions, two different answers, both well-defined.

Implementation: MachineController::overwriteMemory → memory.writeRaw(addr, v). CPU::execute STORE → memory.write(xy, v). loadProgram → memory.writeRaw. Only the CPU's STORE reaches Screen.

FINAL PlantUML All four fixes: one LoadOutcome; setPC returns bool rejecting 0xFF+; Float8 with Brookshear layout and documented overflow/underflow; overwriteMemory uses writeRaw (no screen log).

plantuml @startuml VoleMachineSimulator

skinparam classAttributeIconSize 0 hide empty members

' ==================== Enums / results ====================

enum ScreenMode { DECIMAL HEX }

enum StepStatus { OK HALTED INVALID_INSTRUCTION PC_OUT_OF_RANGE STEP_LIMIT_REACHED }

enum LoadStatus { OK INVALID_START_ADDRESS FILE_NOT_FOUND PARSE_ERROR INVALID_INSTRUCTION DOES_NOT_FIT }

class LoadOutcome <> {

status : LoadStatus
message : std::string
instructions : std::vector }
' ==================== Storage & output ====================

class Screen {

buffer : std::vector<uint8_t>
mode : ScreenMode
Screen()
write(v : uint8_t) : void
setMode(m : ScreenMode) : void
clear() : void
isEmpty() const : bool
display(out : std::ostream&) const : void }
class Memory {

cells : std::array<uint8_t, 256>
screen : Screen*
Memory(screen : Screen&)
read(address : uint8_t) const : uint8_t
writeRaw(address : uint8_t, value : uint8_t) : void
write(address : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
Memory "1" --> "1" Screen : "write(0xFF, v) forwards\nto Screen history"

note bottom of Memory Address 0xFF is memory-mapped to the Screen. writeRaw(addr, v): cells[addr] = v; NO side effects. Used by loadProgram and overwriteMemory (setup / debug writes). write(addr, v): cells[addr] = v; if addr == 0xFF -> screen->write(v). Used ONLY by CPU STORE (runtime writes). read(0xFF): returns cells[0xFF] (last value written, any path). Screen is write-only from the machine's view. end note

' ==================== CPU core ====================

class RegisterFile {

regs : std::array<uint8_t, 16>
get(index : uint8_t) const : uint8_t
set(index : uint8_t, value : uint8_t) : void
reset() : void
display(out : std::ostream&) const : void }
class Float8 {

bits : uint8_t
Float8(bits : uint8_t)
{static} fromBits(bits : uint8_t) : Float8
{static} fromDouble(v : double) : Float8
{static} add(a : Float8, b : Float8) : Float8
toDouble() const : double
bits() const : uint8_t }
note bottom of Float8 8-bit float (Brookshear layout): bit 7 : sign (0 = +, 1 = -) bits 6..4 : exponent, excess-4 (bias 4); real exp = E - 4, range -4..+3 bits 3..0 : mantissa in normalized form .1xxx (leading 1 stored explicitly; no implicit 1) value = ± 0.1mmm(2) × 2^(E-4) Zero: 0x00 = +0, 0x80 = -0 (arithmetic treats both as 0). add(): overflow -> saturate to largest finite magnitude of the sign (0x7F = +max, 0xFF = -max). No inf/NaN. underflow -> flush to signed zero. No denormals. rounding -> truncate extra mantissa bits. zero rules: x + 0 = x; 0 + 0 = +0. These rules are a spec gap and are CHOICES — documented, not given. end note

class ALU <> {

{static} addInt(a : uint8_t, b : uint8_t) : uint8_t
{static} addFloat(a : uint8_t, b : uint8_t) : uint8_t
{static} bitOr(a : uint8_t, b : uint8_t) : uint8_t
{static} bitAnd(a : uint8_t, b : uint8_t) : uint8_t
{static} bitXor(a : uint8_t, b : uint8_t) : uint8_t
{static} rotateRight(v : uint8_t, n : uint8_t) : uint8_t }
ALU ..> Float8 : addFloat delegates

class Instruction {

raw : uint16_t
opcode : uint8_t
r : uint8_t
s : uint8_t
t : uint8_t
xy : uint8_t
Instruction(raw : uint16_t)
{static} fromHexString(line : const std::string&) : std::optional
isValid() const : bool
getOpcode() const : uint8_t
getR() const : uint8_t
getS() const : uint8_t
getT() const : uint8_t
getXY() const : uint8_t
getRaw() const : uint16_t
toString() const : std::string }
note right of Instruction fromHexString: single text->Instruction converter. Strips comment after first token, requires exactly 4 hex digits, constructs Instruction, runs isValid. isValid checks: opcode in 1..C opcode 4: second nibble == 0 (40RS) opcode A: second nibble == 0 (R0X) opcode C: operand == 000 No memory/register-bounds checks here. end note

class CPU {

pc : uint16_t
ir : uint16_t
halted : bool
registers : RegisterFile
memory : Memory*
CPU(memory : Memory&)
setPC(addr : uint16_t) : bool
getPC() const : uint16_t
getIR() const : uint16_t
isHalted() const : bool
clearHalt() : void
step() : StepStatus
reset() : void
displayState(out : std::ostream&) const : void
fetch() : uint16_t
execute(instr : const Instruction&) : StepStatus }
CPU "1" *-- "1" RegisterFile CPU "1" --> "1" Memory : "read / write" CPU ..> Instruction : "creates per fetch" CPU ..> ALU CPU ..> StepStatus

note right of CPU pc is uint16_t so pc+2 reaches 0x0100 without wrapping. setPC(addr): accepts 0x00..0xFE only; returns false and leaves pc unchanged for 0xFF / >=0x100. No clamping. step(): if pc > 0xFE -> PC_OUT_OF_RANGE. Last valid start is 0xFE (bytes 0xFE/0xFF; 0xFF readable). STORE uses memory->write(xy, v); FF side effect in Memory::write. Opcode dispatch is a switch in execute() (see Design Notes). end note

' ==================== Loading ====================

class ProgramLoader {

loadFromFile(path : const std::string&) : LoadOutcome }
ProgramLoader ..> Instruction ProgramLoader ..> LoadOutcome

note right of ProgramLoader Pure parser: iterates lines, skips blanks and lines starting with ';', calls Instruction::fromHexString per line, records the failing line number in message. Returns LoadOutcome with instructions filled and status OK / FILE_NOT_FOUND / PARSE_ERROR / INVALID_INSTRUCTION. Performs NO placement / fit checks. end note

' ==================== Façade ====================

class MachineController {

memory : Memory
screen : Screen
cpu : CPU
loader : ProgramLoader
MachineController()
loadProgram(path : const std::string&, start : uint8_t) : LoadOutcome
overwriteRegister(index : uint8_t, value : uint8_t) : void
overwriteMemory(address : uint8_t, value : uint8_t) : void
setPC(address : uint16_t) : bool
run(maxSteps : unsigned = 1000000) : StepStatus
step() : StepStatus
reset() : void
displayAll(out : std::ostream&) const : void
isHalted() const : bool }
MachineController "1" *-- "1" Memory MachineController "1" *-- "1" Screen MachineController "1" *-- "1" CPU MachineController "1" *-- "1" ProgramLoader MachineController ..> StepStatus MachineController ..> LoadOutcome MachineController ..> LoadStatus

note bottom of MachineController loadProgram order: 1. start even and < 0xFF -> INVALID_START_ADDRESS 2. loader.loadFromFile(path) -> FILE_NOT_FOUND / PARSE_ERROR / INVALID_INSTRUCTION 3. start + 2*N <= 0x100 -> DOES_NOT_FIT 4. commit bytes via memory.writeRaw (NO screen), setPC(start), clearHalt -> OK overwriteMemory: memory.writeRaw (setup; does NOT log to Screen). setPC: forwards to CPU::setPC; returns bool. run(): loops cpu.step() until status != OK; returns terminal StepStatus (never OK); step budget -> STEP_LIMIT_REACHED. reset(): memory.reset(); screen.clear(); cpu.reset() -> PC = 0x00. end note

' ==================== UI ====================

class Menu {

controller : MachineController&
Menu(controller : MachineController&)
run() : void
showMainMenu() const : void
handleLoad() : void
handleEditRegister() : void
handleEditMemory() : void
handleSetPC() : void
handleRun() : void
handleStep() : void
handleDisplay() : void }
Menu "1" --> "1" MachineController : uses

@enduml Design Notes / Resolved Ambiguities These are the gaps the spec leaves open and the choices we made. Record them in your report alongside the diagram.

Screen model — history, not last value. The screen stores every byte written to FF by the running program, displayed in order; this matches "screen content is displayed after the program finishes" and "may store several times." Default mode DECIMAL (example: 5 + 7 = 12 → 0x0C → displays 12); HEX is an optional view; ASCII dropped as unsupported by the spec.

Memory::read(0xFF) returns cells[0xFF] — the last value written to FF by any path — not the screen buffer. The Screen is write-only from the machine's perspective (no instruction reads it).

Two write paths to memory. writeRaw = setup/placement (loadProgram, overwriteMemory); no side effects. write = runtime store (CPU STORE only); writing 0xFF also appends to the Screen history. This keeps program output (Screen) separate from manual machine setup.

FF cell is also ordinary memory. Under this design cells[0xFF] holds the last byte written (program or setup), so LOAD R, FF is well-defined and the memory matrix shows the value. Consequence: 0xFE is a valid program start (bytes 0xFE/0xFF), and the PC bound is pc > 0xFE.

PC width and overflow. pc is uint16_t so pc + 2 can reach 0x0100 without wrapping. step() reports PC_OUT_OF_RANGE when pc > 0xFE; execute may set pc = 0x0100. This prevents the "no HALT → wrap to 0x00 → infinite loop" bug.

setPC validation. Accepts 0x00–0xFE only; rejects 0xFF and above with false and leaves the PC unchanged. No clamping (silent correction hides user error). Validation lives in CPU::setPC (the PC's owner); MachineController::setPC forwards.

reset() semantics. memory.reset(), screen.clear(), cpu.reset() → PC = 0x00, IR = 0, halted = false, registers zeroed. Reset returns to power-on state; it does not remember a "last start address." Loading a program sets PC to its start (assignment requirement); re-running after reset requires reloading.

Placement and fit checks live in MachineController::loadProgram, not in ProgramLoader (which is a pure parser). Order: start address even and < 0xFF → parse file → start + 2*N <= 0x100 → commit. Nothing is written unless all checks pass.

One LoadOutcome (status + message + instructions) is returned by both ProgramLoader::loadFromFile and MachineController::loadProgram. LoadStatus distinguishes the failure reason so the menu can report it (INVALID_START_ADDRESS, FILE_NOT_FOUND, PARSE_ERROR with line number, INVALID_INSTRUCTION with line number, DOES_NOT_FIT).

Instruction validity checks (in isValid): opcode in 1..C; opcode 4 requires second nibble 0 (40RS); opcode A requires second nibble 0 (R0X); opcode C requires operand 000. No memory/register-bounds checks (all nibbles are 0–F by construction). Instruction::fromHexString is the single text→Instruction converter; there is no separate parseLine.

Float8 layout (Brookshear). 1 sign bit, 3 exponent bits in excess-4 (bias 4, real exponent −4…+3), 4-bit mantissa normalized as .1xxx (leading 1 stored explicitly, no implicit 1). 0x00 = +0, 0x80 = −0, both treated as zero. add: overflow saturates to the largest finite magnitude of the sign (0x7F / 0xFF), underflow flushes to signed zero, no denormals, extra mantissa bits truncated, x + 0 = x, 0 + 0 = +0. These are choices — the spec does not define them.

Opcode dispatch is a switch in CPU::execute, not an Instruction subclass hierarchy. Rationale: the opcode set is fixed (13 opcodes), each case is 1–5 lines, and a hierarchy would need 13 subclasses plus a factory plus context-passing (memory, registers, PC, halted) into every subclass, coupling them tightly anyway. The switch is the single place opcode semantics live. Revisit only if the spec ever requires dynamically added opcodes.

Step budget in run(). run(maxSteps = 1'000'000) returns STEP_LIMIT_REACHED if exceeded, guarding against infinite loops (e.g., B000 self-jump) and programs without HALT. It is a safety net, configurable, and not a correctness feature; step-by-step mode is user-paced and needs no budget.

One status enum. StepStatus { OK, HALTED, INVALID_INSTRUCTION, PC_OUT_OF_RANGE, STEP_LIMIT_REACHED } is returned by CPU::step, MachineController::step, and MachineController::run. run() documents that it never returns OK (it loops until a terminal status). RunStatus was removed as redundant.

No duplicated relationships in the diagram: each composition/association is declared once with its multiplicity; there is no separate multiplicities block.

That's the consolidated design and its rationale. If you'd like, the natural next artifact is the interaction sequence for one full run (load → set PC → fetch/decode/execute → PC update → display → invalid-instruction handling → reset → "load another") as a PlantUML sequence diagram — it pairs well with this class diagram for a report.