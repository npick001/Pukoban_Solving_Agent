#include "Board.h"

Board::Board(std::string filename)
{
	m_width = 0;
	m_height = 0;

	m_gameState = new GameState();

	parseFile(filename);
}

Board::~Board()
{
	delete m_gameState;
	delete m_robot;
}

void Board::loadNewBoard(std::string filename)
{
	parseFile(filename);
}

void Board::printBoard()
{
	for (int i = 0; i < m_gameState->board.size(); i++) {

		// back at start of row again
		if ((i % m_width == 0) && (i != 0)) {
			std::cout << '\n';
		}
		std::cout << (char)m_gameState->board[i] << " ";
	}
	std::cout << '\n';
}

void Board::parseFile(std::string filename)
{
	std::ifstream input_file(filename);

	if (!input_file.is_open()) {
		std::cout << "Error loading file: " << filename << std::endl;
	}

	int char_number = 0;
	int board_width = -1;
	int board_height = 0;

	while (!input_file.eof()) {
		char current_char = input_file.get();

		if (current_char == Obstacle) {
			m_gameState->board.push_back(Obstacle);
		} 
		else if (current_char == Storage) {
			m_gameState->board.push_back(Storage);
			m_gameState->storage_positions.push_back(char_number);
		}
		else if (current_char == Box) {
			//m_gameState->board.push_back(Box);
			m_gameState->box_positions.push_back(char_number);
		}
		else if (current_char == Robot) {
			//m_gameState->board.push_back(Robot);
			m_gameState->robot_position = char_number;
		}
		else if (current_char == Blank) {
			m_gameState->board.push_back(Blank);
		}
		else if (current_char == '\n') {
			if (board_width == -1) {
				m_width = char_number;
				board_width = char_number;
			}
			char_number--;
		}
		char_number++;
	}
	board_height = char_number / board_width;
	m_height = board_height;

	m_robot = new ProblemSolvingAgent(m_gameState);

	printBoard();
}

bool GameState::operator==(const GameState& other)
{
	for (int i = 0; i < board.size(); i++) {
		if (other.board[i] != board[i]) {
			return false;
		}
	}
	
	for (int i = 0; i < box_positions.size(); i++) {
		if (other.box_positions[i] != box_positions[i]) {
			return false;
		}
	}

	if (other.robot_position != robot_position) {
		return false;
	}

	return true;
}
