#include "Basics.hpp"

Name::Name()
{
	memset(&data[0], 0, nameSize);
}

Name::Name(const Char* name)
{
	memset(&data[0], 0, nameSize);
	for (Int i = 0; i < nameSize; i++)
	{
		if (name[i] != 0)
		{
			data[i] = name[i];
		}
		else
		{
			return;
		}
	}
}

String Name::toString() const
{
	return data;
}

Name::Name(const String& name)
{
	*this = name;
}

Name& Name::operator=(const Char* other)
{
	memset(&data[0], 0, nameSize);
	for (Int i = 0; i < nameSize; i++)
	{
		if (other[i] != 0)
		{
			data[i] = other[i];
		}
		else
		{
			return *this;
		}
	}
	return *this;
}

Name& Name::operator=(const String& other)
{
	memset(&data[0], 0, nameSize);
	for (Int i = 0; i < nameSize; i++)
	{
		if (other[i] != 0)
		{
			data[i] = other[i];
		}
		else
		{
			return *this;
		}
	}
	return *this;
}

Bool Name::operator<(const Name& other) const
{
	const Int x = memcmp(&this->data[0], &other.data[0], nameSize);
	return 0 > x;
}

Bool Name::operator!=(const Name& other) const
{
	return !(*this == other);
}

Vec2Vector::Vec2Vector(const Vec3Vector& other) {
	for (const auto& v : other)
	{
		push_back(Vec2(v.x, v.y));
	}
}

Vec3Vector Vec2Vector::toVec3() const
{
	return Vec3Vector(*this);
}

Vec3Vector::Vec3Vector(const Vec2Vector& other) {
	for (const auto& v : other)
	{
		push_back(Vec3(v.x, v.y, 0));
	}
}

Vec2Vector Vec3Vector::toVec2() const
{
	return Vec2Vector(*this);
}

Bool Name::operator==(const Name& other) const
{
	return strncmp(data, other.data, nameSize) == 0;
}
