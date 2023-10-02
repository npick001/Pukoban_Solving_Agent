#include "ProblemSolvingAgent.h"

ProblemSolvingAgent::ProblemSolvingAgent(GameState* initial_state) : m_BFS_fringe()
{
	m_initialState = initial_state;
	m_goalState = formulateGoalState(initial_state);

	m_actionList = { "Push_North", "Push_South", "Push_East", "Push_West",
			"Pull_North", "Pull_South", "Pull_East", "Pull_West" };

	m_action_map = {
		{-m_initialState->board_width, {Action::Push_North, Action::Pull_North}},
		{m_initialState->board_width, {Action::Push_South, Action::Pull_South}},
		{1, {Action::Push_East, Action::Pull_East}},
		{-1, {Action::Push_West, Action::Pull_West}}
	};
}

ProblemSolvingAgent::~ProblemSolvingAgent()
{

}

void ProblemSolvingAgent::Solve()
{

#if MULTI_THREADED_SOLVE
	MultiThreadSolve();
#endif

#if A_STAR_SEARCH
	generate_AStar_StateSpace(m_initialState);
#endif // A_STAR_SEARCH

#if BREADTH_FIRST_SEARCH
	generate_BFS_StateSpace(m_initialState);
#endif // BREADTH_FIRST_SEARCH

#if DEPTH_FIRST_SEARCH
	generate_DFS_StateSpace(m_initialState);
#endif // DEPTH_FIRST_SEARCH

#if MANHATTAN_HEURISTIC
	generate_Manhattan_StateSpace(m_initialState);
#endif // MANHATTAN_HEURISTIC

#if MY_HEURISTIC
	generate_MyHeuristic_StateSpace(m_initialState);
#endif // MY_HEURISTIC

#if UNIFORM_COST_SEARCH
	generate_UniformCost_StateSpace(m_initialState);
#endif // UNIFORM_COST_SEARCH

#if GREEDY_SEARCH
	generate_Greedy_StateSpace(m_initialState);
#endif // GREEDY_BEST_FIRST_SEARCH

}

void ProblemSolvingAgent::MultiThreadSolve()
{
	int num_threads = 4;
	std::thread* threads = new std::thread[num_threads];

	threads[0] = std::thread(&ProblemSolvingAgent::generate_AStar_StateSpace, this, m_initialState);
	threads[1] = std::thread(&ProblemSolvingAgent::generate_BFS_StateSpace, this, m_initialState);
	threads[2] = std::thread(&ProblemSolvingAgent::generate_DFS_StateSpace, this, m_initialState);
	threads[3] = std::thread(&ProblemSolvingAgent::generate_Manhattan_StateSpace, this, m_initialState);

	for(int i = 0; i < num_threads; i++)
	{
		threads[i].join();
	}
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
		goal_state->storage_positions.push_back(initial->storage_positions[i]);
	}

	// robot can be anywhere
	return goal_state;
}

void ProblemSolvingAgent::generate_BFS_StateSpace(GameState* initial)
{
	int num_states_visited = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto start_state = new GameState(*initial);

	m_BFS_fringe.insert(start_state);

	bool goal = false;
	while (!goal) {

		GameState* current_state = m_BFS_fringe.extract();

		goal = goalTest(current_state);
		num_states_visited++;

		if (goal) {
			m_goalState = current_state;
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		m_BFS_visitedStates.push_back(current_state);
		for (int i = 0; i < next_states.size(); i++) {

			// check if state has already been visited
			if (!visited(m_BFS_visitedStates, next_states[i]) && !m_BFS_fringe.contains(next_states[i])) {
				m_BFS_fringe.insert(next_states[i]);
			}
		}
	}

	// generate the path from the goal state to the initial state
#if PRINT_SOLUTION_PATH
	GameState* current_state = m_goalState;
	std::cout << "Solution path:" << std::endl;
	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));
		
		std::cout << m_actionList[static_cast<int>(current_state->GetAction())] << '\n';
		std::cout << "Cost from start: " << std::to_string(current_state->GetPathCost()) << '\n';
		std::cout << '\n';
		current_state->print();

		current_state = (GameState*)current_state->GetParent();
	}
