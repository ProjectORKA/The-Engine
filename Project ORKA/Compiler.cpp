#include "Compiler.hpp"
#include "Windows.hpp"

ULong compileFromSource(const char* args)
{
	STARTUPINFO         startupInfo;
	PROCESS_INFORMATION processInfo;
	memset(&startupInfo, 0, sizeof startupInfo);
	memset(&processInfo, 0, sizeof processInfo);

	String cmdLine(R"(C:\Program Files\LLVM\bin\clang++.exe)");
	cmdLine += args;

	if(!CreateProcessA(nullptr, const_cast<char*>(cmdLine.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &processInfo)) return false;
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	DWORD exitCode;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	return exitCode;
}
