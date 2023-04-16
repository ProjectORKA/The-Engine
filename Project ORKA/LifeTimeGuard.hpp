
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"

struct LifetimeGuard {
	Bool initialized = false;

	void create() {
		if (initialized) logError("Already initialized!");
		else initialized = true;
	}

	void destroy() {
		if(!initialized) logError("Object never created");
		else initialized = false;
		logDebug("Destroyed");
	}

	void checkLifetime() {
		if (!initialized) logError("Object never created");
	}

	LifetimeGuard() {
		if (initialized) logError("Already initialized");
		else logDebug("Ready for creation");
	}

	~LifetimeGuard() {
		if (initialized) logError("Object never destroyed");
		else logDebug("No errors on termination");
	}
};