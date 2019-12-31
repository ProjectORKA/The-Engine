//	Welcome fellow programmer!
//	My name is Samuel and im the creator of Project ORKA.
//	This is the open source code for the framework / game engine
//	Feel free to take a look at the code and point out all the errors
//	If you want to report bugs, request features or add your own code take a look at our GitHub:
//	https://github.com/ProjectORKA/Project-ORKA

//	Legal stuff:
//	This engine can NOT be used for your own personal or commercial use!
//	Do not distribute any source code outside of our official websites and platforms without permission!

//	The "Project ORKA" engine is built in a data driven way (i think)
//	This means everything is divided into
//		1. data stored in structs and
//		2. functions that act on that data
//		(3. also threads, but they are essentially functions)
//	This engine uses a left-handed coordinate system like Blender:
//	+X = right
//	+Y = forward
//	+Z = up

//	Rules:
//	1. Data
//		All relevant data will be stored in structs
//		Structs start with an uppercase letter e.g. "SpaceShip"
//		They can have
//			- constructors
//			- destructors
//			- pointers/references
//			- other structs/containers
//		However, they do NOT have member-functions/methods!
//		(unless the stored data always needs conversion)
//	2. Functions
//		All functions are (ideally) of type void and should be self explainatory
//		They only take a minimal amount of references (and data) as inputs
//		Functions start with a lowercase letter and describe what they do e.g. "startSpaceShip"
//		Any kind of initialization or destruction should be a constructor/destructor instead
//	3. Threads
//		Threads are initialized through constructors and destructors
//		The functions they run follow the conventions above, but are declared in their own section and start with an uppercase letter

#pragma region ignore

#ifdef _WIN32
int main(int argc, char *argv[]);
#include <Windows.h>
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	main(__argc, __argv);
}
#endif
#pragma endregion

#include "Program.hpp"

int main(int argc, char *argv[]) {

	
	if(true) //quickly switches between the game and custom testing code
	{
		runProgram();		//<-- starts execution of ALL code
	} else {
		//testing code
		/////////////////////////////////////////////////////////////////////////////////////////////////
		{
		
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////
		system("pause");
	}
	return 0;
}
