#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

// --- Token definitions ---
enum class Opcode {
    NOP, ADDA, ADDB, POPA, POPB, PUSHA, PUSHB, ADD, SUB, CMP, ENDIF,
    POPA2, POPB2, PUSHA2, PUSHB2, REBOOT, JMPA, JMPB, INA, INB, OUT,
    ELSE, PRINT, BIN, SWAP, UNKNOWN
};

// O(1) Lookup Table for instant string-to-token conversion
const std::unordered_map<std::string, Opcode> opcodeMap = {
    {"nop", Opcode::NOP}, {"adda", Opcode::ADDA}, {"addb", Opcode::ADDB},
    {"popa", Opcode::POPA}, {"popb", Opcode::POPB}, {"pusha", Opcode::PUSHA},
    {"pushb", Opcode::PUSHB}, {"add", Opcode::ADD}, {"sub", Opcode::SUB},
    {"cmp", Opcode::CMP}, {"endif", Opcode::ENDIF}, {"swap", Opcode::SWAP},
    {"popa2", Opcode::POPA2}, {"popb2", Opcode::POPB2}, {"pusha2", Opcode::PUSHA2},
    {"pushb2", Opcode::PUSHB2}, {"reboot", Opcode::REBOOT}, {"jmpa", Opcode::JMPA},
    {"jmpb", Opcode::JMPB}, {"ina", Opcode::INA}, {"inb", Opcode::INB},
    {"out", Opcode::OUT}, {"else", Opcode::ELSE}
};

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string charToHex(char c) {
    int val = (unsigned char)c;
    std::string hex = "";
    hex += "0123456789abcdef"[val / 16];
    hex += "0123456789abcdef"[val % 16];
    return hex;
}

std::string getHexForInstruction(const std::string& instr, int& lineNum, std::vector<std::string>& errors, std::vector<int>& cmpStack, int isa) {
    std::string trimmed = trim(instr);
    
    // Ignore empty lines and comments
    if (trimmed.empty() || trimmed[0] == '#') {
        return "";
    }
    
    // 1. Lexer: Convert string to Opcode token
    Opcode op = Opcode::UNKNOWN;
    
    if (trimmed.substr(0, 6) == "print(") {
        op = Opcode::PRINT;
    } else if (trimmed.substr(0, 3) == "bin") {
        op = Opcode::BIN;
    } else {
        auto it = opcodeMap.find(trimmed);
        if (it != opcodeMap.end()) {
            op = it->second;
        }
    }

    // 2. Parser: Switch dispatch for code generation
    switch (op) {
        // --- Base ISA (v1) ---
        case Opcode::NOP:   return "0";
        case Opcode::ADDA:  return "2";
        case Opcode::ADDB:  return "3";
        case Opcode::POPA:  return "4";
        case Opcode::POPB:  return "5";
        case Opcode::PUSHA: return "8";
        case Opcode::PUSHB: return "9";
        case Opcode::ADD:   return "c";
        case Opcode::SUB:   return "d";
        case Opcode::SWAP:  return "89a5"; // Macro expansion
        
        case Opcode::CMP:
            cmpStack.push_back(lineNum);
            return "e";
            
        case Opcode::ENDIF:
            if (cmpStack.empty()) {
                errors.push_back("Line " + std::to_string(lineNum) + ": endif without matching cmp");
                return "";
            }
            cmpStack.pop_back();
            return (isa >= 2) ? "f6" : "f";

        // --- Main V1 (ISA 2) Additions ---
        case Opcode::POPA2:  if (isa >= 2) return "6"; break;
        case Opcode::POPB2:  if (isa >= 2) return "7"; break;
        case Opcode::PUSHA2: if (isa >= 2) return "a"; break;
        case Opcode::PUSHB2: if (isa >= 2) return "b"; break;
        case Opcode::REBOOT: if (isa >= 2) return "f0"; break;
        case Opcode::JMPA:   if (isa >= 2) return "f22"; break;
        case Opcode::JMPB:   if (isa >= 2) return "f23"; break;
        case Opcode::INA:    if (isa >= 2) return "f4"; break;
        case Opcode::INB:    if (isa >= 2) return "f5"; break;
        case Opcode::OUT:    if (isa >= 2) return "f7"; break;

        // --- Main V2 (ISA 3) Additions ---
        case Opcode::ELSE:   if (isa >= 3) return "f9"; break;
        
        case Opcode::BIN:
            if (isa >= 3) {
                size_t ebinPos = trimmed.find("ebin");
                if (ebinPos != std::string::npos) {
                    return trim(trimmed.substr(3, ebinPos - 3));
                }
                errors.push_back("Line " + std::to_string(lineNum) + ": bin without ebin");
                return "";
            }
            break;
            
        case Opcode::PRINT:
            if (isa >= 3) {
                size_t endPos = trimmed.find(')');
                if (endPos != std::string::npos && endPos > 7) {
                    std::string content = trimmed.substr(6, endPos - 6);
                    
                    if (content[0] == '\'' && content[content.length()-1] == '\'') {
                        if (content.length() == 3) {
                            return std::string("f8") + charToHex(content[1]);
                        }
                        errors.push_back("Line " + std::to_string(lineNum) + ": print('x') requires single character");
                        return "";
                    } else if (content[0] == '"' && content[content.length()-1] == '"') {
                        std::string str = content.substr(1, content.length() - 2);
                        std::string result = "";
                        for (char c : str) result += std::string("f8") + charToHex(c);
                        return result;
                    }
                    errors.push_back("Line " + std::to_string(lineNum) + ": print requires single quotes or double quotes");
                    return "";
                }
                errors.push_back("Line " + std::to_string(lineNum) + ": invalid print syntax");
                return "";
            }
            break;
            
        default:
            break; // Falls through to error handler below
    }
    
    // If we reach here, the instruction was either UNKNOWN or restricted by the current ISA
    errors.push_back("Line " + std::to_string(lineNum) + ": unknown or unsupported instruction for this ISA '" + trimmed + "'");
    return "";
}

int main() {
    std::cout << "Nibble Assembler" << std::endl;
    std::cout << "Select ISA:\n1. small v1\n2. main v1\n3. main v2\nChoice: ";
    
    int isa;
    if (!(std::cin >> isa) || isa < 1 || isa > 3) {
        std::cout << "Invalid ISA selection" << std::endl;
        return 1;
    }
    
    std::ifstream inputFile("nibble.asm");
    if (!inputFile.is_open()) {
        std::cout << "error: could not open nibble.asm" << std::endl;
        return 1;
    }
    
    std::string line;
    std::string hexOutput = "";
    std::vector<std::string> errors;
    std::vector<int> cmpStack;
    int lineNum = 1;
    
    while (std::getline(inputFile, line)) {
        std::string hex = getHexForInstruction(line, lineNum, errors, cmpStack, isa);
        hexOutput += hex;
        lineNum++;
    }
    
    inputFile.close();
    
    if (!cmpStack.empty()) {
        errors.push_back("error: unclosed cmp block starting at line " + std::to_string(cmpStack.back()));
    }
    
    if (!errors.empty()) {
        std::cout << "Errors during assembly:" << std::endl;
        for (const auto& err : errors) {
            std::cout << err << std::endl;
        }
        std::cout << std::endl;
    } else {
        std::ofstream outputFile("nibble.hex");
        if (outputFile.is_open()) {
            outputFile << hexOutput;
            outputFile.close();
            std::cout << "Assembly complete.\nOutput: " << hexOutput << "\nWritten to nibble.hex" << std::endl;
        } else {
            std::cout << "error: could not write to nibble.hex" << std::endl;
        }
    }
    
    return 0;
}
