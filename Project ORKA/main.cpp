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

//program includes
#include "GameSystem.hpp"
#include "UserInterface.hpp"

//games
#include "Pong.hpp"
#include "Intro.hpp"
#include "Sortr.hpp"
#include "GPUSim.hpp"
#include "Sandbox.hpp"
#include "UISandbox.hpp"
#include "Mooncrash.hpp"
#include "CheatTest.hpp"
#include "SimpleRTS.hpp"
#include "MindMapper.hpp"
#include "AIPlayground.hpp"
#include "ProjectMelon.hpp"
#include "QuakeMovement.hpp"
#include "ResourceManager.hpp"
#include "PhysicsPlayground.hpp"
#include "DungeonsAndDiscord.hpp"
#include "InfiniteDetailPrototype.hpp"

//#define TESTING

Int main(Int  argc, Char* argv[]) {
#ifndef TESTING

	//initializeResourcemanager();
	//initializeGameSystem();

	//DNDWorld sim;
	//DNDRenderer game(sim);

	//SimpleRTSSimulation sim;
	//SimpleRTSRenderer game(sim);

	//SandboxSimulation sim;
	//Sandbox game(sim);
	
	//MooncrashSimulation sim;
	//MooncrashRenderer game(sim);

	//PhysicsPlaygroundSimulation sim;
	//PhysicsPlayGroundRenderer game(sim);
	
	//Pong game;
	//UISandbox game;
	//MindMapper game;
	//AIPlayground game;
	//MelonRenderer game;
	//GPUSimRenderer game;
	QuakeMovementRenderer game;

	//gameSystem->add(sim);

	Window& w = window(
		"ORKA",
		Area(1920, 1080),
		true,
		WindowState::windowed
	);
	
	w.content = &game;
	game.create(w); //[TODO] clean this up

	gameSystem->run();
	ui.run();
	gameSystem->stop();

	destroyGameSystem();
	destroyResourceManager();

#else
	//testing code
	/////////////////////////////////////////////////////////////////////////////////////////////////
	Everything everythingPrototype;

	everythingPrototype.generate();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	pause();
#endif // TESTING
	return 0;
}