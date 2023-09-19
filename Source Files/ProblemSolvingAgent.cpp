#include "ProblemSolvingAgent.h"

ProblemSolvingAgent::ProblemSolvingAgent(GameState* initial_state)
{
	m_isPushing = false;
	m_isPulling = false;

	m_myPosition = initial_state->robot_position;
	m_targetBoxPosition = -1;

	m_initialState = initial_state;
	m_goalState = formulateGoalState(initial_state);

	m_searchingAlgorithm = Breadth_First;
}

ProblemSolvingAgent::~ProblemSolvingAgent()
{

}

void ProblemSolvingAgent::setSearchAlgorithm(SearchAlgorithm algorithm)
{
	switch (algorithm) {
	case Breadth_First:
		break;

	case Depth_First:
		break;

	case Greedy:
		break;

	case A_Star:
		break;
	}
}

void ProblemSolvingAgent::move()
{

}

GameState* ProblemSolvingAgent::formulateGoalState(GameState* initial)
{
	GameState* goal_state = new GameState();

	// load the static pieces
	for (int i = 0; i < initial->board.size(); i++) {
		goal_state->board.push_back(initial->board[i]);
	}

	// find the storage locations, boxes should be on top of storages when complete
	for (int i = 0; i < initial->storage_positions.size(); i++) {
		goal_state->box_positions.push_back(initial->storage_positions[i]);
	}

	// robot can be anywhere
	return goal_state;
}

void ProblemSolvingAgent::generateStateSpace()
{
	std::vector<GameState*> init_next_states = successor(m_initialState);
	m_visitedStates.push_back(m_initialState);

	// if DFS, add to front of vector
	//for (int i = 0; i < next_states.size(); i++) {
	//}

	// BFS is back of vector
	for (int i = 0; i < init_next_states.size(); i++) {
		m_notVisitedStates.push_back(init_next_states[i]);
	}
	bool goal = false;
	while (!goal) {

		GameState* current_state = m_notVisitedStates.front();
		m_notVisitedStates.pop_back();

		goal = goalTest(current_state);
		if (goal) {
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		for (int i = 0; i < next_states.size(); i++) {

			// check if state has already been visited
			if (!visited(next_states[i])) {
				m_notVisitedStates.push_back(next_states[i]);
			}
		}

		m_visitedStates.push_back(current_state);
	}

	// heuristic adds based on cost/weight
}

std::vector<GameState*> ProblemSolvingAgent::successor(GameState* given_state)
{
	std::vector<GameState*> reachable_states;
	std::vector<Action> applicable_actions = ACTIONS(given_state);

	// generate all reachable states from the applicable actions
	for (int i = 0; i < applicable_actions.size(); i++) {
		GameState* new_state = RESULT(given_state, applicable_actions[i]);
		new_state->SetAction(applicable_actions[i]);
		new_state->SetParent(given_state);
		reachable_states.push_back(new_state);
	}

	return reachable_states;
}

bool ProblemSolvingAgent::goalTest(GameState* state)
{
	// load the static pieces
	for (int i = 0; i < state->board.size(); i++) {
		if (state->board[i] != m_goalState->board[i]) {
			return false;
		}
	}

	// find the storage locations, boxes should be on top of storages when complete
	for (int i = 0; i < state->storage_positions.size(); i++) {
		if (state->box_positions[i] != m_goalState->box_positions[i]) {
			return false;
		}
	}

	// if we make it here then we are at goal state
	return true;
}

GameState* ProblemSolvingAgent::MoveNorth(GameState* state)
{
	return nullptr;
}

GameState* ProblemSolvingAgent::MoveSouth(GameState* state)
{
	return nullptr;
}

GameState* ProblemSolvingAgent::MoveEast(GameState* state)
{
	return nullptr;
}

GameState* ProblemSolvingAgent::MoveWest(GameState* state)
{
	return nullptr;
}

GameState* ProblemSolvingAgent::RESULT(GameState* state, Action action)
{
	return new GameState();
}

std::vector<Action> ProblemSolvingAgent::ACTIONS(GameState* state)
{
	std::vector<Action> executable_actions;

	return executable_actions;
}

bool ProblemSolvingAgent::visited(GameState* state)
{
	// check through the visited states and see if given state is in there
	for (int i = 0; i < m_visitedStates.size(); i++) {
		if (m_visitedStates[i] == state) {
			return true;
		}
	}

	return false;
}


