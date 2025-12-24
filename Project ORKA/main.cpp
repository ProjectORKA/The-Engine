//	Welcome to Project ORKA!
//	My name is Sam and im the creator of Project ORKA.
//	This is the open source code for the engine, systems and some projects build with Project ORKA
//	Feel free to take a look at the code, play around

//	If you want to report bugs, request features or add your own code take a look at our GitHub:
//	https://github.com/ProjectORKA/Project-ORKA

//	If you want to know what Project ORKA is all about please visit:
//	https://project-orka.com

//  If you want to chat with us or ask questions here's a Discord link, were always happy to help:
//  https://discord.gg/NUH4bRzqwJ

//	Legal stuff:
//	You can do whatever you want as long as it's not used commercially!
//	Modify, hack and tinker around as you please.
//	If you've got something that you would like to share, you can upload your work to Project ORKA
//  (There's no simple process yet, so if you don't know how to make pull requests, just get in touch with us.)

//	Disclaimer:
//	This engine uses a left-handed coordinate system like Blender: +X = right +Y = forward +Z = up
//  This engine has clear conventions for objects, functions, variables, files, etc.. please try to respect them
//	The following code is meant to be as clean and readable as possible adhering to that standard is advised
//  Base types (float, int, bool) have been overridden as a design choice and to clearly differentiate between data and functions

#include "Engine.hpp"

#include "Pong.hpp"
#include "SoftwareRendererGame.hpp"
#include "ShadertoyRenderer.hpp"
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

int main(const int argc, char* argv[])
{
	Engine engine;
	engine.create(argc, argv, false);
	ShadertoyGame game;
	game.run();
}
