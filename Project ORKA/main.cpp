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

//  You can already choose some games and projects to play around in.
//  Just replace the Game in the main() function with one of the projects underneath
#include "FlappyBird.hpp"
#include "MoonCrash.hpp"
#include "Orbital Oblivion.hpp"
#include "ProjectMelon.hpp"
#include "Pong.hpp"
#include "TripleNine.hpp"

#include "AIPlayground.hpp"
#include "DungeonsAndDiscord.hpp"
#include "MindMapper.hpp"
#include "GPUSim.hpp"
#include "GuessingGame.hpp"
#include "Hello Triangle.hpp"
#include "ImageViewer.hpp"
#include "ImguiExample.hpp"
#include "MSDFText.hpp"
#include "NameGenerator.hpp"
#include "Organizer.hpp"
#include "PhysicsPlayground.hpp"
#include "PrototypingSandbox.hpp"
#include "RedDotHack.hpp"
#include "Sandbox.hpp"
#include "SDFRenderer.hpp"
#include "SimpleRTS.hpp"
#include "Snake.hpp"
#include "Sortr.hpp"
#include "UISandbox.hpp"

Int main(const Int argc, Char* argv[])
{
	TripleNine game;
	game.run();
}

// windows entry point (ignore)
#include "Platform.hpp"

Int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, Int nShowCmd)
{
	return main(__argc, __argv);
}
