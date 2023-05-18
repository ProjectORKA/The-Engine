#include "Compiler.hpp"
#include "Platform.hpp"

ULong compileFromSource(const char* args) {
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION proccessInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	memset(&proccessInfo, 0, sizeof(proccessInfo));

	String cmdLine("C:\\Program Files\\LLVM\\bin\\clang++.exe");
	cmdLine += args;

	if (!CreateProcessA(nullptr, const_cast<char*>(cmdLine.c_str()),
	                    nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &proccessInfo))
		return false;
	WaitForSingleObject(proccessInfo.hProcess, INFINITE);
	DWORD exitCode;
	GetExitCodeProcess(proccessInfo.hProcess, &exitCode);
	CloseHandle(proccessInfo.hProcess);
	CloseHandle(proccessInfo.hThread);
	return exitCode;
}
