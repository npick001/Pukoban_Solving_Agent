#pragma once
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
	State* GetParent();

protected:
	State();

private:
	std::vector<State*> m_next_states;

	// the node in the search tree that generated this node
	State* m_parent;

	// the action that was applied to the parent to generate the node
	Action m_parent_action;

	// the cost of the path from the initial state
	// to the self state, as indicated by the parent pointers
	// i.e. root to here cost
	int m_path_cost;
};

