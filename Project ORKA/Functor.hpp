
#include "Basics.hpp"

template <typename T>
struct Functor {
	Functor(T functionPointer) {
		run = functionPointer;
	};
	void swap(T functionPointer) {
		run = functionPointer;
	}
	T run = nullptr;
};