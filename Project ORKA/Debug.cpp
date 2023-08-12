#include "Debug.hpp"

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
