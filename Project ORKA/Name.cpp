
#include "Name.hpp"

Name::Name() {
	memset(&data[0], 0, NAME_SIZE);
}
Name::Name(const char* name) {
	memset(&data[0], 0, NAME_SIZE);
	strcpy_s(data, name);
}
Name& Name::operator=(const char* other) {
	memset(&data[0], 0, NAME_SIZE);
	strcpy_s(data, other);
	return *this;
}
Name& Name::operator=(String other)
{
	memset(&data[0], 0, NAME_SIZE);
	strcpy_s(data, other.data());
	return *this;
}

bool Name::operator==(const Name& rhs) {
	return 0 == memcmp(&data[0], &rhs.data[0], NAME_SIZE);
}
bool operator<(const Name& l, const Name& r) {
	int x = memcmp(&l.data[0], &r.data[0], NAME_SIZE);
	return 0 > x;
}