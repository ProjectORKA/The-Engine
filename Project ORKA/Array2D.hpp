#pragma once

#include "Basics.hpp"

template <typename T>
struct Array2D
{
	UInt size = 0;
	T*   data = nullptr;

	~Array2D();
	Array2D(UInt size);
	Array2D() = default;
	T& get(UInt x, UInt y);
};

template <typename T>
Array2D<T>::Array2D(UInt size) {
	this->size = size;
	data       = static_cast<T*>(malloc(sizeof(T) * size * size));
}

template <typename T>
Array2D<T>::~Array2D() {
	free(data);
}

template <typename T>
T& Array2D<T>::get(const UInt x, const UInt y) {
	return data[x + size * y];
}
