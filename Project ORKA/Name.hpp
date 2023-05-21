#pragma once
#include <string>

constexpr unsigned short nameSize = 64;

struct Name
{
	char data[nameSize] = {};

	Name();

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Name(const std::string& name);

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Name(const char* name);

	Name& operator=(const char* other);

	Name& operator=(const std::string& other);

	bool operator==(const Name& other) const;

	bool operator!=(const Name& other) const;
};

bool operator<(const Name& l, const Name& r);