
#pragma once

#include <cstring>
#include <string>

using Bool = bool;

#pragma warning(disable : 4996) //disables unsecure warning

using String = std::string;

String toString(Bool b);

template<typename T>
String toString(T t) {
	return std::to_string(t);
}

//trim
//trim end
//trim front
//to lower
//to upper
//replace
//contains
//starts with
//ends with

