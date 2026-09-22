#pragma once

#include "../OrkaTypes.hpp"
#include "../Windows.hpp"

struct AiChildProcess
{
	HANDLE processHandle = nullptr;
	DWORD processId = 0;

	[[nodiscard]] Bool isRunning() const;
	void terminate();
};

[[nodiscard]] AiChildProcess startHiddenProcess(const char* commandLine, const Path& workingDirectory);
