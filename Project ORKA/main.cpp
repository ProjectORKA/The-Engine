//	Welcome to Project ORKA!
//	My name is Samuel and im the creator of Project ORKA.
//	This is the open source code for the engine, systems and some projects build with Project ORKA
//	Feel free to take a look at the code, play around and point out all the errors :)

//	If you want to report bugs, request features or add your own code take a look at our GitHub:
//	https://github.com/ProjectORKA/Project-ORKA

//	If you want to know what Project ORKA is all about please visit:
//	https://project-orka.com

//  If you want to chat with us or ask questions here's a Discord link:
//  https://discord.gg/NUH4bRzqwJ

//	Legal stuff:
//	You can do whatever you want as long as its not used commercially or redistributed as your own!
//	Modify, hack and tinker around as you please.
//	If you've got something that you would like to share, you can upload your work to Project ORKA
//  (Theres no simple process yet, so if you dont know how to make pull requests, just get in touch with us.)

//	Disclaimer:
//	This engine uses a left-handed coordinate system like Blender: +X = right +Y = forward +Z = up
//  This engine has clear conventions for objects, functions, variables, files, etc.. please try to respect them
//	The following code is meant to be as clean and readable as possible adhering to that standard is advised
//  Base types (float, int, bool) have been overridden as a design choice and to clearly differentiate between data and functions

#include "Engine.hpp"

//  You can already choose some games and projects to play around in.
//  Just replace the Game in the main() function with one of the projects underneath

//#include "Pong.hpp"
//#include "MoonCrash.hpp"
//#include "FlappyBird.hpp"
//#include "TripleNine.hpp"
//#include "ImageViewer.hpp"
//#include "ProjectMelon.hpp"
//#include "Orbital Oblivion.hpp"
//
//#include "Keva.hpp"
//#include "Snake.hpp"
//#include "Sortr.hpp"
//#include "GPUSim.hpp"
//#include "Sandbox.hpp"
//#include "MSDFText.hpp"
//#include "Organizer.hpp"
#include "JobSystem.hpp"
#include "TripleNine.hpp"
//#include "SimpleRTS.hpp"
//#include "SoundBars.hpp"
//#include "UISandbox.hpp"
//#include "MindMapper.hpp"
//#include "RedDotHack.hpp"
//#include "AudioSystem.hpp"
//#include "SDFRenderer.hpp"
//#include "AIPlayground.hpp"
//#include "GuessingGame.hpp"
//#include "FunctionFinder.hpp"
//#include "Hello Triangle.hpp"
//#include "Music Visualizer.hpp"
//#include "PrototypingSandbox.hpp"
//#include "PhysicsPlayground.hpp"
//#include "DungeonsAndDiscord.hpp"

void testAll(const int argc, char* argv[])
{
	//BROKEN

	//FlappyBird		 game1;  game1.run();
	//MoonCrash          game2;  game2.run();
	//OrbitalOblivion    game3;  game3.run();
	//ProjectMelon       game4;  game4.run();
	//Pong               game5;  game5.run();
	//TripleNine         game6;  game6.run();
	//AIPlayground       game7;  game7.run();
	//DungeonsAndDiscord game8;  game8.run();
	//MindMapper         game9;  game9.run();
	//GPUSim             game10; game10.run();
	//GuessingGame       game11; game11.run();
	//HelloTriangle      game12; game12.run();
	//ImageViewer        game13; game13.run(argc,argv);
	//MSDFText           game15; game15.run();
	//Organizer          game17; game17.run();
	//PhysicsPlayground  game18; game18.run();
	//PrototypingSandbox game19; game19.run();
	//Sandbox            game20; game20.run();
	//SDFRenderer        game21; game21.run();
	//SimpleRts          game22; game22.run();
	//Snake              game23; game23.run();
	//Sortr              game24; game24.run(argc, argv);
	//UISandbox          game25; game25.run();
	//Keva               game26; game26.run();
	//FunctionFinder     game27; game27.run();
}

using namespace External;

int main(const int argc, char* argv[])
{
	const Engine engine;
	engine.create(argc, argv);

	TripleNine game;
	game.run();

	//Server server;
	//server.start();
	//server.waitTillReady();


	//Client a;
	//a.start();
	//a.waitTillConnected();
	//a.stop();

	//Client b;
	//Client c;

	//b.start();
	//c.start();

	//b.waitTillConnected();
	//c.waitTillConnected();

	//pause();

	//b.stop();
	//c.stop();

	//server.stop();
	//pause();
}

#include "Windows.hpp"

#include <stdlib.h>

// ReSharper disable once CppInconsistentNaming
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return main(__argc, __argv);
}
