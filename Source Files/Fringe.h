#pragma once

#include <queue>
#include <stack>

#define f_QUEUE
//#define f_STACK

// implement the fringe as a template class
// necessary for hiding the implementation of the fringe
// from the search algorithm
template <typename T>
class Fringe
{
public:
	Fringe(T* root) {

		#ifdef f_QUEUE
		m_queue.push(root);
		#endif // f_QUEUE

		#ifdef f_STACK
		m_stack.push(root);
		#endif // f_STACK
	}
	~Fringe() {}

	void push(T* node) {
		#ifdef f_QUEUE
		m_queue.push(node);
		#endif // f_QUEUE

		#ifdef f_STACK
		m_stack.push(node);
		#endif // f_STACK
	}
	
	T* pop() {
		T* node;

		#ifdef f_QUEUE
		node = m_queue.front();
		m_queue.pop();
		#endif // f_QUEUE

		#ifdef f_STACK
		node = m_stack.pop();
		#endif // f_STACK

		return node;
	}

	bool is_empty() {
		#ifdef f_QUEUE
		return m_queue.empty();
		#endif // f_QUEUE

		#ifdef f_STACK
		return m_stack.empty();
		#endif // f_STACK
	}

private:
#ifdef f_QUEUE
	// implement the fringe as a queue
	std::queue<T*> m_queue;
#endif // f_QUEUE

#ifdef f_STACK
	// implement the fringe as a stack
	std::stack<T*> m_stack;
#endif // f_STACK
};

