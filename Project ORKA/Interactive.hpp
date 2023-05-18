#pragma once

#include "Basics.hpp"
#include "Renderable.hpp"

enum InteractiveObjectState {
	inactive,
	active,
	hovered,
	selected,
};

struct Interactive : public Renderable {
	Index objectID = 0;
	InteractiveObjectState state;

	virtual void onEnter() {};
	virtual void onLeave() {};
	virtual void onClick(Int buttonID) {};
	virtual void onRelease(Int buttonID) {};
};
