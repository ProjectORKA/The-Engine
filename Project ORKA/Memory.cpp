#include "Memory.hpp"

Memory::~Memory()
{
	destroy();
}

void Memory::destroy()
{
	size = 0;
	delete[] data;
	data = nullptr;
}

Bool Memory::isValid() const
{
	return data != nullptr && size != 0;
}

Byte* Memory::getData() const
{
	return data;
}

SizeT Memory::getSize() const
{
	return size;
}

Memory::Memory(const SizeT size): size(size)
{
	data = new Byte[size];
}

Memory::Memory(): data(nullptr), size(0) {}

Memory::Memory(const Memory& other): size(other.size)
{
	data = new Byte[size];
	memcpy(data, other.data, size);
}

Memory& Memory::operator=(const Memory& other)
{
	if(this != &other)
	{
		delete[] data;

		size = other.size;
		data = new Byte[size];
		memcpy(data, other.data, size);
	}
	return *this;
}

Memory::Memory(const void* source, const SizeT size): size(size)
{
	data = new Byte[size];
	memcpy(data, source, size);
}
