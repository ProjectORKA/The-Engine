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

#include "Sandbox.hpp"
#include "UISandbox.hpp"
#include "SimpleRts.hpp"

int main(const int argc, char* argv[])
{
	Engine engine;
	engine.create(argc, argv, false);

	SimpleRts game;
	game.run();
}
