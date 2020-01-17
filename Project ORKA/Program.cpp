
#include "Program.hpp"

void startGLFW() {
	try {
		if (glfwInit() != GLFW_TRUE) {
			throw std::exception("Failed to initialize GLFW!");
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};
	glfwSetErrorCallback(whenGLFWThrowsError);
}

void runProgram() {
	Program program;

	srand(static_cast <unsigned> (time(0)));

	startGLFW();

	startGameSimulation(program.gameSimulation);

	std::chrono::steady_clock::time_point t;

	createNewWindow(program.windows, program.gameSimulation);

	while (program.windows.size() > 0) {
		t = std::chrono::steady_clock::now();
		t += std::chrono::milliseconds(8);

		checkWindowEvents(program);

		std::this_thread::sleep_until(t);
	}

	stopGameSimulation(program.gameSimulation);

	stopGLFW(program);
};

void stopGLFW(Program & program) {
	program.windows.clear();
	glfwTerminate();
}

void checkWindowEvents(Program& program) {
	glfwWaitEvents();
	for (std::list<Window>::iterator it = program.windows.begin(); it != program.windows.end(); it++) {
		if (it->duplicateWindow) {
			createNewWindow(program.windows, program.gameSimulation);
			it->duplicateWindow = false;
		}
		if (glfwWindowShouldClose(it->glfwWindow)) {
			destroyWindow(*it);
			program.windows.erase(it);
			return;
		}
	}
}

void createNewWindow(std::list<Window> & windows, GameSimulation& gameSimulation) {
	windows.emplace_back();
	createWindow(windows.back(), gameSimulation);
}