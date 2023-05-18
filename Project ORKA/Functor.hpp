#pragma once

#include "Basics.hpp"

template <typename T>
struct Functor {
	T run = nullptr;

	Functor(T functionPointer) { run = functionPointer; };
	void swap(T functionPointer) { run = functionPointer; }
};
