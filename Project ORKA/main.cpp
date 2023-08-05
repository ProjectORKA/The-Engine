//	Welcome fellow programmer!
//	My name is Samuel and im the creator of Project ORKA.
//	This is the open source code for the games and game resourceManager
//	Feel free to take a look at the code and point out all the errors :)
//	If you want to report bugs, request features or add your own code take a look at our GitHub:
//	https://github.com/ProjectORKA/Project-ORKA
//	If you want to know what Project ORKA is all about please visit:
//	https://project-orka.com

//	Legal stuff:
//	You can do whatever you want as long as its not used commercially!
//	Modify, hack, tinker around

//	About the code:
//	The following C++ code is meant to be as clean and humanly readable as possible.
//	If you are a human and you find that you can not read it please contact me :)
//	The "Project ORKA" resourceManager is divided into
//		1. data stored in structs (yes everything is public)
//		2. functions that act on that data in form of void or member functions
//	We are trying to avoid con/destructors and inheritance and advanced features like macros, lambdas, templates, etc...
//	Ideally a beginner programmer will be able to get started very quickly.
//	Everything is written in camelCase and datatypes starts with upper case while variables and functions start with lower case
//	For example "ThisIsADataType" vs "thisIsAFunction"

//	About the resourceManager:
//	This resourceManager uses a left-handed coordinate system like Blender:
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
//		mesh import
//		texture loading
// easy to use algorithms
// multiple window support

//#include "AIPlayground.hpp"
//#include "DungeonsAndDiscord.hpp"
#include "FlappyBird.hpp"
//#include "MindMapper.hpp"
//#include "GPUSim.hpp"
//#include "GuessingGame.hpp"
//#include "Hello Triangle.hpp"
//#include "ImageViewer.hpp"
//#include "ImguiExample.hpp"
//#include "MoonCrash.hpp"
//#include "MSDFText.hpp"
//#include "NameGenerator.hpp"
//#include "Organizer.hpp"
//#include "PhysicsPlayground.hpp"
//#include "Pong.hpp"
//#include "ProjectMelon.hpp"
//#include "RedDotHack.hpp"
//#include "Sandbox.hpp"
//#include "SDFRenderer.hpp"
//#include "SimpleRTS.hpp"
//#include "Snake.hpp"
//#include "Sortr.hpp"
//#include "TripleNine.hpp"
//#include "UISandbox.hpp"

// [TODO] implement opengl logger into all opengl functions
// [TODO] fix vertex shader recompilation
// [TODO] move advanced rendering systems like textRenderSystem out of renderer and into window
// (basically simplify renderer to bare bones rendering tech)
// OR separate into renderer and lowLevelRenderer
// [TODO] remove Name and NameTable and replace it with Hash and HashTabl
// [TODO] remove old thread system

Int main(const Int argc, Char* argv[])
{
	FlappyBird game;
	game.run();

	//cheatTest();
}

// windows entry point
#include "Platform.hpp"

Int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, Int nShowCmd)
{
	return main(__argc, __argv);
}
