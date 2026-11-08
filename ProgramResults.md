# Test Programs

| # | Program | Description | Expected Output | Actual Output | Result |
|---|---------|-------------|-----------------|---------------|--------|
| 1 | program1.txt | Load 6 and 8, add, show on screen| Screen: 14, R2 = 0x0E, Halted| Screen: 14, R2 = 0x0E, Halted: Yes| PASS |

| 2 | program2.txt| Load 10 and 15, add, store in memory[02]| R2 = 0x19, memory[02] = 0x19, Halted, Screen empty| R0 = 0x0A, R1 = 0x0F, R2 = 0x19, memory[02] = 0x19, Halted: Yes, Screen empty| PASS |

| 3 | program3.txt | Load 0A and 0F, bitwise OR, store in memory[00]| R2 = 0x0F, memory[00] = 0x0F, Halted, Screen empty| R0 = 0x0A, R1 = 0x0F, R2 = 0x0F, memory[00] = 0x0F, Halted: Yes, Screen empty | PASS |

| 4 | program4.txt | Rotate 01 right by 1, store in memory[00] | R0 = 0x80, memory[00] = 0x80, Halted, Screen empty | R0 = 0x80, memory[00] = 0x80, Halted: Yes, Screen empty | PASS |

| 5 | program5.txt | AND of 0F and A5, store in memory[10] | R2 = 0x05, memory[10] = 0x05, Halted, Screen empty | R0 = 0x0F, R1 = 0xA5, R2 = 0x05, memory[10] = 0x05, Halted: Yes, Screen empty| PASS |

| 6 | program6.txt | Max of 5 and 9 via sign-bit test, print on screen | Screen: 9, R1 = 0x09, R6 = 0x80, memory[FF] = 0x09, Halted |R0 = 0x00, R1 = 0x09, R2 = 0x09, R6 = 0x80, memory[FF] = 0x09, Halted: Yes, Screen: 9 | PASS |

| 7 | program7.txt | Mask AB with 0F (AND), show on screen | R1 = 0xAB, R2 = 0x0F, R3 = 0x0B, memory[FF] = 0x0B, Halted,Screen: 11 (0B)| R1 = 0xAB, R2 = 0x0F, R3 = 0x0B, memory[FF] = 0x0B, Halted: Yes, Screen: 11 | PASS |

| 8 | program8.txt | OR of 0F and F0, store in memory[FF] (screen output) | R0 = 0x0F, R1 = 0xF0, R2 = 0xFF, memory[FF] = 0xFF, Halted, Screen: 255 (FF) | R0 = 0x0F, R1 = 0xF0, R2 = 0xFF, memory[FF] = 0xFF, Halted: Yes, Screen: 255 (FF) | PASS |

| 9 | program9.txt | XOR of AA and 55, store in memory[FF] (screen output) | R0 = 0xAA, R1 = 0x55, R2 = 0xFF, memory[FF] = 0xFF, Halted, Screen: 255 (FF) | R0 = 0xAA, R1 = 0x55, R2 = 0xFF, memory[FF] = 0xFF, Halted: Yes, Screen: 255 (FF) | PASS |

| 10 | program10.txt | Add 2, 3 and 4 using two's complement addition, print result on screen | R0 = 0x02, R1 = 0x03, R2 = 0x05, R3 = 0x04, R4 = 0x09, memory[FF] = 0x09, Halted, Screen: 9 | R0 = 0x02, R1 = 0x03, R2 = 0x05, R3 = 0x04, R4 = 0x09, memory[FF] = 0x09, Halted: Yes, Screen: 9 | PASS |