#endif // PRINT_SOLUTION_PATH

	auto end_time = std::chrono::high_resolution_clock::now();

#if PRINT_SOLUTION_PATH
	// print the path
	for (int i = m_path.size() - 1; i >= 0; i--) {
		std::cout << m_actionList[static_cast<int>(m_path[i].second)] << '\n';
		m_path[i].first->print();
	}
#endif // PRINT_SOLUTION_PATH

#if WRITE_SOLUTION_PATH_TO_FILE

	GameState* current_state = m_goalState;
	std::string bfs_filename = ".\\Output Files\\BFS_solution.txt";
	WriteToFile(bfs_filename, "Solution path:", 1);

	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));
		current_state = (GameState*)current_state->GetParent();
	}

	// after building the path, write to file
	for (int i = m_path.size() - 1; i >= 0; i--) {
		WriteToFile(bfs_filename, m_actionList[static_cast<int>(m_path[i].second)], 0);
		WriteToFile(bfs_filename, m_path[i].first->to_string(), 0);
	}

#endif // WRITE_SOLUTION_PATH_TO_FILE

	std::cout << "Number of states visited in BFS search: " << num_states_visited << '\n';
	std::cout << "Number of states visited by the goal state: " << m_goalState->GetNumberOfMoves() << '\n';

	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n\n";
}

void ProblemSolvingAgent::generate_DFS_StateSpace(GameState* initial)
{
	int num_states_visited = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto start_state = new GameState(*initial);

	m_DFS_fringe.insert(start_state);

	bool goal = false;
	while (!goal) {

		GameState* current_state = m_DFS_fringe.extract();

		if (visited(m_DFS_visitedStates, current_state)) {
			delete current_state;
			current_state = m_DFS_fringe.extract();
		}

		goal = goalTest(current_state);
		num_states_visited++;

		if (goal) {
			m_goalState = current_state;
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		m_DFS_visitedStates.push_back(current_state);
		for (int i = 0; i < next_states.size(); i++) {

			// check if state has already been visited
			if (!visited(m_DFS_visitedStates, next_states[i])) {
				m_DFS_fringe.insert(next_states[i]);
			}
		}
	}

	// generate the path from the goal state to the initial state
#if PRINT_SOLUTION_PATH
	GameState* current_state = m_goalState;
	std::cout << "Solution path:" << std::endl;
	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));

		std::cout << m_actionList[static_cast<int>(current_state->GetAction())] << '\n';
		std::cout << "Cost from start: " << std::to_string(current_state->GetPathCost()) << '\n';
		std::cout << '\n';
		current_state->print();

		current_state = (GameState*)current_state->GetParent();
	}
#endif // PRINT_SOLUTION_PATH

	auto end_time = std::chrono::high_resolution_clock::now();

#if PRINT_SOLUTION_PATH
	// print the path
	for (int i = m_path.size() - 1; i >= 0; i--) {
		std::cout << m_actionList[static_cast<int>(m_path[i].second)] << '\n';
		m_path[i].first->print();
	}
#endif // PRINT_SOLUTION_PATH

#if WRITE_SOLUTION_PATH_TO_FILE

	GameState* current_state = m_goalState;
	std::string dfs_filename = ".\\Output Files\\DFS_solution.txt";
	WriteToFile(dfs_filename, "Solution path:", 1);

	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));
		current_state = (GameState*)current_state->GetParent();
	}

	// after building the path, write to file
	for (int i = m_path.size() - 1; i >= 0; i--) {
		WriteToFile(dfs_filename, m_actionList[static_cast<int>(m_path[i].second)], 0);
		WriteToFile(dfs_filename, m_path[i].first->to_string(), 0);
	}

#endif // WRITE_SOLUTION_PATH_TO_FILE

	std::cout << "Number of states visited in DFS search: " << num_states_visited << '\n';
	std::cout << "Number of states visited by the goal state: " << m_goalState->GetNumberOfMoves() << '\n';

	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n\n";
}

