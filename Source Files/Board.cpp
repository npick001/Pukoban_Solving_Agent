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
	m_gameState->print();
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
			m_gameState->obstacle_positions.push_back(char_number);
		} 
		else if (current_char == Storage) {
			m_gameState->board.push_back(Blank);
			m_gameState->storage_positions.push_back(char_number);
		}
		else if (current_char == Box) {
			m_gameState->board.push_back(Blank);
			m_gameState->box_positions.push_back(char_number);
		}
		else if (current_char == Robot) {
			m_gameState->board.push_back(Blank);
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
	m_gameState->board_width = board_width;
	m_height = board_height;

	m_robot = new ProblemSolvingAgent(m_gameState);

	printBoard();
	std::cout << '\n';
	m_robot->Solve();
}

GameState::GameState() : State()
{
	board_width = 1;
	robot_position = -1;
}

GameState::GameState(const GameState& other)
	: State(), board(other.board),
	box_positions(other.box_positions),
	storage_positions(other.storage_positions),
	obstacle_positions(other.obstacle_positions),
	robot_position(other.robot_position),
	board_width(other.board_width)
{
}

void GameState::print()
{
	std::cout << "Cost: " << m_manhattan_distance << std::endl;
	std::cout << "Robot Position: " << robot_position << std::endl;
	for (int k = 0; k < box_positions.size(); k++) {
		std::cout << "Box " << std::to_string(k) << " Position: " << box_positions[k] << std::endl;
	}

	// add obstacle positions to board
	for (int k = 0; k < obstacle_positions.size(); k++) {
		board[obstacle_positions[k]] = Board::CellValues::Obstacle;
	}

	// add storage positions to board
	for (int k = 0; k < storage_positions.size(); k++) {
		board[storage_positions[k]] = Board::CellValues::Storage;
	}

	// add box positions to board
	for (int k = 0; k < box_positions.size(); k++) {
		board[box_positions[k]] = Board::CellValues::Box;
	}

	// add robot position to board
	board[robot_position] = Board::CellValues::Robot;

	// print board
	for (int i = 0; i < board.size(); i++) {

		// back at start of row again
		if ((i % board_width == 0) && (i != 0)) {
			std::cout << '\n';
		}

		std::cout << (char)board[i] << " ";
	}
	std::cout << '\n';

	// clear the board again and add the obstacles back
	for (int i = 0; i < board.size(); i++) {
		board[i] = Board::CellValues::Blank;
	}
	for (int k = 0; k < obstacle_positions.size(); k++) {
		board[obstacle_positions[k]] = Board::CellValues::Obstacle;
	}
}

std::string GameState::to_string()
{
	// generate the string version of the print function
	std::string output = "";

	// add obstacle positions to board
	for (int k = 0; k < obstacle_positions.size(); k++) {
		board[obstacle_positions[k]] = Board::CellValues::Obstacle;
	}

	// add storage positions to board
	for (int k = 0; k < storage_positions.size(); k++) {
		board[storage_positions[k]] = Board::CellValues::Storage;
	}

	// add box positions to board
	for (int k = 0; k < box_positions.size(); k++) {
		board[box_positions[k]] = Board::CellValues::Box;
	}

	// add robot position to board
	board[robot_position] = Board::CellValues::Robot;

	// put board into string
	for (int i = 0; i < board.size(); i++) {

		// back at start of row again
		if ((i % board_width == 0) && (i != 0)) {
			output += '\n';
		}

		output += (char)board[i];
	}
	output += '\n';
	return output;
}

GameState& GameState::operator=(const GameState& other)
{
	if (this != &other) { // self-assignment check
		board = other.board;
		box_positions = other.box_positions;
		storage_positions = other.storage_positions;
		obstacle_positions = other.obstacle_positions;
		robot_position = other.robot_position;
		board_width = other.board_width;
	}
	return *this;
}

bool GameState::operator==(const GameState& other)
{	
	//State::operator==(other);

	// Compare board_width and robot_position
	if (robot_position != other.robot_position) {
		return false;
	}

	// Compare box_positions, movable_box_positions, and storage_positions
	for (int i = 0; i < box_positions.size(); i++) {
		
		bool boxMatched = false;
		for (int j = 0; j < other.box_positions.size(); j++) {
			if (box_positions[i] == other.box_positions[j]) {
				boxMatched = true;
				break;
			}
		}

		if (!boxMatched) {
			return false;
		}
	}

	// If all the above checks passed, then the objects are equal
	return true;
}
