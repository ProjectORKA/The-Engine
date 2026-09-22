#include "AiArchive.hpp"

#include "../Debug.hpp"
#include "../Windows.hpp"

#include <sstream>

Bool extractZipArchive(const Path& zipPath, const Path& destinationDirectory)
{
	std::filesystem::create_directories(destinationDirectory);

	std::ostringstream command;
	command << "powershell.exe -NoProfile -ExecutionPolicy Bypass -Command \""
		<< "Expand-Archive -LiteralPath '" << zipPath.string() << "' -DestinationPath '"
		<< destinationDirectory.string() << "' -Force\"";

	STARTUPINFOA startupInfo{};
	startupInfo.cb = sizeof startupInfo;
	PROCESS_INFORMATION processInfo{};
	String cmdLine = command.str();
	Vector<char> cmdBuffer(cmdLine.begin(), cmdLine.end());
	cmdBuffer.push_back('\0');
	if (!CreateProcessA(nullptr, cmdBuffer.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &processInfo))
	{
		logError("AiArchive: failed to launch PowerShell for Expand-Archive.");
		return false;
	}

	WaitForSingleObject(processInfo.hProcess, INFINITE);
	DWORD exitCode = 1;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	if (exitCode != 0)
	{
		logError("AiArchive: Expand-Archive exit code " + toString(exitCode));
		return false;
	}
	return true;
}

Path findFirstFileNamed(const Path& root, const String& fileName)
{
	if (!std::filesystem::exists(root)) return {};
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(root))
	{
		if (!entry.is_regular_file()) continue;
		if (entry.path().filename().string() == fileName) return entry.path();
	}
	return {};
}
