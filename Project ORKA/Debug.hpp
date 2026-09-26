#pragma once

#include "Settings.hpp"
#include "Basics.hpp"

inline void print(const String& text)
{
	std::cout << text;
}

inline void println(const String& text)
{
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
using DebugLogSink = Function<void(const String&, const String&, const String&)>;
void setDebugLogSink(DebugLogSink sink);
void emitStructuredDebugLog(const String& level, const String& category, const String& message);
void printToConsole(const String& message);
void printHighlighted(const String& highlighted, const Vec4 color, const String& message);

template <typename T>
void logDebug(T value)
{
	if (debugLoggingIsEnabled)
	{
		const String message = toString(value);
		emitStructuredDebugLog("debug", "engine", message);
		printHighlighted("Debug: ", Color(0, 1, 0, 0), message);
	}
}

template <typename T>
void logWarning(T value)
{
	if (warningLoggingIsEnabled)
	{
		const String message = toString(value);
		emitStructuredDebugLog("warning", "engine", message);
		printHighlighted("Warning: ", Color(1, 1, 0, 0), message);
		#ifdef _WIN32
		if (debugBreakOnWarning) __debugbreak();
		#endif
		beep();
	}
}

template <typename T>
void logError(T value)
{
	if (errorLoggingIsEnabled)
	{
		const String message = toString(value);
		emitStructuredDebugLog("error", "engine", message);
		printHighlighted("ERROR: ", Color(1, 0, 0, 0), message);
		beep();
		#ifdef _WIN32
		if (debugBreakOnError) __debugbreak();
		#endif
		if (debugBreakOnError) pause();
	}
	else messageBox(toString(value), "Error");
}

inline void logDebugLaunchParameters(const Int count, Char** params)
{
	if (!debugLaunchParameters) return;
	for (int i = 0; i < count; i++) logDebug("Launch parameter " + toString(i) + ": " + toString(params[i]));
}
