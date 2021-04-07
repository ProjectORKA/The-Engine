#include "ULLUtil.hpp"

ULL cutBackBits(ULL number, UShort count)
{
    if (count < 64) {
    ULL mask = ULL(1) << count;
    mask -= 1;
    mask = ~mask;
    return number & mask;
    }
    else {
        return 0;
    }
}
ULL cutFrontBits(ULL number, UShort count)
{
    if (count != 0) {
    ULL mask = ULL(1) << 64-count;
    mask -= 1;
    return number & mask;
    }
    else {
        return number;
    }
}
