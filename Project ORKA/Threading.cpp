#include "Threading.hpp"

void Thread::stop()
{
	if (thread.joinable())thread.join();
	else logError("Thread not joinable!");
}
