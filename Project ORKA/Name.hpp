#pragma once

#include "String.hpp"

#include <iostream>

#define NAME_SIZE 64

struct Name {
	char data[NAME_SIZE] = {};

	Name() { memset(&data[0], 0, NAME_SIZE); }

	Name(const String& name) { *this = name; }

	Name(const char* name) {
		memset(&data[0], 0, NAME_SIZE);
		for (int i = 0; i < NAME_SIZE; i++) {
			if (name[i] != 0) data[i] = name[i];
			else return;
		}
	}

	Name& operator=(const char* other) {
		memset(&data[0], 0, NAME_SIZE);
		for (int i = 0; i < NAME_SIZE; i++) {
			if (other[i] != 0) data[i] = other[i];
			else return *this;
		}
		return *this;
	}

	Name& operator=(const String& other) {
		memset(&data[0], 0, NAME_SIZE);
		for (int i = 0; i < NAME_SIZE; i++) {
			if (other[i] != 0) data[i] = other[i];
			else return *this;
		}
		return *this;
	}

	bool operator==(const Name& other) const { return strncmp(data, other.data, NAME_SIZE) == 0; }

	bool operator!=(const Name& other) const { return !(*this == other); }
};

bool operator<(const Name& l, const Name& r);