void ProblemSolvingAgent::generate_Manhattan_StateSpace(GameState* initial)
{
	int num_states_visited = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto start_state = new GameState(*initial);

	start_state->SetManhattanDistance(manhattan_heuristic(start_state));
	m_manhattan_fringe.insert(start_state->GetManhattanDistance(), start_state);

	bool goal = false;
	while (!goal) {

		GameState* current_state = (GameState*)m_manhattan_fringe.extract();

		if (visited(m_Manhattan_visitedStates, current_state)) {
			current_state = (GameState*)m_manhattan_fringe.extract();
			//std::cout << "Fringe duplicate detected" << std::endl;
		}

		goal = goalTest(current_state);
		num_states_visited++;

		if (goal) {
			m_goalState = current_state;
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		m_Manhattan_visitedStates.push_back(current_state);
		for (int i = 0; i < next_states.size(); i++) {

			// check if state hasnt been visited
			if (!visited(m_Manhattan_visitedStates, next_states[i])) {
				int manhattan_distance = manhattan_heuristic(next_states[i]);

				next_states[i]->SetManhattanDistance(manhattan_distance);

				m_manhattan_fringe.insert(manhattan_distance, next_states[i]);
			}
		}
	}

	// generate the path from the goal state to the initial state
#if PRINT_SOLUTION_PATH
	GameState* current_state = m_goalState;
	std::cout << "Solution path:" << std::endl;
	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));

		std::cout << m_actionList[static_cast<int>(current_state->GetAction())] << '\n';
		std::cout << "Cost from start: " << std::to_string(current_state->GetPathCost()) << '\n';
		std::cout << '\n';
		current_state->print();

		current_state = (GameState*)current_state->GetParent();
	}
#endif // PRINT_SOLUTION_PATH

	auto end_time = std::chrono::high_resolution_clock::now();

#if PRINT_SOLUTION_PATH
	// print the path
	for (int i = m_path.size() - 1; i >= 0; i--) {
		std::cout << m_actionList[static_cast<int>(m_path[i].second)] << '\n';
		m_path[i].first->print();
	}
#endif // PRINT_SOLUTION_PATH

#if WRITE_SOLUTION_PATH_TO_FILE
	GameState* current_state = m_goalState;
	std::string manhattanH_filename = ".\\Output Files\\ManhattanHeuristic_solution.txt";
	WriteToFile(manhattanH_filename, "Solution path:", 1);

	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));
		current_state = (GameState*)current_state->GetParent();
	}

	// after building the path, write to file
	for (int i = m_path.size() - 1; i >= 0; i--) {
		WriteToFile(manhattanH_filename, m_actionList[static_cast<int>(m_path[i].second)], 0);
		WriteToFile(manhattanH_filename, m_path[i].first->to_string(), 0);
	}
#endif // WRITE_SOLUTION_PATH_TO_FILE

	std::cout << "Number of states visited in manhatten heuristic informed search: " << num_states_visited << '\n';
	std::cout << "Number of states visited by the goal state: " << m_goalState->GetNumberOfMoves() << '\n';

	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n\n";
}

void ProblemSolvingAgent::generate_MyHeuristic_StateSpace(GameState* initial)
{
	int num_states_visited = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto start_state = new GameState(*initial);

	start_state->SetHeuristicValue(heuristic(start_state));
	m_my_heuristic_fringe.insert(start_state->GetHeuristicValue(), start_state);

	bool goal = false;
	while (!goal) {

		GameState* current_state = (GameState*)m_my_heuristic_fringe.extract();

		if (visited(m_visitedStates, current_state)) {
			current_state = (GameState*)m_my_heuristic_fringe.extract();
			//std::cout << "Fringe duplicate detected" << std::endl;
		}

		goal = goalTest(current_state);
		num_states_visited++;

		if (goal) {
			m_goalState = current_state;
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		m_visitedStates.push_back(current_state);
		for (int i = 0; i < next_states.size(); i++) {

			// check if state hasnt been visited
			if (!visited(m_visitedStates, next_states[i])) {
				int heuristic_value = heuristic(next_states[i]);

				next_states[i]->SetHeuristicValue(heuristic_value);

				m_my_heuristic_fringe.insert(heuristic_value, next_states[i]);
			}
		}
	}

	// generate the path from the goal state to the initial state
#if PRINT_SOLUTION_PATH
	GameState* current_state = m_goalState;
	std::cout << "Solution path:" << std::endl;
	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));

		std::cout << m_actionList[static_cast<int>(current_state->GetAction())] << '\n';
		std::cout << "Cost from start: " << std::to_string(current_state->GetPathCost()) << '\n';
		std::cout << '\n';
		current_state->print();

		current_state = (GameState*)current_state->GetParent();
	}
