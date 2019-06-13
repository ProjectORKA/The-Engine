
#include "Program.hpp"

Program::Program() {

	srand(static_cast <unsigned> (time(0)));
	
	debugPrint("\nCreating a Window!");
	createNewWindow(windowHandler, gameServer);

	std::chrono::steady_clock::time_point t; 
	debugPrint("\nStarted Window Management Loop!");
	while (windowHandler.windows.size() > 0) {
		t = std::chrono::steady_clock::now();
		
		checkWindowEvents(windowHandler);
		
		t += std::chrono::milliseconds(8);
		std::this_thread::sleep_until(t);
	}
	debugPrint("\nNo more Windows! Program is being terminated!");
}
Program::~Program()
{
	debugPrint("|Program was destroyed!");
}
