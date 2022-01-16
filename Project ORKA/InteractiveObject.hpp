#pragma once

#include "Basics.hpp"

enum InteractiveObjectState {
	inactive,
	active,
	hovered,
	selected,
};

struct InteractiveObject {
	Index objectID = 0;
	InteractiveObjectState state;

	virtual void onEnter(){};
	virtual void onLeave() {};
	virtual void onClick(Int buttonID) {};
	virtual void onRelease(Int buttonID) {};
};