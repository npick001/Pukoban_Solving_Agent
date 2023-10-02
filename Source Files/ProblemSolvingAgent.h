#pragma once
#include <chrono>
#include <algorithm>
#include <vector>
#include <map>
#include <thread>
#include <stdlib.h> 
#include <utility>

#include "PriorityQueue.h"
#include "Fringe.h"
#include "Board.h"
class GameState;

#define PRINT_SOLUTION_PATH 0
#define WRITE_SOLUTION_PATH_TO_FILE 1

// IF YOU CHOSE TO MULTI-THREAD THE SOLVE FUNCTION
// YOU ARE ACCEPTING ALL 4 ALGORITHMS WILL BE RAN
// {A*, VFS, DFS, Manhattan Heuristic}
#define MULTI_THREADED_SOLVE 1

#if MULTI_THREADED_SOLVE == 0
	// define the search algorithm to use
	// designed so many can be ran at once
	// takes a long time to run all of them on puzzles harder than 1_5
	// recommend testing on puzzle1_5
	#define BREADTH_FIRST_SEARCH 1
	#define DEPTH_FIRST_SEARCH 1
	#define MANHATTAN_HEURISTIC 1
	#define MY_HEURISTIC 0
	#define UNIFORM_COST_SEARCH 0
	#define GREEDY_SEARCH 0 // Not working
	#define A_STAR_SEARCH 1
#endif // MULTI_THREADED_SOLVE == 0

// {action, gamestate} form state space {arc, node}
enum class Action {
	Push_North = 0,
	Push_South = 1,
	Push_East = 2,
	Push_West = 3,

	Pull_North = 4,
	Pull_South = 5,
	Pull_East = 6,
	Pull_West = 7,
};

class ProblemSolvingAgent
{
public:
	ProblemSolvingAgent(GameState* initial_state);
	~ProblemSolvingAgent();
	void Solve();
	void MultiThreadSolve();

protected:
	GameState* formulateGoalState(GameState* initial);

	/*
		MUST BE CALLED TO START THE STATE GENERATION
		Highest level function to generate all states
		and connect them into the state space.

		State space is represented through pointers between
		states.
	*/
	void generate_BFS_StateSpace(GameState* initial); // working
	void generate_DFS_StateSpace(GameState* initial); // working
	void generate_Manhattan_StateSpace(GameState* initial); // working
	void generate_MyHeuristic_StateSpace(GameState* initial); // working
	void generate_UniformCost_StateSpace(GameState* initial); // working
	void generate_AStar_StateSpace(GameState* initial); // working

	void generate_Greedy_StateSpace(GameState* initial); // NOT working

	// successor function
	// return all valid game states from given game state 
	std::vector<GameState*> successor(GameState* given_state);
	bool goalTest(GameState* given_state);

	// manhattan distance heuristic
	// returns the sum of the manhattan distances of all boxes to their closest goals
	int manhattan_heuristic(GameState* given_state);

	// my heuristic
	// returns the sum of box distances to all storages
	// minus the sum of all storage distances to each other
	// ideally it should work towards having all boxes near all storages
	// doesnt seem to be optimal - much worse than manhattan
	int heuristic(GameState* given_state);

private:
	// returns the set of actions that can be executed in s
	// each action is applicable in given_state
	std::vector<Action> ACTIONS(GameState* given_state);

	// returns the state that results from doing action in given_state
	// this function assumes the action is valid 
	GameState* RESULT(GameState* given_state, Action action);

	// determines whether a state has been visited before
	bool visited(std::vector<GameState*> states, GameState* state);
	bool is_free(GameState* state, int index);

	// functions for executing action list
	GameState* Execute(int move_delta, GameState* given_state, bool isPulling);

	// functions for building action list
	void Move(int move_delta, GameState* given_state, std::vector<Action>& possible_actions);
	std::vector<Action> moveNorth(GameState* given_state);
	std::vector<Action> moveSouth(GameState* given_state);
	std::vector<Action> moveEast(GameState* given_state);
	std::vector<Action> moveWest(GameState* given_state);

	std::map<int, std::vector<Action>> m_action_map;
	std::vector<std::string> m_actionList;

	GameState* m_initialState;
	GameState* m_goalState;
	std::vector<std::pair<GameState*, Action>> m_path;

	// this is very verbose, but it is necessary for
	// the way i want the algorithms to work.
	// for serial execution, i want to be able to
	// maintain a unique fringe for each algorithm
	// and for multi-thread the DS need to be separate
	std::vector<GameState*> m_visitedStates;
	std::vector<GameState*> m_BFS_visitedStates;
	std::vector<GameState*> m_DFS_visitedStates;
	std::vector<GameState*> m_AStar_visitedStates;
	std::vector<GameState*> m_Manhattan_visitedStates;

	FIFO<GameState> m_BFS_fringe;
	LIFO<GameState> m_DFS_fringe;
	PriorityQueue<GameState> m_manhattan_fringe;
	PriorityQueue<GameState> m_my_heuristic_fringe;
	PriorityQueue<GameState> m_greedy_fringe;
	PriorityQueue<GameState> m_uniform_cost_fringe;
	PriorityQueue<GameState> m_AStar_fringe;
};

/**
 * @brief Writes the given data to a file.
 *
 * This template function writes a single item of any type to a specified file, with two possible write modes:
 * - Append mode (openMode=0)
 * - Truncate mode (openMode=1)
 *
 * @tparam T The type of the data to write to the file. The type must support the insertion operator (operator<<).
 * @param filename The name of the file (with path if needed) where the data is to be written.
 * @param data The data item to write to the file.
 * @param openMode The mode to open the file. 0 represents append mode, and 1 represents truncate mode.
 */
template <typename T>
void WriteToFile(std::string filename, T data, int openMode) {

	std::ofstream outfile;

	switch (openMode) {
	case 0:
		outfile.open(filename, std::ios_base::app);
		break;
	case 1:
		outfile.open(filename, std::ios_base::trunc);
		break;
	}

	if (!outfile.is_open()) {
		std::cout << filename << " WAS UNABLE TO OPEN. " << std::endl;
	}

	outfile << data << '\n';
	outfile.close();
}