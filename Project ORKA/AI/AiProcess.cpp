#include "AiProcess.hpp"

#include "../Debug.hpp"
#include "../Windows.hpp"

#include <cstring>

Bool AiChildProcess::isRunning() const
{
	if (!processHandle) return false;
	DWORD exitCode = STILL_ACTIVE;
	if (!GetExitCodeProcess(processHandle, &exitCode)) return false;
	return exitCode == STILL_ACTIVE;
}

void AiChildProcess::terminate()
{
	if (!processHandle) return;
	if (isRunning()) TerminateProcess(processHandle, 0);
	CloseHandle(processHandle);
	processHandle = nullptr;
	processId = 0;
}

AiChildProcess startHiddenProcess(const char* commandLine, const Path& workingDirectory)
{
	AiChildProcess child;
	STARTUPINFOA startupInfo{};
	startupInfo.cb = sizeof startupInfo;
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION processInfo{};

	Vector<char> mutableCommand;
	if (commandLine)
	{
		const SizeT length = std::strlen(commandLine);
		mutableCommand.assign(commandLine, commandLine + length);
	}
	mutableCommand.push_back('\0');
	const Bool created = CreateProcessA(
		nullptr,
		mutableCommand.data(),
		nullptr,
		nullptr,
		FALSE,
		CREATE_NO_WINDOW,
		nullptr,
		workingDirectory.string().c_str(),
		&startupInfo,
		&processInfo);

	if (!created)
	{
		logError("AiProcess: CreateProcess failed.");
		return child;
	}

	child.processHandle = processInfo.hProcess;
	child.processId = processInfo.dwProcessId;
	CloseHandle(processInfo.hThread);
	return child;
}
