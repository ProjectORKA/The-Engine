#include "ULLUtil.hpp"

ULL cutBackBits(const ULL number, const UShort count) {
	if (count < 64) {
		ULL mask = static_cast<ULL>(1) << count;
		mask -= 1;
		mask = ~mask;
		return number & mask;
	}
	return 0;
}

ULL cutFrontBits(const ULL number, const UShort count) {
	if (count != 0) {
		ULL mask = static_cast<ULL>(1) << 64 - count;
		mask -= 1;
		return number & mask;
	}
	return number;
}
