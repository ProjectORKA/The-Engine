#pragma once

#include "Basics.hpp"

struct InteractiveObject {
	virtual void onClick() {};
};

struct InteractionSystem {
	Map<Index, InteractiveObject*> currentInteractiveObjects;
};
