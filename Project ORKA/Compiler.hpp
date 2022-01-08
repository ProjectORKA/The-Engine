
#include "Basics.hpp"
#include <Windows.h>

//if (compileFromSource(" C:\\Users\\Blendurian\\Desktop\\main.cpp"))logDebug("Okay!"); else logDebug("NotOkay!");

ULong compileFromSource(const char* args)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION proccessInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	memset(&proccessInfo, 0, sizeof(proccessInfo));

	std::string cmdLine("C:\\Program Files\\LLVM\\bin\\clang++.exe");
	cmdLine += args;

	if (!CreateProcessA(0, const_cast<char*>(cmdLine.c_str()),
		0, 0, FALSE, 0, 0, 0, &startupInfo, &proccessInfo))
		return false;
	WaitForSingleObject(proccessInfo.hProcess, INFINITE);
	DWORD exitCode;
	GetExitCodeProcess(proccessInfo.hProcess, &exitCode);
	CloseHandle(proccessInfo.hProcess);
	CloseHandle(proccessInfo.hThread);
	return exitCode;
}