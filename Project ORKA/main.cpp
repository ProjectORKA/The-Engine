#define DEBUG

#include <Windows.h>
#include "Program.hpp"
//GLAPIENTRY
int main(int argc, char *argv[]) {
	Program program;
	return 0;
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	main(__argc, __argv);
}
#endif