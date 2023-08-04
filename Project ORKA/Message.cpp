#include "Message.hpp"

Vector<Byte> Message::serialize()
{
	header.bufferSize = data.size();
	Vector<Byte> buffer(headerLength + header.bufferSize);
	for(SizeT i = 0; i < headerLength; ++i) buffer[i] = header.bufferSize >> (headerLength - i - 1) * 8 & 0xFF;
	std::ranges::copy(data, buffer.begin() + headerLength);
	return buffer;
}

Message Message::deserialize(const Vector<Byte>& buffer)
{
	Message message;
	for(SizeT i = 0; i < headerLength; ++i) message.header.bufferSize |= static_cast<SizeT>(buffer[i]) << (headerLength - i - 1) * 8;
	message.data.assign(buffer.begin() + headerLength, buffer.end());
	return message;
}
