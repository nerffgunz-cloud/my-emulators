#include <iostream>
#include <string>

int main() {
    std::cout << "enter help for list of commands" << std::endl;
    std::string inpt, program = " ";
    int ISA = 0;
    bool log, varmon = false;
    while (true) {
        std::cout << "emulater $> ";
        std::cin >> inpt;
        if (inpt == "help") {
            std::cout << "--help--\nhelp - list of commands\nemu - start emulater\nset - emulater settings\n|    pro - set program\n|    isa - change nibble langage ISA\n|    log - enable/disable log feature\n|    varmon - enable/disable varmon feature\ntip - tips for using the emulater\nquit - quit emulator" << std::endl;
        } else if (inpt == "quit") {
            break;
        } else if (inpt == "set") {
            std::cout << "emulater/set $> ";
            std::cin >> inpt;
            if (inpt == "isa") {
                std::cout << "--nibble-ISA--\n";
                std::cout << "0. none\n1. small v1\n2. main v1 (not done)" << std::endl;
                std::cout << "emulater/set/isa $> ";
                std::cin >> ISA;
            } else if (inpt == "pro") {
                std::cout << "--program-change--" << std::endl;
                std::cout << "emulater/set/pro $> ";
                std::cin >> program;
            } else if (inpt == "log") {
                log = !log;
            } else if (inpt == "varmon") {
                varmon = !varmon;
            }
        } else if (inpt == "tip") {
            std::cout << "--tips--\n";
            std::cout << "the emulater uses hexidecimal encoding\n";
            std::cout << "we recommend using our assembler for programming this ISA\n";
            std::cout << "nibble ISA uses 8 bits for numbers, newer ISA's may have more then 8 bits for the program counter\n";
        } else if (inpt == "emu") {
            std::cout << "press enter to continue\nlog = " << log << "\nvarmon = " << varmon << "\nISA = " << ISA << std::endl;
            std::cout << "emulater/emu $> ";
            std::cin >> inpt;
            if (ISA == 1) {
                int reg[] = {0, 0};
                int stk1[256], stk2[256];
                for (int i = 0; i < 256; i++) {
                    stk1[i] = 0;
                    stk2[i] = 0;
                }
                int stkc[] = {-1, -1};
                bool skif = false;
                
                for (int po = 0; po < program.length(); po++) {
                    char hex = program[po];
                    int nibble = (hex >= 'a') ? (hex - 'a' + 10) : (hex - '0');
                    
                    if (skif) {
                        if (nibble == 0xf) {
                            skif = false;
                        }
                        continue;
                    }
                    
                    if (nibble == 0x0) {
                        if (log) std::cout << "ign" << std::endl;
                    } else if (nibble == 0x2) {
                        reg[0]++;
                        if (log) std::cout << "ad1 reg[0]=" << reg[0] << std::endl;
                    } else if (nibble == 0x3) {
                        reg[1]++;
                        if (log) std::cout << "ad1 reg[1]=" << reg[1] << std::endl;
                    } else if (nibble == 0x4) {
                        reg[0] = stk1[stkc[0]];
                        stkc[0]--;
                        if (log) std::cout << "mvrs reg[0]=" << reg[0] << std::endl;
                    } else if (nibble == 0x5) {
                        reg[1] = stk1[stkc[0]];
                        stkc[0]--;
                        if (log) std::cout << "mvrs reg[1]=" << reg[1] << std::endl;
                    } else if (nibble == 0x6) {
                        reg[0] = stk2[stkc[1]];
                        stkc[1]--;
                        if (log) std::cout << "mvrs reg[0]=" << reg[0] << std::endl;
                    } else if (nibble == 0x7) {
                        reg[1] = stk2[stkc[1]];
                        stkc[1]--;
                        if (log) std::cout << "mvrs reg[1]=" << reg[1] << std::endl;
                    } else if (nibble == 0x8) {
                        stkc[0]++;
                        stk1[stkc[0]] = reg[0];
                        if (log) std::cout << "mvsr reg[0]=" << reg[0] << std::endl;
                    } else if (nibble == 0x9) {
                        stkc[0]++;
                        stk1[stkc[0]] = reg[1];
                        if (log) std::cout << "mvsr reg[1]=" << reg[1] << std::endl;
                    } else if (nibble == 0xa) {
                        stkc[1]++;
                        stk2[stkc[1]] = reg[0];
                        if (log) std::cout << "mvsr reg[0]=" << reg[0] << std::endl;
                    } else if (nibble == 0xb) {
                        stkc[1]++;
                        stk2[stkc[1]] = reg[1];
                        if (log) std::cout << "mvsr reg[1]=" << reg[1] << std::endl;
                    } else if (nibble == 0xc) {
                        reg[0] = reg[0] + reg[1];
                        if (log) std::cout << "add reg[0]=" << reg[0] << std::endl;
                    } else if (nibble == 0xd) {
                        reg[0] = reg[0] - reg[1];
                        if (log) std::cout << "sub reg[0]=" << reg[0] << std::endl;
                    } else if (nibble == 0xe) {
                        if (reg[0] != reg[1]) {
                            skif = true;
                        }
                        if (log) std::cout << "com " << (reg[0] == reg[1]) << std::endl;
                    } else if (nibble == 0xf) {
                        if (log) std::cout << "ecom" << std::endl;
                    }
                    
                    if (varmon) {
                        std::cout << "reg[0]=" << reg[0] << " reg[1]=" << reg[1] << std::endl;
                    }
                }
                
                std::cout << "\nFinal State:\nreg[0]=" << reg[0] << " reg[1]=" << reg[1] << std::endl;
            }
        }
    }
}
