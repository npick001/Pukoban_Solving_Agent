#include "Board.h"

int main() {

	std::string puzzle0 = ".\\Test Cases\\testlevel_0.txt";
	std::string puzzle0_1 = ".\\Test Cases\\testlevel_0_1.txt";
	std::string puzzle1 = ".\\Test Cases\\testlevel_1.txt";
	std::string puzzle1_5 = ".\\Test Cases\\testlevel_1_5.txt";
	std::string puzzle2 = ".\\Test Cases\\testlevel_2.txt";
	std::string puzzle3 = ".\\Test Cases\\testlevel_3.txt";

	Board* pukoban = new Board(puzzle1_5);
	return 0;
}