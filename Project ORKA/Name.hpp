#pragma once

#include <string>
#include "Basics.hpp"

constexpr unsigned short nameSize = 64;

struct Name
{
	Name();
	Name(const char* name);
	Name(const String& name);
	Name& operator=(const char* other);
	Name& operator=(const String& other);
	[[nodiscard]] String toString() const;
	[[nodiscard]] bool  operator<(const Name& other) const;
	[[nodiscard]] bool  operator==(const Name& other) const;
	[[nodiscard]] bool  operator!=(const Name& other) const;
private:
	char data[nameSize] = {};
};

inline String toString(const Name& name)
{
	return name.toString();
}