#endif // PRINT_SOLUTION_PATH

	auto end_time = std::chrono::high_resolution_clock::now();

#if PRINT_SOLUTION_PATH
	// print the path
	for (int i = m_path.size() - 1; i >= 0; i--) {
		std::cout << m_actionList[static_cast<int>(m_path[i].second)] << '\n';
		m_path[i].first->print();
	}
#endif // PRINT_SOLUTION_PATH

#if WRITE_SOLUTION_PATH_TO_FILE
	GameState* current_state = m_goalState;
	std::string heuristic_filename = ".\\Output Files\\Heuristic_solution.txt";
	WriteToFile(heuristic_filename, "Solution path:", 1);

	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));
		current_state = (GameState*)current_state->GetParent();
	}

	// after building the path, write to file
	for (int i = m_path.size() - 1; i >= 0; i--) {
		WriteToFile(heuristic_filename, m_actionList[static_cast<int>(m_path[i].second)], 0);
		WriteToFile(heuristic_filename, m_path[i].first->to_string(), 0);
	}
#endif // WRITE_SOLUTION_PATH_TO_FILE

	std::cout << "Number of states visited in my heuristic informed search: " << num_states_visited << '\n';
	std::cout << "Number of states visited by the goal state: " << m_goalState->GetNumberOfMoves() << '\n';

	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n\n";

	// reset all data structures
	m_visitedStates.clear();
}

void ProblemSolvingAgent::generate_AStar_StateSpace(GameState* initial)
{
	int num_states_visited = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto start_state = new GameState(*initial);

	start_state->SetHeuristicValue(manhattan_heuristic(start_state));
	m_AStar_fringe.insert(0, start_state);

	bool goal = false;
	while (!goal) {

		GameState* current_state = (GameState*)m_AStar_fringe.extract();

		if (visited(m_AStar_visitedStates, current_state)) {
			current_state = (GameState*)m_AStar_fringe.extract();
			//std::cout << "Fringe duplicate detected" << std::endl;
		}

		goal = goalTest(current_state);
		num_states_visited++;

		if (goal) {
			m_goalState = current_state;
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		m_AStar_visitedStates.push_back(current_state);

		for (int i = 0; i < next_states.size(); i++) {

			// check if state hasnt been visited
			if (!visited(m_AStar_visitedStates, next_states[i])) {
				int h_n = manhattan_heuristic(next_states[i]);
				next_states[i]->SetHeuristicValue(h_n);

				int g_n = next_states[i]->GetNumberOfMoves();
				int AStar_value = g_n + h_n;

				m_AStar_fringe.insert(AStar_value, next_states[i]);
			}
		}
	}

	// generate the path from the goal state to the initial state
#if PRINT_SOLUTION_PATH
	GameState* current_state = m_goalState;
	std::cout << "Solution path:" << std::endl;
	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));

		std::cout << m_actionList[static_cast<int>(current_state->GetAction())] << '\n';
		std::cout << "Cost from start: " << std::to_string(current_state->GetPathCost()) << '\n';
		std::cout << '\n';
		current_state->print();

		current_state = (GameState*)current_state->GetParent();
	}
#endif // PRINT_SOLUTION_PATH

	auto end_time = std::chrono::high_resolution_clock::now();

