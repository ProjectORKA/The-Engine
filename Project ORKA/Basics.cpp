#include "Basics.hpp"

#include <iostream>

Name::Name() {
	memset(&data[0], 0, NAME_SIZE);
}
Name::Name(const char* name) {
	memset(&data[0], 0, NAME_SIZE);
	strcpy(&data[0], name);
}
Name& Name::operator=(const char* other) {
	memset(&data[0], 0, NAME_SIZE);
	strcpy(&data[0], other);
	return *this;
}
Name& Name::operator=(std::string other)
{
	memset(&data[0], 0, NAME_SIZE);
	strcpy(&data[0], other.data());
	return *this;
}

bool Name::operator==(const Name& rhs) {
	return 0 == memcmp(&data[0], &rhs.data[0], NAME_SIZE);
}
bool operator<(const Name& l, const Name& r) {
	int x = memcmp(&l.data[0], &r.data[0], NAME_SIZE);
	return 0 > x;
}