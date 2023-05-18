#pragma once

//dont use doesent work for some reason

template <typename T>
struct QueueNode {
	T data;
	QueueNode* next = nullptr;
};

template <typename T>
struct ORKAQueue {
	QueueNode<T>* root = nullptr;

	T& front() { return root->data; }

	void popFront() {
		if (root) {
			const QueueNode<T>* first = root;
			root = root->next;
			delete first;
		}
	};

	void remove(T data) {
		if (root) {
			QueueNode<T>** nodePtrPtr = &root;

			while (*nodePtrPtr != nullptr) {
				if ((*nodePtrPtr)->data == data) {
					const QueueNode<T>* tmp = *nodePtrPtr;
					*nodePtrPtr = (*nodePtrPtr)->next;
					delete tmp;
				}
				else { nodePtrPtr = &((*nodePtrPtr)->next); }
			}
		}
	}

	void insert(T data) {
		if (!root) {
			root = new QueueNode<T>();
			root->data = data;
		}
		else {
			QueueNode<T>* current = root;

			while (current->next != nullptr) {
				if (current->data == data) return;
				current = current->next;
			}

			current->next = new QueueNode<T>();
			current->next->data = data;
		}
	};
};
