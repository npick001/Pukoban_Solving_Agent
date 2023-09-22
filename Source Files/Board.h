#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "State.h"
#include "ProblemSolvingAgent.h"

class State;
class GameState;
class ProblemSolvingAgent;

class Board
{
public:
	enum CellValues {
		Obstacle = 'O',
		Storage = 'S',
		Box = 'B',
		Robot = 'R',
		Blank = ' '
	};

	// give the board the file with the puzzle, board will handle the rest
	Board(std::string filename);
	~Board();

	// override the current board
	void loadNewBoard(std::string filename);
	void printBoard();

protected:
	void parseFile(std::string filename);

private:
	int m_width;
	int m_height;

	GameState* m_gameState;
	ProblemSolvingAgent* m_robot;
};

class GameState : public State {
public:
	
	GameState(GameState* other);
	~GameState() = default;

	std::vector<Board::CellValues> board;
	std::vector<int> box_positions;
	std::vector<int> storage_positions;
	int robot_position;
	int board_width;

private:
	GameState& operator=(const GameState& other);
	bool operator==(const GameState& other);
};
