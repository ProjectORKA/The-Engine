#pragma once

#include "Settings.hpp"
#include "Basics.hpp"
#include "fmt/core.h"
#include "fmt/color.h"

inline void print(const String& text) {
	std::cout << text;
}

inline void println(const String& text) {
	std::cout << text + "\n";
}

inline void beep()
{
	std::cout << '\a';
}

inline void pause()
{
	if (debugLoggingIsEnabled)
	{
#ifdef _WIN32
		system("pause");
#else
	std::cout << "Thread was paused..." << "\n";
	char a;
	std::cin >> a;
#endif
	}
}

inline void clearConsole()
{
#ifdef _WIN32
	system("cls");
#else
    system("clear");
#endif
}

void messageBox(const String& message, const String& title);

inline void printToConsole(const String& message)
{
	fmt::print("{}\n", message);
}

inline void printHighlighted(const String& highlighted, const Vec4 color, const String& message)
{
	fmt::println("{}{}", fmt::format(fg(static_cast<fmt::color>(colorToRGBHex(color))) | fmt::emphasis::bold, "{}", highlighted), message);
}

template <typename T>
void logDebug(T value)
{
	if (debugLoggingIsEnabled) printHighlighted("Debug: ", Color(0, 1, 0, 0), toString(value));
}

template <typename T>
void logWarning(T value)
{
	if (warningLoggingIsEnabled)
	{
		printHighlighted("Warning: ", Color(1, 1, 0, 0), toString(value));
		if (debugBreakOnWarning) __debugbreak();
		beep();
	}
}

template <typename T>
void logError(T value)
{
	if (errorLoggingIsEnabled)
	{
		printHighlighted("ERROR: ", Color(1, 0, 0, 0), toString(value));
		beep();
		__debugbreak();
		pause();
	}
	else
	{
		messageBox(toString(value), "Error");
	}
}

inline void logDebugLaunchParameters(const Int count, Char** params)
{
	for (int i = 0; i < count; i++) logDebug(params[i]);
}
