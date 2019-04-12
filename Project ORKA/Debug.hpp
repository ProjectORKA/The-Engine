#pragma once
#ifndef DEBUG_HPP
#define DEBUG_HPP

void debugPrint(const char * debugMessage) {
#ifdef DEBUG
	std::cout << debugMessage << std::endl;
#endif // DEBUG
}

#endif // !DEBUG_HPP
