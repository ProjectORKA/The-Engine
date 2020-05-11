#pragma once

#ifdef _WIN32
int main(int argc, char* argv[]);
#include <Windows.h>
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	return main(__argc, __argv);
}
#endif