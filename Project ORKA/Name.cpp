#include "Name.hpp"

Name::Name()
{
	memset(&data[0], 0, nameSize);
}

Name::Name(const std::string& name)
{
	*this = name;
}

Name::Name(const char* name)
{
	memset(&data[0], 0, nameSize);
	for(int i = 0; i < nameSize; i++)
	{
		if(name[i] != 0) data[i] = name[i];
		else return;
	}
}

Name& Name::operator=(const char* other)
{
	memset(&data[0], 0, nameSize);
	for(int i = 0; i < nameSize; i++)
	{
		if(other[i] != 0) data[i] = other[i];
		else return *this;
	}
	return *this;
}

Name& Name::operator=(const std::string& other)
{
	memset(&data[0], 0, nameSize);
	for(int i = 0; i < nameSize; i++)
	{
		if(other[i] != 0) data[i] = other[i];
		else return *this;
	}
	return *this;
}

bool Name::operator==(const Name& other) const
{
	return strncmp(data, other.data, nameSize) == 0;
}

bool Name::operator!=(const Name& other) const
{
	return ! (*this == other);
}

bool operator<(const Name& l, const Name& r)
{
	const int x = memcmp(&l.data[0], &r.data[0], nameSize);
	return 0 > x;
}