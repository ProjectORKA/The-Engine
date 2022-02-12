#pragma once

#include "Basics.hpp"
#include "Window.hpp"

struct InputManager {

	//Double scrollAxisYTotal = 0;

	Bool capturing = false;

	//IVec2 cursorDelta = IVec2(0, 0);
	IVec2 cursorPosition = IVec2(0, 0);

	IVec2 capturePosition = IVec2(0);

	//these functions will be called by the window/windowAPI
	Bool isCapturing(Window& window);
	void captureCursor(Window& window);
	void uncaptureCursor(Window& window);

	//callbacks
	void windowInFocus(Window& window);
	void windowOutOfFocus(Window& window);

	//void mouseIsScrolled(Window& window, Float position);
	void mouseIsMoving(Window& window, IVec2 position);
	void filesDropped(Window& window, Vector<Path> paths);
	void windowChangedFocus(Window& window, Bool isInFocus);
	void mouseWheelIsScrolled(Window& window, Double xAxis, Double yAxis);
	void buttonIsPressed(Window& window, Key key, Int action, Int modifiers);
	void mouseIsPressed(Window& window, MouseButton button, Int action, Int modifiers);
};

extern InputManager inputManager;