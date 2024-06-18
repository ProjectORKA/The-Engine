#include "Debug.hpp"
#include "Windows.hpp"

void messageBox(const String& message, const String& title)
{
	MessageBox(NULL, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
}
