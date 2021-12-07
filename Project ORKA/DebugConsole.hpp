
#pragma once

#include "Basics.hpp"
#include "Threading.hpp"

struct Command {
	String command = "nothing";
	String description = "does nothing";
	void(*function)(String) = nullptr;

	Command(String command, String description, void(*function)(String));
};

struct DebugConsole {
	List<Command> commands;
	void addCommand(String command, String description, void(*function)(String)) {
		commands.push_back(Command(command, description, function));
	}
	DebugConsole();
};

//extern DebugConsole debugConsole;