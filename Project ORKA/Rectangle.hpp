
#pragma once

#include "BasicsModule.hpp"

template<typename T>
struct Rect {
	T x;
	T y;
	T width;
	T height;

	Vec2 center();
};

template<typename T>
Vec2 Rect<T>::center()
{
	return Vec2((width / 2) + x, (height / 2) + y);
}