#if PRINT_SOLUTION_PATH
	// print the path
	for (int i = m_path.size() - 1; i >= 0; i--) {
		std::cout << m_actionList[static_cast<int>(m_path[i].second)] << '\n';
		m_path[i].first->print();
	}
#endif // PRINT_SOLUTION_PATH

#if WRITE_SOLUTION_PATH_TO_FILE
	GameState* current_state = m_goalState;
	std::string AStar_filename = ".\\Output Files\\AStar_solution.txt";
	WriteToFile(AStar_filename, "Solution path:", 1);

	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));
		current_state = (GameState*)current_state->GetParent();
	}

	// after building the path, write to file
	for (int i = m_path.size() - 1; i >= 0; i--) {
		WriteToFile(AStar_filename, m_actionList[static_cast<int>(m_path[i].second)], 0);
		WriteToFile(AStar_filename, m_path[i].first->to_string(), 0);
	}
#endif // WRITE_SOLUTION_PATH_TO_FILE

	std::cout << "Number of states visited in A* informed search: " << num_states_visited << '\n';
	std::cout << "Number of states visited by the goal state: " << m_goalState->GetNumberOfMoves() << '\n';

	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n\n";

	// reset all data structures
	m_visitedStates.clear();
}

void ProblemSolvingAgent::generate_UniformCost_StateSpace(GameState* initial)
{
	int num_states_visited = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto start_state = new GameState(*initial);

	m_uniform_cost_fringe.insert(0, start_state);

	bool goal = false;
	while (!goal) {

		GameState* current_state = (GameState*)m_uniform_cost_fringe.extract();

		if (visited(m_visitedStates, current_state)) {
			current_state = (GameState*)m_uniform_cost_fringe.extract();
			//std::cout << "Fringe duplicate detected" << std::endl;
		}

		goal = goalTest(current_state);
		num_states_visited++;

		if (goal) {
			m_goalState = current_state;
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		m_visitedStates.push_back(current_state);
		for (int i = 0; i < next_states.size(); i++) {

			// check if state hasnt been visited
			if (!visited(m_visitedStates, next_states[i])) {
				int path_cost = next_states[i]->GetNumberOfMoves();

				m_uniform_cost_fringe.insert(path_cost, next_states[i]);
			}
		}
	}

	// generate the path from the goal state to the initial state
#if PRINT_SOLUTION_PATH
	GameState* current_state = m_goalState;
	std::cout << "Solution path:" << std::endl;
	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));

		std::cout << m_actionList[static_cast<int>(current_state->GetAction())] << '\n';
		std::cout << "Cost from start: " << std::to_string(current_state->GetPathCost()) << '\n';
		std::cout << '\n';
		current_state->print();

		current_state = (GameState*)current_state->GetParent();
	}
#endif // PRINT_SOLUTION_PATH

	auto end_time = std::chrono::high_resolution_clock::now();

#if PRINT_SOLUTION_PATH
	// print the path
	for (int i = m_path.size() - 1; i >= 0; i--) {
		std::cout << m_actionList[static_cast<int>(m_path[i].second)] << '\n';
		m_path[i].first->print();
	}
#endif // PRINT_SOLUTION_PATH

#if WRITE_SOLUTION_PATH_TO_FILE
	GameState* current_state = m_goalState;
	std::string uniformCost_filename = ".\\Output Files\\UniformCost_solution.txt";
	WriteToFile(uniformCost_filename, "Solution path:", 1);

	while (current_state != start_state) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));
		current_state = (GameState*)current_state->GetParent();
	}

	// after building the path, write to file
	for (int i = m_path.size() - 1; i >= 0; i--) {
		WriteToFile(uniformCost_filename, m_actionList[static_cast<int>(m_path[i].second)], 0);
		WriteToFile(uniformCost_filename, m_path[i].first->to_string(), 0);
	}
#endif // WRITE_SOLUTION_PATH_TO_FILE

	std::cout << "Number of states visited in uniform-cost informed search: " << num_states_visited << '\n';
	std::cout << "Number of states visited by the goal state: " << m_goalState->GetNumberOfMoves() << '\n';

	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n\n";

	// reset all data structures
	m_visitedStates.clear();
}

