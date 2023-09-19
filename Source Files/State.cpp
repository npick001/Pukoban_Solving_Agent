#include "State.h"

void State::SetParent(State* previous_state)
{
	m_parent = previous_state;
}

void State::SetAction(Action action)
{
	m_parent_action = action;
}

State* State::GetParent()
{
	return m_parent;
}

State::State()
{
	m_parent = nullptr;
}
