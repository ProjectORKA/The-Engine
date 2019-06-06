
#include "Program.hpp"

Program::Program() {
	debugPrint("|Program was created!");

	debugPrint("\nCreating a Window!");
	createNewWindow(windowHandler,gameServer);

	debugPrint("\nStarted Window Management Loop!");
	while(windowHandler.windows.size() > 0){
		checkWindowEvents(windowHandler);
	}
	debugPrint("\nNo more Windows! Program is being terminated!");
}
Program::~Program()
{
	debugPrint("|Program was destroyed!");
}