void ProblemSolvingAgent::generate_Greedy_StateSpace(GameState* initial)
{
	int num_states_visited = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto start_state = new GameState(*initial);

	start_state->SetHeuristicValue(heuristic(start_state));
	m_greedy_fringe.insert(start_state->GetHeuristicValue(), start_state);

	bool goal = false;
	while (!goal) {

		GameState* current_state = (GameState*)m_greedy_fringe.extract();

		goal = goalTest(current_state);
		num_states_visited++;

		if (goal) {
			m_goalState = current_state;
			break;
		}

		std::vector<GameState*> next_states = successor(current_state);
		m_visitedStates.push_back(current_state);

		// only select the smallest heuristic value
		// to add to the fringe
		// for greedy-best first search
		int min_value = INT_MAX;
		for (int i = 0; i < next_states.size(); i++) {

			// take the minimum of the next_states heuristic_value
			int heuristic_value = heuristic(next_states[i]);
			next_states[i]->SetHeuristicValue(heuristic_value);

			if (heuristic_value < min_value) {
				min_value = heuristic_value;
			}
		}

		// only insert the smallest one
		for (int i = 0; i < next_states.size(); i++) {
			if (min_value == next_states[i]->GetHeuristicValue()) {
				m_greedy_fringe.insert(min_value, next_states[i]);
			}
		}
	}

	// generate the path from the goal state to the initial state
#if PRINT_SOLUTION_PATH
	GameState* current_state = m_goalState;
	std::cout << "Solution path:" << std::endl;

	m_path.clear();
	while (current_state != m_initialState) {
		m_path.push_back(std::make_pair(current_state, current_state->GetAction()));

		//std::cout << m_actionList[static_cast<int>(current_state->GetAction())] << '\n';
		//std::cout << '\n';
		//current_state->print();

		current_state = (GameState*)current_state->GetParent();
	}
#endif // PRINT_SOLUTION_PATH

	auto end_time = std::chrono::high_resolution_clock::now();

	// print the path
	for (int i = m_path.size() - 1; i >= 0; i--) {
		std::cout << m_actionList[static_cast<int>(m_path[i].second)] << '\n';
		m_path[i].first->print();
	}
	std::cout << "Number of states visited in greedy best-first search: " << num_states_visited << '\n';
	std::cout << "Number of states visited by the goal state: " << m_goalState->GetNumberOfMoves() << '\n';

	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n\n";

	// reset all data structures
	m_visitedStates.clear();
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
	// find the storage locations, boxes should be on top of storages when complete
	for (int i = 0; i < state->storage_positions.size(); i++) {
		bool boxMatched = false;
		for (int j = 0; j < state->box_positions.size(); j++) {
			if (state->storage_positions[i] == state->box_positions[j]) {
				boxMatched = true;
				break;
			}
		}
		
		if (!boxMatched) {
			return false;
		}
	}

	// if we make it here then we are at goal state
	return true;
}

int ProblemSolvingAgent::manhattan_heuristic(GameState* given_state)
{
	// calculate the manhattan distance between all boxes and storages and sum
	int manhattan_distance = 0;

	for (int i = 0; i < given_state->box_positions.size(); i++) {
		int min_distance = INT_MAX;
		int box_x = given_state->box_positions[i] % given_state->board_width;
		int box_y = given_state->box_positions[i] / given_state->board_width;

		// for each movable box find the closest storage
		for (int j = 0; j < given_state->storage_positions.size(); j++) {
			int storage_x = given_state->storage_positions[j] % given_state->board_width;
			int storage_y = given_state->storage_positions[j] / given_state->board_width;

			// find the smallest manhattan distance from each box to the closest storage
			if (abs(box_x - storage_x) + abs(box_y - storage_y) < min_distance) {
				min_distance = abs(box_x - storage_x) + abs(box_y - storage_y);
			}
		}
		manhattan_distance += min_distance;
	}

	return manhattan_distance;
}

