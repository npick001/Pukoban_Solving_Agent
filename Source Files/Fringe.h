#pragma once
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <iostream>

#include "Board.h"
class GameState;

// implement the BFS fringe 
template <typename T1>
class FIFO
{
public:
	FIFO() {}
	~FIFO() {}

	void insert(T1* node) {
		m_queue.emplace_back(node);

		if (m_queue.size() % 10000 == 0) {
			std::cout << "Fringe size: " << m_queue.size() << '\n';
		}
	}
	
	T1* extract() {
		T1* node = m_queue.front();
		m_queue.pop_front();
		return node;
	}

	bool contains(T1* node) {
		auto compare_nodes = [node](T1* other) { return *node == *other; };
		return std::find_if(m_queue.begin(), m_queue.end(), compare_nodes) != m_queue.end();
	}

	bool is_empty() {
		return m_queue.empty();
	}

	std::string get_type() {
		return std::string("Queue");
	}

private:

	// implement the fringe as a queue
	std::deque<T1*> m_queue;
};

// implement the DFS fringe 
template <typename T2>
class LIFO
{
public:
	LIFO() {}
	~LIFO() {}

	void insert(T2* node) {
		m_stack.push(node);

		if (m_stack.size() % 10000 == 0) {
			std::cout << "Fringe size: " << m_stack.size() << '\n';
		}
	}

	T2* extract() {
		T2* node = m_stack.top();
		m_stack.pop();
		return node;
	}

	bool contains(T2* node) {
		// stack allows no iteration through data structure
		// we only have access to the top element
		return false;
	}

	bool is_empty() {
		return m_stack.empty();
	}

	std::string get_type() {
		return std::string("Stack");
	}

private:

	// implement the fringe as a stack
	std::stack<T2*> m_stack;
};