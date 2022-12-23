//	Welcome fellow programmer!
//	My name is Samuel and im the creator of Project ORKA.
//	This is the open source code for the games and game engine
//	Feel free to take a look at the code and point out all the errors :)
//	If you want to report bugs, request features or add your own code take a look at our GitHub:
//	https://github.com/ProjectORKA/Project-ORKA
//	If you want to know what Project ORKA is all about please visit:
//	https://project-orka.com

//	Legal stuff:
//	This code can not be used commercially!

//	About the code:
//	The following C++ code is meant to be as clean and humanly readable as possible.
//	The "Project ORKA" engine is divided into
//		1. data stored in structs (yes everything is public)
//		2. functions that act on that data in form of void or member functions
//	We are trying to avoid con/destructors and inheritance and advanced features like macros, lambdas, templates, etc...
//	Everything is written in camelCase and datatypes starts with upper case while variables and functions start with lower case
//	For example "ThisIsADataType" vs "thisIsAFunction"

//	About the engine:
//	This engine uses a left-handed coordinate system like Blender:
//	+X = right
//	+Y = forward
//	+Z = up

// FEATURE LIST:
// gbuffer
// multithreading
// particle system
// mesh instancing
// input
//		mouse input
//		keyboard input
//		window events
// filesystem
//		meshimport
//		texture loading
// easy to use algorithms
// multiple window support

//games
//#include "Pong.hpp"
//#include "Sortr.hpp"
//#include "Snake.hpp"
//#include "GPUSim.hpp"
#include "Sandbox.hpp"
//#include "MSDFText.hpp"
//#include "FlappyBird.hpp"
//#include "UISandbox.hpp"
#include "Mooncrash.hpp"
//#include "CheatTest.hpp"
//#include "SimpleRTS.hpp"
//#include "MindMapper.hpp"
//#include "ImageViewer.hpp"
//#include "AIPlayground.hpp"
//#include "ProjectMelon.hpp"
//#include "QuakeMovement.hpp"
//#include "ResourceManager.hpp"
//#include "ORKAIntroSequence.hpp"
//#include "PhysicsPlayground.hpp"
//#include "DungeonsAndDiscord.hpp"
//#include "InfiniteDetailPrototype.hpp"

#define TESTING
#include "QuadtreeSystem2.hpp"


//generate 4 random values
//get average
//subtract average from all values
//get max abs
//divide all by absolute
//multiply by half of max divided by level



Int main(Int  argc, Char* argv[]) {
#ifndef TESTING

	fileSystem.init(argv[0]);

	resourceManager.init();

	Mooncrash game;
	//Sandbox game;

	game.run();



#else
	//testing code
	/////////////////////////////////////////////////////////////////////////////////////////////////

	QuadtreeSystem2 sys;

	sys.count();

	sys.subdivide(sys.levels[0][0]);

	sys.count();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	pause();
#endif // TESTING
	return 0;
}