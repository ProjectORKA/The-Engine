#include "Debug.hpp"
#include "Windows.hpp"

void beep()
{
	std::cout << '\a';
}

void pause()
{
	if(debugLoggingIsEnabled)
	{
#ifdef _WIN32
		system("pause");
#else
	std::cout << "Thread was paused..." << "\n";
	char a;
	std::cin >> a;
#endif
	}
}

void messageBox(String message, String title) {
	MessageBox(NULL, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
}
