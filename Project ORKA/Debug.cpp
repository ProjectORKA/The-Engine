
#include "Debug.hpp"

void debugPrint(const char * debugMessage) {
#ifdef DEBUG
	std::cout << debugMessage << std::endl;
#endif // DEBUG
};