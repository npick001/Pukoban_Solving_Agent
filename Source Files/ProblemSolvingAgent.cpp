#include "ProblemSolvingAgent.h"

ProblemSolvingAgent::ProblemSolvingAgent(GameState* initial_state)
{
	m_isPushing = false;
	m_isPulling = false;

	m_myPosition = initial_state->robot_position;
	m_targetBoxPosition = -1;

	m_initialState = initial_state;
	m_goalState = formulateGoalState(initial_state);

	m_fringe = Fringe<GameState>(initial_state);
}

ProblemSolvingAgent::~ProblemSolvingAgent()
{
	
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

	// fringe is a stack for DFS
	//for (int i = 0; i < next_states.size(); i++) {
	//}

	// fringe is a queue for BFS
	for (int i = 0; i < init_next_states.size(); i++) {
		m_fringe.push(init_next_states[i]);
	}
	bool goal = false;
	while (!goal) {

		GameState* current_state = m_fringe.pop();

		goal = goalTest(current_state);
		if (goal) {
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		for (int i = 0; i < next_states.size(); i++) {

			// check if state has already been visited
			if (!visited(next_states[i])) {
				m_fringe.push(next_states[i]);
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
	GameState* new_state = new GameState(state);
	int move_delta = 0;

	// currently only moving the robot,
	// need to add the box movement
	// and check if the move is valid
	switch (action) {
	case Action::Push_North:
		move_delta = -new_state->board_width;
		break;
	case Action::Push_South:
		move_delta = new_state->board_width;
		break;
	case Action::Push_East:
		move_delta = 1;
		break;
	case Action::Push_West:
		move_delta = -1;
		break;
	case Action::Pull_North:
		move_delta = -new_state->board_width;
		break;
	case Action::Pull_South:
		move_delta = new_state->board_width;
		break;
	case Action::Pull_East:
		move_delta = 1;
		break;
	case Action::Pull_West:
		move_delta = -1;
		break;
	}

	// move the robot
	new_state->robot_position += move_delta;

	// check if theres a box in the movement direction
	for (int i = 0; i < new_state->box_positions.size(); i++) {
		if (new_state->box_positions[i] == new_state->robot_position) {
			// move the box
			int loc_check = new_state->robot_position + (2 * move_delta);
			if (new_state->board[loc_check] != Board::CellValues::Obstacle) {
				new_state->box_positions[i] += move_delta;
			}
			break;
		}
	}

	return new_state;
}

std::vector<Action> ProblemSolvingAgent::ACTIONS(GameState* state)
{
	// this is where we add the rules for the specific problem
	std::vector<Action> executable_actions;

	// where are we moving to
	int north_delta = -state->board_width;
	int south_delta = state->board_width;
	int east_delta = 1;
	int west_delta = -1;

	// check if movement spot is blank
	// if its blank then its a valid action automatically
	if (state->board[state->robot_position + north_delta] == Board::CellValues::Blank) {
		executable_actions.push_back(Action::Push_North);
	}
	if (state->board[state->robot_position + south_delta] == Board::CellValues::Blank) {
		executable_actions.push_back(Action::Push_South);
	}
	if (state->board[state->robot_position + east_delta] == Board::CellValues::Blank) {
		executable_actions.push_back(Action::Push_East);
	}
	if (state->board[state->robot_position + west_delta] == Board::CellValues::Blank) {
		executable_actions.push_back(Action::Push_West);
	}


	// check if theres a box in the movement direction
	// and if that can move, then we can push that direction
	for (int i = 0; i < state->box_positions.size(); i++) {

		// check if robot is moving north and box is to the north
		if ((state->box_positions[i] == (state->robot_position + north_delta))
			&& (state->board[state->robot_position + (2 * north_delta)] == Board::CellValues::Blank)) {
			executable_actions.push_back(Action::Push_North);
		}
		// check if robot is moving north and box is to the south
		if ((state->box_positions[i] == (state->robot_position + south_delta))
			&& (state->board[state->robot_position + north_delta] == Board::CellValues::Blank)) {
			executable_actions.push_back(Action::Pull_North);
		}
		if ((state->box_positions[i] == (state->robot_position + east_delta))
			&& (state->board[state->robot_position + (2 * east_delta)] == Board::CellValues::Blank)) {
			executable_actions.push_back(Action::Push_East);
		}
		if ((state->box_positions[i] == (state->robot_position + west_delta))
			&& (state->board[state->robot_position + east_delta] == Board::CellValues::Blank)) {
			executable_actions.push_back(Action::Pull_East);
		}
		if ((state->box_positions[i] == (state->robot_position + south_delta))
			&& (state->board[state->robot_position + (2 * south_delta)] == Board::CellValues::Blank)) {
			executable_actions.push_back(Action::Push_South);
		}
		if ((state->box_positions[i] == (state->robot_position + north_delta))
			&& (state->board[state->robot_position + south_delta] == Board::CellValues::Blank)) {
			executable_actions.push_back(Action::Pull_South);
		}
		if ((state->box_positions[i] == (state->robot_position + west_delta))
			&& (state->board[state->robot_position + (2 * west_delta)] == Board::CellValues::Blank)) {
			executable_actions.push_back(Action::Push_West);
		}
		if ((state->box_positions[i] == (state->robot_position + east_delta))
			&& (state->board[state->robot_position + west_delta] == Board::CellValues::Blank)) {
			executable_actions.push_back(Action::Pull_West);
		}
	}


	// check if we can move south

	// check if we can move east

	// check if we can move west

	return executable_actions;
}

bool ProblemSolvingAgent::is_valid(GameState* state)
{



	return false;
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


