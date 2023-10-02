#pragma once
#include <string>
#include <vector>
enum class Action;
/* 
 parent class that defines a state.
 this class will assume it is always given 
 states valid to the problem.
*/
class State
{
public:
	void SetParent(State* previous_state);
	void SetAction(Action action);
	void SetPathCost(int cost);
	void SetHeuristicValue(int cost);
	void SetManhattanDistance(int distance);

	State* GetParent();
	Action GetAction();
	int GetPathCost();
	int GetHeuristicValue();
	int GetManhattanDistance();
	int GetNumberOfMoves();

	virtual void print() = 0;
	virtual std::string to_string() = 0;
	bool operator==(const State& other) const;
	bool operator<(const State& other) const;
	bool operator>(const State& other) const;
	bool operator<=(const State& other) const;

protected:
	State();

	// the node in the search tree that generated this node
	State* m_parent;

	// the action that was applied to the parent to generate the node
	Action m_parent_action;

	// the cost of the path from the initial state
	// to the self state, as indicated by the parent pointers
	// i.e. root to here cost
	int m_path_cost_absolute;

	// the cost of the path from the last state
	// to the self state, as indicated by the parent pointers
	int m_path_cost_marginal;

	int m_manhattan_distance;
private:
	int m_number_of_moves;
};

