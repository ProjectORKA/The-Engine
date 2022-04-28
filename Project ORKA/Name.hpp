
#pragma once

#include "String.hpp"

#include <iostream>

#define NAME_SIZE 64

struct Name {
	char data[NAME_SIZE] = {};

	Name();
	Name(String name);
	Name(const char* name);
	Name& operator=(const char* other);
	Name& operator=(String other);
	bool operator==(const Name& rhs);
};

bool operator<(const Name& l, const Name& r);