int ProblemSolvingAgent::heuristic(GameState* given_state)
{
	// this heuristic is the sum of boxes from all storages 
	// minus the storage distances from each other
	int heuristic_value = 0;

	int storage_distances = 0;

	// calculate the distances between all storages
	for (int i = 1; i < given_state->storage_positions.size(); i++) {

		int storage1_x = given_state->storage_positions[i - 1] % given_state->board_width;
		int storage1_y = given_state->storage_positions[i - 1] / given_state->board_width;

		int storage2_x = given_state->storage_positions[i] % given_state->board_width;
		int storage2_y = given_state->storage_positions[i] / given_state->board_width;

		storage_distances += abs(storage1_x - storage2_x) + abs(storage1_y - storage2_y);
	}

	// calculate the sum of the distances from all boxes to all storages
	int total_distance = 0;
	for (int i = 0; i < given_state->box_positions.size(); i++) {
		int box_x = given_state->box_positions[i] % given_state->board_width;
		int box_y = given_state->box_positions[i] / given_state->board_width;

		// for each movable box find the closest storage
		for (int j = 0; j < given_state->storage_positions.size(); j++) {
			int distance = 0;
			int storage_x = given_state->storage_positions[j] % given_state->board_width;
			int storage_y = given_state->storage_positions[j] / given_state->board_width;

			distance = abs(box_x - storage_x) + abs(box_y - storage_y);
		}
	}

	heuristic_value = total_distance - storage_distances;

	return heuristic_value;
}

void ProblemSolvingAgent::Move(int move_delta, GameState* given_state, std::vector<Action>& possible_actions)
{
	if ((given_state->robot_position + move_delta) > given_state->board.size() - 1
		|| (given_state->robot_position + move_delta) < 0
		|| (given_state->robot_position + 2 * move_delta) > given_state->board.size() - 1
		|| (given_state->robot_position + 2 * move_delta) < 0) {
		return;
	}

	for (int box = 0; box < given_state->box_positions.size(); box++) {
		
		// check front and back
		bool box_in_front = (given_state->robot_position + move_delta) == given_state->box_positions[box];
		bool dbl_forward_open = is_free(given_state, given_state->robot_position + 2 * move_delta);
		bool box_behind = (given_state->robot_position - move_delta) == given_state->box_positions[box];
		bool forward_open = is_free(given_state, given_state->robot_position + move_delta);

		// case when robot is pushing (forward)box forward or free space ahead
		if ((box_in_front && dbl_forward_open) || forward_open) {
			
			// push direction
			Action action = m_action_map[move_delta][0];
			possible_actions.push_back(action);
		}

		// case when robot is pulling (backward)box forward
		if (box_behind && forward_open) {
			
			// pull direction
			Action action = m_action_map[move_delta][1];
			possible_actions.push_back(action);
		}
	}

	// remove all duplicates of actions
	std::sort(possible_actions.begin(), possible_actions.end());
	possible_actions.erase(std::unique(possible_actions.begin(), possible_actions.end()), possible_actions.end());
}

std::vector<Action> ProblemSolvingAgent::moveNorth(GameState* state)
{
	std::vector<Action> north_actions;
	int move_delta = -state->board_width;
	Move(move_delta, state, north_actions);
	return north_actions;
}

std::vector<Action> ProblemSolvingAgent::moveSouth(GameState* state)
{
	std::vector<Action> south_actions;
	int move_delta = state->board_width;
	Move(move_delta, state, south_actions);
	return south_actions;
}

std::vector<Action> ProblemSolvingAgent::moveEast(GameState* state)
{
	std::vector<Action> east_actions;
	int move_delta = 1;
	Move(move_delta, state, east_actions);
	return east_actions;
}

std::vector<Action> ProblemSolvingAgent::moveWest(GameState* state)
{
	std::vector<Action> west_actions;
	int move_delta = -1;
	Move(move_delta, state, west_actions);
	return west_actions;
}

