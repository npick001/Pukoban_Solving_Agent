#pragma once
#include <iostream>

template <typename T>
class PriorityQueue {

public:
	PriorityQueue() {
		_head = 0;
		_size = 0;
	}

	/*
		This function uses an insertion sort to add the event
		at the position in ascending order.

		This is so the removal of an event occurs at the head.
	*/
	void insert(int value, T* data) {
	/*
		check nodes and do an insertion sort
		to insert event in the correct position
		between the correct time steps.
	*/

		Node* node = new Node(value, data);

		if (_head == 0) {
			_head = node;
		}
		else {
			Node* current = _head;

			// check if it is smaller than the head
			if (node->_eventTime < _head->_eventTime) {
				node->_next = _head;
				_head = node;
			}
			else {

				// check all nodes in event set except head
				// for conditional insertion
				while (current->_next != NULL && current->_next->_eventTime < node->_eventTime) {
					current = current->_next;
				}

				// add the node in the correct position
				node->_next = current->_next;

				// move the list to after the insertion
				current->_next = node;
			}
		}
		_size++;
	}

	/*
		This function returns the event with the
		lowest time stamp.
	*/
	T* extract() {
		if (_head == 0) return 0;
		else {
			// save the EA from the head
			T* ea = _head->_ea;
			Node* n = _head;
			_head = _head->_next;

			// remove the event from list
			delete n;
			_size--;
			return ea;
		}
	}

	/*
		This is a debugging function
		designed to print out the times in the event set.
	*/
	//void PrintEventSet();

	/*
		This function returns the event time
		of the first event in the list -> already sorted
	*/
	bool empty() {
		return _size == 0;
	}

private:

	struct Node {
		Node(int t, T* ea) {
			_ea = ea;
			_eventTime = t;
			_next = 0;
		}
		T* _ea;
		int _eventTime;
		Node* _next;
	};

	Node* _head;
	int _size;
};