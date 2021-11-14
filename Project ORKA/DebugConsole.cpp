
#include "DebugConsole.hpp"

void helpCommand(String s) {
	for (Command c : debugConsole.commands) {
		std::cout << c.command << " (" << c.description << ")\n";
	}
}
void beepCommand(String s){
	beep();
}

Command::Command(String command, String description, void(*function)(String))
{
	this->command = command;
	this->description = description;
	this->function = function;
}

void consoleInput() {
	while (true) {
		String s;
		std::cout << "> ";
		std::cin >> s;
		for (Command c : debugConsole.commands) {
			//[TODO] split commands by spaces
			if (c.command == s) {
				c.function(s);
			}
		}
	}
}

DebugConsole::DebugConsole() {

	addCommand("help", "prints out all currently available commands", helpCommand);
	addCommand("beep", "plays a sound", beepCommand);

	std::thread(consoleInput).detach();
}

DebugConsole debugConsole;