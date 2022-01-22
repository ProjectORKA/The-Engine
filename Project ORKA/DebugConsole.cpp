
#include "DebugConsole.hpp"
#include "UserInterface.hpp"


//void helpCommand(String s) {
//	for (Command c : debugConsole.commands) {
//		std::cout << c.command << " (" << c.description << ")\n";
//	}
//}
//void beepCommand(String s){
//	beep();
//}
//void windowCountCommand(String s) {
//	logDebug(String("Window count: ").append(toString(ui.windows.size())));
//}
//
//Command::Command(String command, String description, void(*function)(String))
//{
//	this->command = command;
//	this->description = description;
//	this->function = function;
//}
//
//void consoleInput() {
//	while (true) {
//		String s;
//		std::cout << "> ";
//		std::cin >> s;
//		for (Command c : debugConsole.commands) {
//			//[TODO] split commands by spaces
//			if (c.command == s) {
//				c.function(s);
//			}
//		}
//	}
//}
//
//DebugConsole::DebugConsole() {
//
//	addCommand("help", "prints out all currently available commands", helpCommand);
//	addCommand("beep", "plays a sound", beepCommand);
//	addCommand("windowCount", "outputs number of current windows", windowCountCommand);
//
//	std::thread(consoleInput).detach();
//}

//DebugConsole debugConsole;