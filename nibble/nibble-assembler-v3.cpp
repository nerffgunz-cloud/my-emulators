#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

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

std::string expandMacro(const std::string& macro, int isa) {
    if (macro == "swap") {
        return std::string("8") + "9" + "a" + "5";
    }
    return "";
}

std::string getHexForInstruction(const std::string& instr, int& lineNum, std::vector<std::string>& errors, std::vector<int>& cmpStack, int isa) {
    std::string trimmed = trim(instr);
    
    if (trimmed.empty() || trimmed[0] == '#') {
        return "";
    }
    
    if (isa == 1) {
        if (trimmed == "nop") {
            return "0";
        } else if (trimmed == "adda") {
            return "2";
        } else if (trimmed == "addb") {
            return "3";
        } else if (trimmed == "popa") {
            return "4";
        } else if (trimmed == "popb") {
            return "5";
        } else if (trimmed == "pusha") {
            return "8";
        } else if (trimmed == "pushb") {
            return "9";
        } else if (trimmed == "add") {
            return "c";
        } else if (trimmed == "sub") {
            return "d";
        } else if (trimmed == "cmp") {
            cmpStack.push_back(lineNum);
            return "e";
        } else if (trimmed == "endif") {
            if (cmpStack.empty()) {
                errors.push_back("Line " + std::to_string(lineNum) + ": endif without matching cmp");
            } else {
                cmpStack.pop_back();
            }
            return "f";
        } else {
            std::string macro = expandMacro(trimmed, isa);
            if (!macro.empty()) {
                return macro;
            }
            errors.push_back("Line " + std::to_string(lineNum) + ": unknown instruction '" + trimmed + "'");
            return "";
        }
    } else if (isa == 2) {
        if (trimmed == "nop") {
            return "0";
        } else if (trimmed == "adda") {
            return "2";
        } else if (trimmed == "addb") {
            return "3";
        } else if (trimmed == "popa") {
            return "4";
        } else if (trimmed == "popb") {
            return "5";
        } else if (trimmed == "popa2") {
            return "6";
        } else if (trimmed == "popb2") {
            return "7";
        } else if (trimmed == "pusha") {
            return "8";
        } else if (trimmed == "pushb") {
            return "9";
        } else if (trimmed == "pusha2") {
            return "a";
        } else if (trimmed == "pushb2") {
            return "b";
        } else if (trimmed == "add") {
            return "c";
        } else if (trimmed == "sub") {
            return "d";
        } else if (trimmed == "cmp") {
            cmpStack.push_back(lineNum);
            return "e";
        } else if (trimmed == "reboot") {
            return "f0";
        } else if (trimmed == "jmpa") {
            return "f22";
        } else if (trimmed == "jmpb") {
            return "f23";
        } else if (trimmed == "ina") {
            return "f4";
        } else if (trimmed == "inb") {
            return "f5";
        } else if (trimmed == "endif") {
            if (cmpStack.empty()) {
                errors.push_back("Line " + std::to_string(lineNum) + ": endif without matching cmp");
            } else {
                cmpStack.pop_back();
            }
            return "f6";
        } else if (trimmed == "out") {
            return "f7";
        } else {
            std::string macro = expandMacro(trimmed, isa);
            if (!macro.empty()) {
                return macro;
            }
            errors.push_back("Line " + std::to_string(lineNum) + ": unknown instruction '" + trimmed + "'");
            return "";
        }
    } else if (isa == 3) {
        if (trimmed == "nop") {
            return "0";
        } else if (trimmed == "adda") {
            return "2";
        } else if (trimmed == "addb") {
            return "3";
        } else if (trimmed == "popa") {
            return "4";
        } else if (trimmed == "popb") {
            return "5";
        } else if (trimmed == "popa2") {
            return "6";
        } else if (trimmed == "popb2") {
            return "7";
        } else if (trimmed == "pusha") {
            return "8";
        } else if (trimmed == "pushb") {
            return "9";
        } else if (trimmed == "pusha2") {
            return "a";
        } else if (trimmed == "pushb2") {
            return "b";
        } else if (trimmed == "add") {
            return "c";
        } else if (trimmed == "sub") {
            return "d";
        } else if (trimmed == "cmp") {
            cmpStack.push_back(lineNum);
            return "e";
        } else if (trimmed == "reboot") {
            return "f0";
        } else if (trimmed == "jmpa") {
            return "f22";
        } else if (trimmed == "jmpb") {
            return "f23";
        } else if (trimmed == "ina") {
            return "f4";
        } else if (trimmed == "inb") {
            return "f5";
        } else if (trimmed == "endif") {
            if (cmpStack.empty()) {
                errors.push_back("Line " + std::to_string(lineNum) + ": endif without matching cmp");
            } else {
                cmpStack.pop_back();
            }
            return "f6";
        } else if (trimmed == "else") {
            return "f9";
        } else if (trimmed == "out") {
            return "f7";
        } else if (trimmed.substr(0, 6) == "print(") {
    size_t endPos = trimmed.find(')');
    if (endPos != std::string::npos && endPos > 7) {
        // Start at index 6, length is endPos - 6
        std::string content = trimmed.substr(6, endPos - 6);
        
        if (content[0] == '\'' && content[content.length()-1] == '\'') {
            if (content.length() == 3) {
                char c = content[1];
                return std::string("f8") + charToHex(c);
            } else {
                errors.push_back("Line " + std::to_string(lineNum) + ": print('x') requires single character");
                return "";
            }
        } else if (content[0] == '"' && content[content.length()-1] == '"') {
            std::string str = content.substr(1, content.length() - 2);
            std::string result = "";
            for (char c : str) {
                result += std::string("f8") + charToHex(c);
            }
            return result;
        } else {
            errors.push_back("Line " + std::to_string(lineNum) + ": print requires single quotes or double quotes");
            return "";
        }
    } else {
        errors.push_back("Line " + std::to_string(lineNum) + ": invalid print syntax");
        return "";
    }
        } else if (trimmed.substr(0, 3) == "bin") {
            size_t ebinPos = trimmed.find("ebin");
            if (ebinPos != std::string::npos) {
                std::string hexContent = trimmed.substr(3, ebinPos - 3);
                return trim(hexContent);
            } else {
                errors.push_back("Line " + std::to_string(lineNum) + ": bin without ebin");
                return "";
            }
        } else if (trimmed == "swap") {
            return std::string("8") + "9" + "a" + "5";
        } else {
            std::string macro = expandMacro(trimmed, isa);
            if (!macro.empty()) {
                return macro;
            }
            errors.push_back("Line " + std::to_string(lineNum) + ": unknown instruction '" + trimmed + "'");
            return "";
        }
    }
    
    return "";
}

int main() {
    std::cout << "Nibble Assembler" << std::endl;
    std::cout << "Select ISA:" << std::endl;
    std::cout << "1. small v1" << std::endl;
    std::cout << "2. main v1" << std::endl;
    std::cout << "3. main v2" << std::endl;
    std::cout << "Choice: ";
    
    int isa;
    std::cin >> isa;
    
    if (isa < 1 || isa > 3) {
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
    }
    
    std::ofstream outputFile("nibble.hex");
    outputFile << hexOutput;
    outputFile.close();
    
    std::cout << "Assembly complete." << std::endl;
    std::cout << "Output: " << hexOutput << std::endl;
    std::cout << "Written to nibble.hex" << std::endl;
    
    return 0;
}
