#include "State.h"

void State::SetParent(State* previous_state)
{
	m_parent = previous_state;
	m_number_of_moves = m_parent->GetNumberOfMoves() + 1;
}

void State::SetAction(Action action)
{
	m_parent_action = action;
}

void State::SetPathCost(int cost)
{
	m_path_cost_absolute = cost;
}

void State::SetHeuristicValue(int cost)
{
	m_path_cost_marginal = cost;

	if (m_parent == nullptr) {
		m_path_cost_absolute = m_path_cost_marginal;
	}
	else {
		m_path_cost_absolute = m_parent->GetPathCost() + m_path_cost_marginal;
	}
}

void State::SetManhattanDistance(int distance)
{
	m_manhattan_distance = distance;
}

State* State::GetParent()
{
	return m_parent;
}

Action State::GetAction()
{
	return m_parent_action;
}

int State::GetPathCost()
{
	return m_path_cost_absolute;
}

int State::GetHeuristicValue()
{
	return m_path_cost_marginal;
}

int State::GetManhattanDistance()
{
	return m_manhattan_distance;
}

int State::GetNumberOfMoves()
{
	return m_number_of_moves;
}

bool State::operator==(const State& other) const
{
	if (m_parent == other.m_parent
		&& m_parent_action == other.m_parent_action) {
		return true;
	}
	return false;
}

bool State::operator<(const State& other) const
{
	return (m_manhattan_distance < other.m_manhattan_distance);
}

bool State::operator>(const State& other) const
{
	return (m_manhattan_distance > other.m_manhattan_distance);
}

bool State::operator<=(const State& other) const
{
	return (m_manhattan_distance <= other.m_manhattan_distance);
}

State::State()
{
	m_path_cost_absolute = 0;
	m_path_cost_marginal = 1;
	m_parent = nullptr;
	m_number_of_moves = 0;
}
