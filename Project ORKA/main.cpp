#define DEBUG

#include <Windows.h>
#include "Program.hpp"

//test commit

int main(int argc, char *argv[]) {
	Program program;
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	main(__argc, __argv);
}
#endif