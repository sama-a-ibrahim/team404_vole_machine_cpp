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

| 11 | program11.txt | XOR of 0A and 08, rotate right by 1, show on screen | R0 = 0x0A, R1 = 0x08, RA = 0x01, memory[FF] = 0x01, Halted ,Screen: 1 (01) | R0 = 0x0A, R1 = 0x08, RA = 0x01, memory[FF] = 0x01, Halted: Yes, Screen: 1 | PASS |

| 12 | program12.txt | Parity check of 10+5 via AND with 1, show on screen | R3 = 0x0F, R4 = 0x01, memory[FF] = 0x01, Halted,Screen: 1 | R0 = 0x01, R1 = 0x0A, R2 = 0x05, R3 = 0x0F, R4 = 0x01, memory[FF] = 0x01, Halted: Yes, Screen: 1 | PASS |

| 13 | program13.txt | 6x2 minus 3 via two's complement, show on screen | R2 = 0x0C, R4 = 0xFD, R6 = 0x09, memory[FF] = 0x09, Halted, Screen: 9 | R1 = 0x06, R2 = 0x0C, R3 = 0x03, R4 = 0xFD, R6 = 0x09, memory[FF] = 0x09, Halted: Yes, Screen: 9 | PASS |

| 14 | program14.txt | Countdown 5 to 1 using JUMP loop | R1 = 0x00, R2 = 0xFF, memory[FF] = 0x01, Halted, Screen: 5 4 3 2 1 | R1 = 0x00, R2 = 0xFF, memory[FF] = 0x01, Halted: Yes, Screen: 5 4 3 2 1 | PASS |

| 15 | program15.txt | 3 x 4 by repeated addition, show on screen | R0 = 0x0C, R3 = 0x04, memory[FF] = 0x0C, Halted, Screen: 12 | R0 = 0x0C, R2 = 0x03, R3 = 0x04, memory[FF] = 0x0C, Halted: Yes, Screen: 12 | PASS |

| 16 | program16.txt | Average of 2,4,6,8 via rotate right by 2, show on screen | R7 = 0x05, memory[FF] = 0x05, Halted, Screen: 5 | R7 = 0x05, memory[FF] = 0x05, Halted: Yes, Screen: 5 | PASS |

| 17 | program17.txt | 12 x 2 by loop, minus 4, show on screen | R2 = 0x14, R3 = 0x02, memory[FF] = 0x14, Halted, Screen: 20 | R0 = 0x02, R1 = 0x0C, R2 = 0x14, R3 = 0x02, R4 = 0x01, R5 = 0xFC, memory[FF] = 0x14, Halted: Yes, Screen: 20 | PASS |

| 18 | program18.txt | Opcode 0 | Rejected: INVALID_INSTRUCTION | Loading failed:  Line 5 : invalid instruction 0000
Line: 5 | PASS |

| 19 | program19.txt | No HALT | Stops with error, no crash | Stopped: "Invalid instruction." at PC = 0x06, R0 = 0x06, R1 = 0x08, R2 = 0x0E, Halted: No | PASS |

| 20 | program20.txt | Infinite loop | STEP_LIMIT_REACHED | "Step limit reached." PC = 0x00, IR = 0xB000, Halted: No, no crash | PASS |