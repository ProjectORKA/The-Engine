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

//program includes
#include "GameSystem.hpp"
#include "UserInterface.hpp"
#include "WindowsEntry.hpp"

//#define TESTING
//#define MEMORY_LEAK_DETECTION


Int main(Int  argc, Char* argv[]) {
	srand(static_cast <unsigned> (time(0)));
#ifndef TESTING
	
	gameSystem.add(new Intro(new Pong()));

	window("ORKA",&gameView(0));

	ui.run();
#else
	//testing code
	/////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////////////////////////
	pause();
#endif // TESTING
#ifdef MEMORY_LEAK_DETECTION
	_CrtDumpMemoryLeaks();
#endif // MEMORY_LEAK_DETECTION

	return 0;
}