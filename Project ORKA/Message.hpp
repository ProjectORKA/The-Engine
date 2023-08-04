#pragma once

#include "Basics.hpp"

struct MessageHeader
{
	SizeT bufferSize = 0;
};

struct Message
{
	String                 data;
	MessageHeader          header;
	static constexpr SizeT headerLength = sizeof(MessageHeader);

	Vector<Byte>   serialize();
	static Message deserialize(const Vector<Byte>& buffer);
};
