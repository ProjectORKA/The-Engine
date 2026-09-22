#include "AiDownload.hpp"

#include "../Debug.hpp"
#include "../Windows.hpp"
#include "AiProcess.hpp"

#include <filesystem>
#include <sstream>

namespace
{
	Bool runShellCommand(const String& command, const Path& workingDirectory)
	{
		const AiChildProcess child = startHiddenProcess(command.c_str(), workingDirectory);
		if (!child.processHandle) return false;

		WaitForSingleObject(child.processHandle, INFINITE);
		DWORD exitCode = 1;
		GetExitCodeProcess(child.processHandle, &exitCode);
		CloseHandle(child.processHandle);
		return exitCode == 0;
	}
}

Bool remoteUrlAvailable(const String& url)
{
	std::ostringstream command;
	command << "curl.exe -sI -L --fail -A \"ORKA-Engine\" \"" << url << "\"";
	return runShellCommand(command.str(), std::filesystem::current_path());
}

Bool downloadUrlToFile(
	const String& url,
	const Path& destination,
	const AiDownloadProgressCallback& onProgress)
{
	std::filesystem::create_directories(destination.parent_path());
	const Path partial = destination.string() + ".partial";
	std::error_code removeError;
	std::filesystem::remove(partial, removeError);

	std::ostringstream command;
	command << "curl.exe -L --fail --retry 3 -A \"ORKA-Engine\" --output \"" << partial.string() << "\" \"" << url << "\"";

	if (onProgress)
	{
		AiDownloadProgress progress;
		progress.phase = "download";
		progress.bytesReceived = 0;
		progress.bytesTotal = 0;
		onProgress(progress);
	}

	if (!runShellCommand(command.str(), destination.parent_path()))
	{
		logError("AiDownload: curl failed for " + url);
		std::filesystem::remove(partial, removeError);
		return false;
	}

	std::error_code ec;
	std::filesystem::rename(partial, destination, ec);
	if (ec)
	{
		logError("AiDownload: failed to finalize " + toString(destination) + " (" + ec.message() + ")");
		return false;
	}

	if (onProgress)
	{
		AiDownloadProgress done;
		done.phase = "download";
		done.bytesReceived = 1;
		done.bytesTotal = 1;
		onProgress(done);
	}
	return true;
}
