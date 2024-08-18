#include "Hash.hpp"
#include "Debug.hpp"

namespace External
{
	#include "openssl/sha.h"
}

// [TODO] current approach is deprecated
String calculateSha256Hash(const Path& filePath)
{
	std::ifstream file(filePath, std::ios::binary);

	if (!file.is_open())
	{
		logError("Error opening file: " + toString(filePath));
		return "";
	}

	External::SHA256_CTX sha256Context;
	SHA256_Init(&sha256Context);

	while (!file.eof())
	{
		constexpr SizeT bufferSize = 4096;
		Char buffer[bufferSize];
		file.read(buffer, bufferSize);
		SHA256_Update(&sha256Context, buffer, file.gcount());
	}

	Byte hash[SHA256_DIGEST_LENGTH];
	SHA256_Final(hash, &sha256Context);

	std::stringstream result;
	for (Byte i : hash)
	{
		result << std::hex << std::setw(2) << std::setfill('0') << static_cast<Int>(i);
	}

	return result.str();
}
