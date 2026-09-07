#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string getHexForInstruction(const std::string& instr, int& lineNum, std::vector<std::string>& errors, std::vector<int>& cmpStack) {
    std::string trimmed = trim(instr);
    
    if (trimmed.empty() || trimmed[0] == '#') {
        return "";
    }
    
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
        errors.push_back("Line " + std::to_string(lineNum) + ": unknown instruction '" + trimmed + "'");
        return "";
    }
}

int main() {
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
        std::string hex = getHexForInstruction(line, lineNum, errors, cmpStack);
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
