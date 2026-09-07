#include <iostream>
#include <string>
void start_program() { //gdb test.c -o output.c
	int variable = 0;
	int inptvar = 0;
	int varalu = 0;
	int ip = 0;
	char inpt = ' ';
	char char1;
	char char2 = ' ';
	bool is_in_bracket = false;
	while (true) {
		std::cin >> char1;
		if (char1 == '[') {
			if (is_in_bracket) {
				std::cout << "error nested bracket is used" << std::endl;
				break;
			}
			is_in_bracket = true;
			std::cin >> char1;
			if (char1 == ',') { // second letter for if then
				std::cin >> inptvar;
				std::cin >> char1;
				if (char1 == '/') { // if then end
					if (inptvar == variable) {
					} else {
						for (int count = 0; count < 1024; count++) {
							std::cin >> char1;
							if (char1 == ']') {
								break;
							}
						}
					}
				}
			}
		} else if (char1 == '.') {
			std::cout << "variable: " << variable << std::endl << "inptvar: " << inptvar << std::endl << "inpt: " << inpt << std::endl;
		} else if (char1 == '*') {
			if (is_in_bracket) {
				std::cout << "program ended too early, program is still in bracket" << std::endl;
			}
			std::cout << "program ended" << std::endl;
			break;
		} else if (char1 == '/') {
			if (char2 == '-' || char2 == '+') varalu = variable;
			std::cin >> variable;
			if (char2 == '-') variable = varalu - variable;
			if (char2 == '+') variable = varalu + variable;
		} else if (char1 == ',') {
			std::cin >> inpt;
			if (inpt == '/') {
				std::cin >> inptvar;
			}
		} else if (char1 == '+') {
			variable++;
		} else if (char1 == '-') {
			variable--;
		} else {
			std::cout << "Error unrecognized command\nprogram failed" << std::endl;
			break;
		}
		char2 = char1;
	}
}
int main() {
	std::cout << "flight emulator v1.0\nenter h if help needed\nenter program" << std::endl;
	char char1;
	while (true) {
		std::cin >> char1;
		if (char1 == '*') {
			std::cout << "program started" << std::endl;
			start_program();
		} else if (char1 == 'h') std::cout << ". prints variable content\n* starts, and ends program\n/ variable ex: */ 23*\n[,/ if then ex: */ 10 [, 10 / .]*   prints 10, 10\n+ adds one to variable\n+/ adds a value to variable ex: *+/ 20 .* prints 21, prints 21 because it adds 1 then 20\n- subtracts one from variable\n-/ subtracts a value from variable ex: */ 10 -/ 5 .* prints 4 prints 4 because it subtracts 1 the subtracts 5 " << std::endl;
	}
	return 0;
}