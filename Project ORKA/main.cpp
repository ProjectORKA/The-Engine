//	Welcome fellow programmer!
//	My name is Samuel and im the creator of Project ORKA.
//	This is the open source code for the framework / game engine
//	Feel free to take a look at the code and point out all the errors :)
//	Dont try to run this on Linux. It will probably break or dont work at all.
//	If you want to report bugs, request features or add your own code take a look at our GitHub:
//	https://github.com/ProjectORKA/Project-ORKA
//	If you want to know what Project ORKA is all about please visit:
//	https://project-orka.com (its probably not up-to-date though)

//	Legal stuff:
//	This engine can NOT be used for your own personal or commercial use!
//	Do not distribute any source code outside of our official websites and platforms without permission!

//	About the code:
//	The following C++ code is meant to be as clean and humanly readable as possible.
//	The "Project ORKA" engine is divided into
//		1. data stored in structs (yes everything is public)
//		2. functions that act on that data in form of void or member functions
//	It has a minimal amount of inheritance and polymorphism and avoids de/constructors as much as possible 

//	About the engine:
//	This engine uses a left-handed coordinate system like Blender:
//	+X = right
//	+Y = forward
//	+Z = up

//#define TESTING

//program includes
#include "Program.hpp"
#include "WindowsEntry.hpp"

//#include <windows.h>
//#include <string>
//#include <iostream>

//ULong compileFromSource(const char* args)
//{
//	STARTUPINFO startupInfo;
//	PROCESS_INFORMATION proccessInfo;
//	memset(&startupInfo, 0, sizeof(startupInfo));
//	memset(&proccessInfo, 0, sizeof(proccessInfo));
//
//	std::string cmdLine("C:\\Program Files\\LLVM\\bin\\clang++.exe");
//	cmdLine += args;
//
//	if (!CreateProcessA(0, const_cast<char*>(cmdLine.c_str()),
//		0, 0, FALSE, 0, 0, 0, &startupInfo, &proccessInfo))
//		return false;
//	WaitForSingleObject(proccessInfo.hProcess, INFINITE);
//	DWORD exitCode;
//	GetExitCodeProcess(proccessInfo.hProcess, &exitCode);
//	CloseHandle(proccessInfo.hProcess);
//	CloseHandle(proccessInfo.hThread);
//	return exitCode;
//}

#include "Random.hpp"

//#define TESTING

Int main(Int  argc, Char* argv[]) {
	srand(static_cast <unsigned> (time(0)));
#ifndef TESTING
	program.run();		//<-- starts the Program
#else
	//testing code
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//if (compileFromSource(" C:\\Users\\Blendurian\\Desktop\\main.cpp"))logDebug("Okay!"); else logDebug("NotOkay!");


	/////////////////////////////////////////////////////////////////////////////////////////////////
	pause();
#endif // TESTING


#ifdef MEMORY_LEAK_DETECTION
	_CrtDumpMemoryLeaks();
#endif // MEMORY_LEAK_DETECTION

	return 0;
}