#include <iostream>
#include <string>

// Inline helper for fast, safe hex conversion (supports both lower and upper case)
inline int hexToInt(char hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    return 0;
}

int main() {
    std::cout << "enter help for list of commands" << std::endl;
    std::string inpt, program = " ";
    int ISA = 0;
    bool log = false;
    bool varmon = false; 
    bool quiet = false;
    
    while (true) {
        std::cout << "emulater $> ";
        std::cin >> inpt;
        
        if (inpt == "help") {
            std::cout << "--help--\nhelp - list of commands\nemu - start emulater\nset - emulater settings\n|    pro - set program\n|    isa - change nibble langage ISA\n|    log - enable/disable log feature\n|    varmon - enable/disable varmon feature\n|    quiet - hide input/output prompts\ntip - tips for using the emulater\nquit - quit emulator" << std::endl;
        } else if (inpt == "quit") {
            break;
        } else if (inpt == "set") {
            std::cout << "emulater/set $> ";
            std::cin >> inpt;
            if (inpt == "isa") {
                std::cout << "--nibble-ISA--\n1. small v1\n2. main v1\n3. main v2\nemulater/set/isa $> ";
                std::cin >> ISA;
            } else if (inpt == "pro") {
                std::cout << "--program-change--\nemulater/set/pro $> ";
                std::cin >> program;
            } else if (inpt == "log") {
                log = !log;
            } else if (inpt == "varmon") {
                varmon = !varmon;
            } else if (inpt == "quiet") {
                quiet = !quiet;
            }
        } else if (inpt == "tip") {
            std::cout << "--tips--\nthe emulater uses hexidecimal encoding\nwe recommend using our assembler for programming this ISA\nnibble ISA uses 8 bits for numbers, newer ISA's may have more then 8 bits for the program counter\n";
        } else if (inpt == "emu") {
            std::cout << "press enter to continue\nlog = " << log << "\nvarmon = " << varmon << "\nquiet = " << quiet << "\nISA = " << ISA << std::endl;
            std::cout << "emulater/emu $> ";
            std::cin >> inpt;
            
            // Unified Execution Core for all 3 ISAs
            int reg[2] = {0, 0};
            int stk1[256] = {0}; 
            int stk2[256] = {0};
            int stkc[2] = {-1, -1};
            bool skif = false;
            bool elseMode = false;
            
            for (int po = 0; po < program.length(); po++) {
                int nibble = hexToInt(program[po]);
                
                // --- Skip Logic ---
                if (skif) {
                    if (nibble == 0xf) {
                        if (ISA == 1) {
                            skif = false;
                        } else if (po + 1 < program.length()) {
                            int nibble2 = hexToInt(program[po + 1]);
                            if (nibble2 == 0x6) {
                                skif = false;
                                elseMode = false;
                                po++; // consume the second nibble
                            } else if (ISA == 3 && nibble2 == 0x9) {
                                skif = false;
                                elseMode = true;
                                po++; // consume the second nibble
                            }
                        }
                    }
                    continue;
                }
                
                if (elseMode) {
                    if (nibble == 0xf && po + 1 < program.length()) {
                        int nibble2 = hexToInt(program[po + 1]);
                        if (nibble2 == 0x6) {
                            elseMode = false;
                            po++; // consume the second nibble
                        }
                    }
                    continue;
                }
                
                // --- Switch Dispatcher ---
                switch (nibble) {
                    case 0x0: 
                        if (log) std::cout << "nop\n"; 
                        break;
                    case 0x2: 
                        reg[0]++; 
                        if (log) std::cout << "adda reg[0]=" << reg[0] << "\n"; 
                        break;
                    case 0x3: 
                        reg[1]++; 
                        if (log) std::cout << "addb reg[1]=" << reg[1] << "\n"; 
                        break;
                    case 0x4: 
                        if (stkc[0] >= 0) { reg[0] = stk1[stkc[0]--]; }
                        if (log) std::cout << "popa reg[0]=" << reg[0] << "\n"; 
                        break;
                    case 0x5: 
                        if (stkc[0] >= 0) { reg[1] = stk1[stkc[0]--]; }
                        if (log) std::cout << "popb reg[1]=" << reg[1] << "\n"; 
                        break;
                    case 0x6: 
                        if (ISA >= 2 && stkc[1] >= 0) { reg[0] = stk2[stkc[1]--]; }
                        if (log && ISA >= 2) std::cout << "popa2 reg[0]=" << reg[0] << "\n"; 
                        break;
                    case 0x7: 
                        if (ISA >= 2 && stkc[1] >= 0) { reg[1] = stk2[stkc[1]--]; }
                        if (log && ISA >= 2) std::cout << "popb2 reg[1]=" << reg[1] << "\n"; 
                        break;
                    case 0x8: 
                        if (stkc[0] < 255) { stk1[++stkc[0]] = reg[0]; }
                        if (log) std::cout << "pusha reg[0]=" << reg[0] << "\n"; 
                        break;
                    case 0x9: 
                        if (stkc[0] < 255) { stk1[++stkc[0]] = reg[1]; }
                        if (log) std::cout << "pushb reg[1]=" << reg[1] << "\n"; 
                        break;
                    case 0xa: 
                        if (ISA >= 2 && stkc[1] < 255) { stk2[++stkc[1]] = reg[0]; }
                        if (log && ISA >= 2) std::cout << "pusha2 reg[0]=" << reg[0] << "\n"; 
                        break;
                    case 0xb: 
                        if (ISA >= 2 && stkc[1] < 255) { stk2[++stkc[1]] = reg[1]; }
                        if (log && ISA >= 2) std::cout << "pushb2 reg[1]=" << reg[1] << "\n"; 
                        break;
                    case 0xc: 
                        reg[0] += reg[1]; 
                        if (log) std::cout << "add reg[0]=" << reg[0] << "\n"; 
                        break;
                    case 0xd: 
                        reg[0] -= reg[1]; 
                        if (log) std::cout << "sub reg[0]=" << reg[0] << "\n"; 
                        break;
                    case 0xe: 
                        skif = (reg[0] != reg[1]); 
                        if (log) std::cout << "cmp " << (reg[0] == reg[1]) << "\n"; 
                        break;
                    case 0xf:
                        if (ISA >= 2 && po + 1 < program.length()) {
                            po++;
                            int nibble2 = hexToInt(program[po]);
                            
                            switch (nibble2) {
                                case 0x0:
                                    reg[0] = 0; reg[1] = 0; stkc[0] = -1; stkc[1] = -1;
                                    for (int i = 0; i < 256; i++) { stk1[i] = 0; stk2[i] = 0; }
                                    if (log) std::cout << "reboot\n";
                                    break;
                                case 0x2:
                                    if (po + 1 < program.length()) {
                                        po++;
                                        int nibble3 = hexToInt(program[po]);
                                        if (nibble3 == 0x2) { 
                                            po = reg[0]; 
                                            if (log) std::cout << "jmpa reg[0]=" << reg[0] << "\n"; 
                                        } else if (nibble3 == 0x3) { 
                                            po = reg[1]; 
                                            if (log) std::cout << "jmpb reg[1]=" << reg[1] << "\n"; 
                                        } else { po--; }
                                    }
                                    break;
                                case 0x4: {
                                    int input;
                                    if (!quiet) std::cout << "input: ";
                                    std::cin >> input;
                                    if (stkc[0] < 255) stk1[++stkc[0]] = input;
                                    if (log) std::cout << "ina stk1=" << input << "\n";
                                    break;
                                }
                                case 0x5: {
                                    int input;
                                    if (!quiet) std::cout << "input: ";
                                    std::cin >> input;
                                    if (stkc[1] < 255) stk2[++stkc[1]] = input;
                                    if (log) std::cout << "inb stk2=" << input << "\n";
                                    break;
                                }
                                case 0x6:
                                    if (log) std::cout << "endif\n";
                                    break;
                                case 0x7:
                                    std::cout << reg[0] << "\n";
                                    if (log) std::cout << " [out reg[0]=" << reg[0] << "]\n";
                                    break;
                                case 0x8:
                                    if (ISA == 3 && po + 2 < program.length()) {
                                        int val3 = hexToInt(program[++po]);
                                        int val4 = hexToInt(program[++po]);
                                        int charVal = (val3 << 4) | val4;
                                        std::cout << (char)charVal;
                                        if (log) std::cout << " [print '" << (char)charVal << "']";
                                    } else if (ISA == 3) {
                                        po += 2; // Prevent infinite loop on bad syntax
                                    } else { po--; }
                                    break;
                                case 0x9:
                                    if (ISA == 3) {
                                        elseMode = true;
                                        if (log) std::cout << "else\n";
                                    } else { po--; }
                                    break;
                                default:
                                    po--;
                                    break;
                            }
                        }
                        break;
                }
                
                if (varmon) {
                    std::cout << "reg[0]=" << reg[0] << " reg[1]=" << reg[1] << "\n";
                }
            }
            
            std::cout << "\nFinal State:\nreg[0]=" << reg[0] << " reg[1]=" << reg[1] << std::endl;
        }
    }
}
