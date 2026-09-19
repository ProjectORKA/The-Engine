#include "Debug.hpp"
#include "Windows.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

namespace
{
	Mutex debugLogSinkMutex;
	DebugLogSink debugLogSink;
}

void setDebugLogSink(DebugLogSink sink)
{
	LockGuard lock(debugLogSinkMutex);
	debugLogSink = std::move(sink);
}

void emitStructuredDebugLog(const String& level, const String& category, const String& message)
{
	DebugLogSink sink;
	{
		LockGuard lock(debugLogSinkMutex);
		sink = debugLogSink;
	}
	if(sink) sink(level, category, message);
}

void messageBox(const String& message, const String& title)
{
	MessageBox(NULL, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
}

void printToConsole(const String& message)
{
	fmt::print("{}\n", message);
}

void printHighlighted(const String& highlighted, const Vec4 color, const String& message)
{
	fmt::println(
		"{}{}",
		fmt::format(fg(static_cast<fmt::color>(colorToRGBHex(color))) | fmt::emphasis::bold, "{}", highlighted),
		message);
}
