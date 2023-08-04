#pragma once

#include "Settings.hpp"
#include "Basics.hpp"

constexpr bool showEvents  = true;
constexpr bool showDebug   = true;
constexpr bool showWarning = true;
constexpr bool showError   = true;

void beep();
void pause();

inline void printToConsole(const String& message) {
	std::cout << message + "\n";
}

template <typename T> void logDebug(T value)
{
#ifdef DEBUG
	printToConsole("Debug: " + toString(value));
#endif
}

template <typename T> void logWarning(T value)
{
#ifdef WARNING_LOG
	printToConsole("Warning: " + toString(value));
	beep();
#endif // WARNING
}

template <typename T> void logError(T value)
{
#ifdef ERROR_LOG
	printToConsole("ERROR: " + toString(value));
	beep();
	__debugbreak();
	pause();
#endif // WARNING
}