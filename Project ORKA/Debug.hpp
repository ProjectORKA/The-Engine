#pragma once

#include "Settings.hpp"
#include "Basics.hpp"

void beep();
void pause();

inline void printToConsole(const String& message)
{
	std::cout << message + "\n";
}

template <typename T> void logDebug(T value)
{
	if(debugLoggingIsEnabled) printToConsole("Debug: " + toString(value));
}

template <typename T> void logWarning(T value)
{
	if(warningLoggingIsEnabled)
	{
		printToConsole("Warning: " + toString(value));
		beep();
	}
}

template <typename T> void logError(T value)
{
	if(errorLoggingIsEnabled)
	{
		printToConsole("ERROR: " + toString(value));
		beep();
		__debugbreak();
		pause();
	}
}
