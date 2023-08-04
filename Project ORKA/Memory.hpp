// ReSharper disable CppDeletingVoidPointer
#pragma once
#include "Basics.hpp"

class Memory
{
public:
	Memory();
	~Memory();
	explicit Memory(SizeT size);
	Memory(const Memory& other);
	Memory(const void* source, SizeT size);

	void destroy();

	Memory& operator=(const Memory& other);

	[[nodiscard]] Bool  isValid() const;
	[[nodiscard]] Byte* getData() const;
	[[nodiscard]] SizeT getSize() const;
private:
	Byte* data;
	SizeT size;
};
