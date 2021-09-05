#pragma once

#include <cstring>
#include <string>

#pragma warning(disable : 4996) //disables unsecure warning

using String = std::string;

template<typename T>
String toString(T t) {
	return std::to_string(t);
}