
#pragma once

#include "Basics.hpp"

template <typename T>
struct Array2D {
	UInt size = 0;
	T * data = nullptr;

	Array2D(UInt size) {
		this->size = size;
		data = (T*)malloc(sizeof(T) * size * size);
	}
	~Array2D() {
		free(data);
	}
	T& get(UInt x, UInt y) {
		return data[x + size * y];
	}
	void set(UInt x, UInt y, T value) {
		data[x + size * y] = value;
	}
};