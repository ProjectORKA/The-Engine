#include "Threading.hpp"

void Thread::stop()
{
	if (keepThreadRunning) {
		keepThreadRunning = false;
	}
	if(thread.joinable())thread.join();
}