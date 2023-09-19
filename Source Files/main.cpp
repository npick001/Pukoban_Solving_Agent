#include "Board.h"

int main() {

	std::string puzzle1 = ".\\Test Cases\\test1.txt";
	std::string puzzle2 = ".\\Test Cases\\test2.txt";
	std::string puzzle3 = ".\\Test Cases\\test3.txt";

	Board* pukoban = new Board(puzzle3);

	return 0;
}