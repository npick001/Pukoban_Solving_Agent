#pragma once
#include "Board.h"
class GameState;

enum SearchAlgorithm {
	Breadth_First = 0,
	Depth_First = 1,
	Greedy = 2,
	A_Star = 3
};

// {action, gamestate} form state space {arc, node}
enum class Action {
	Push_North,
	Push_South,
	Push_East,
	Push_West,

	Pull_North,
	Pull_South,
	Pull_East,
	Pull_West,
};

class ProblemSolvingAgent
{
public:
	ProblemSolvingAgent(GameState* initial_state);
	~ProblemSolvingAgent();

	void setSearchAlgorithm(SearchAlgorithm algorithm);

protected:
	GameState* formulateGoalState(GameState* initial);

	/*
		MUST BE CALLED TO START THE STATE GENERATION
		Highest level function to generate all states
		and connect them into the state space.

		The state space can then be searched later with 
		different searching algorithms.

		State space is represented through pointers between
		states.
	*/
	void generateStateSpace();

	// successor function
	// return all valid game states from given game state 
	std::vector<GameState*> successor(GameState* given_state);
	bool goalTest(GameState* given_state);

	GameState* MoveNorth(GameState* given_state);
	GameState* MoveSouth(GameState* given_state);
	GameState* MoveEast(GameState* given_state);
	GameState* MoveWest(GameState* given_state);

	void move(); 
private:
	// returns the state that results from doing action in given_state
	GameState* RESULT(GameState* given_state, Action action);
	
	// returns the set of actions that can be executed in s
	// each action is applicable in given_state
	std::vector<Action> ACTIONS(GameState* given_state);

	bool visited(GameState* state);

	bool m_isPushing;
	bool m_isPulling;

	int m_myPosition;
	int m_targetBoxPosition;

	GameState* m_initialState;
	GameState* m_goalState;
	std::vector<Action> m_solution;
	SearchAlgorithm m_searchingAlgorithm;

	std::vector<GameState*> m_visitedStates;
	std::vector<GameState*> m_notVisitedStates;

};

