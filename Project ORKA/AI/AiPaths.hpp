#pragma once

#include "../OrkaTypes.hpp"

struct AiPaths
{
	[[nodiscard]] static Path aiRoot();
	[[nodiscard]] static String activeRuntimeTag();
	static void setActiveRuntimeTag(const String& tag);
	[[nodiscard]] static Path runtimeRoot();
	[[nodiscard]] static Path modelsRoot();
	[[nodiscard]] static Path consentFile();
	[[nodiscard]] static Path runtimeZip(const String& zipFileName);
	[[nodiscard]] static Path defaultModelFile();
	[[nodiscard]] static Path stateFile();

	static void ensureLayout();
};
