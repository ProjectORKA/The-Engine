#pragma once
#include "Basics.hpp"

extern ULL lifeTimerCounter;

struct LifetimeGuard
{
	Bool initialized = false;

	void create();
	void destroy();
	LifetimeGuard();
	~LifetimeGuard();
	void checkLifetime() const;
};
