#include "Threading.hpp"

void Thread::stop()
{
	keepThreadRunning = false;
	thread.join();
}