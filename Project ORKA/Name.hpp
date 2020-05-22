#pragma once

#include "Basics.hpp"
#include <cstring>

#pragma warning(disable : 4996) //disables unsecure warning

#define NAME_SIZE 100

struct Name {
	char data[NAME_SIZE] = {};
	
	Name();
	Name(const char* name);
	Name& operator=(const char* other);
	Name& operator=(String other);
	bool operator==(const Name& rhs);
};

bool operator<(const Name& l, const Name& r);