#include "LifetimeGuard.hpp"
#include "Debug.hpp"

ULL lifeTimerCounter = 0;

void LifetimeGuard::create()
{
	if (initialized) logError("Already initialized!");
	else
	{
		logDebug(lifeTimerCounter++);
		initialized = true;
	}
}

void LifetimeGuard::destroy()
{
	if (!initialized) logError("Object never created");
	else
	{
		logDebug(lifeTimerCounter--);
		initialized = false;
	}
	logDebug("Destroyed");
}

void LifetimeGuard::checkLifetime() const
{
	if (!initialized) logError("Object never created");
}

LifetimeGuard::LifetimeGuard()
{
	if (initialized) logError("Already initialized");
}

LifetimeGuard::~LifetimeGuard()
{
	if (initialized) logError("Object never destroyed");
}