GameState* ProblemSolvingAgent::RESULT(GameState* state, Action action)
{
	GameState* new_state = new GameState(*state);
	int move_delta = 0;

	switch (action)
	{
	case Action::Push_North:
		move_delta = -state->board_width;
		new_state = Execute(move_delta, state, false);
		break;

	case Action::Push_South:
		move_delta = state->board_width;
		new_state = Execute(move_delta, state, false);
		break;

	case Action::Push_East:
		move_delta = 1;
		new_state = Execute(move_delta, state, false);
		break;

	case Action::Push_West:
		move_delta = -1;
		new_state = Execute(move_delta, state, false);
		break;

	case Action::Pull_North:
		move_delta = -state->board_width;
		new_state = Execute(move_delta, state, true);
		break;

	case Action::Pull_South:
		move_delta = state->board_width;
		new_state = Execute(move_delta, state, true);
		break;

	case Action::Pull_East:
		move_delta = 1;
		new_state = Execute(move_delta, state, true);
		break;

	case Action::Pull_West:
		move_delta = -1;
		new_state = Execute(move_delta, state, true);
		break;
	}

	return new_state;
}

std::vector<Action> ProblemSolvingAgent::ACTIONS(GameState* state)
{
	std::vector<Action> executable_actions;

	std::vector<Action> north_actions = moveNorth(state);
	std::vector<Action> south_actions = moveSouth(state);
	std::vector<Action> east_actions = moveEast(state);
	std::vector<Action> west_actions = moveWest(state);

	// loop thru north actions and add to executable actions
	for (int i = 0; i < north_actions.size(); i++) {
		executable_actions.push_back(north_actions[i]);
	}

	// loop thru south actions and add to executable actions
	for (int i = 0; i < south_actions.size(); i++) {
		executable_actions.push_back(south_actions[i]);
	}

	// loop thru east actions and add to executable actions
	for (int i = 0; i < east_actions.size(); i++) {
		executable_actions.push_back(east_actions[i]);
	}

	// loop thru west actions and add to executable actions
	for (int i = 0; i < west_actions.size(); i++) {
		executable_actions.push_back(west_actions[i]);
	}

	return executable_actions;
}

bool ProblemSolvingAgent::visited(std::vector<GameState*> states, GameState* state)
{
	// check through the visited states and see if given state is in there
	for (int i = 0; i < states.size(); i++) {
		if (*states[i] == *state) {
			return true;
		}
	}

	return false;
}

bool ProblemSolvingAgent::is_free(GameState* state, int index)
{
	if (state->board[index] == Board::CellValues::Obstacle) {
		return false;
	}

	for (int i = 0; i < state->box_positions.size(); i++) {
		if (state->box_positions[i] == index) {
			return false;
		}
	}

	return true;
}

GameState* ProblemSolvingAgent::Execute(int move_delta, GameState* given_state, bool isPulling)
{
	// copy given state
	GameState* new_state = new GameState(*given_state);

	// check all moveable boxes to see if they can be moved on this action
	// one box can be moved at a time
	// robot can make a move if there is no box in front of it
	// robot can only execute one action per function call
	bool bot_already_moved = false;
	for (int box = 0; box < new_state->box_positions.size(); box++) {

		if ((new_state->box_positions[box] + move_delta) > new_state->board.size() - 1
			|| (new_state->box_positions[box] + move_delta) < 0) {
			continue;
		}
		else if ((new_state->box_positions[box] - move_delta) > new_state->board.size() - 1
			|| (new_state->box_positions[box] - move_delta) < 0) {
			continue;
		}

		// check to the sides of the robot
		bool box_in_front = (given_state->robot_position + move_delta) == given_state->box_positions[box];
		bool box_behind = (given_state->robot_position - move_delta) == given_state->box_positions[box];

		if ((isPulling && box_behind) || box_in_front) {
			new_state->box_positions[box] += move_delta;
			new_state->robot_position += move_delta;
			bot_already_moved = true;
		}

		if (bot_already_moved) {
			break;
		}
	}
	// what happens when guy just wants to move solo
	if(!bot_already_moved) {
		new_state->robot_position += move_delta;
	}

	return new_state;
}