#pragma once

#include "Debug.hpp"
#include "Basics.hpp"

struct Window;

struct Button{
	Bool isPressed;
};

struct InputManager {
	//states
	Button forward;
	Button backward;
	Button left;
	Button right;
	Button up;
	Button down;
	Button faster;
	Button slower;

	Double scrollAxisYTotal = 200;

	Bool capturing = false;

	//IVec2 cursorDelta = IVec2(0, 0);
	IVec2 cursorPosition = IVec2(0,0);
	
	IVec2 capturePosition = IVec2(0);

	Float mouseSensitivity = 0.0015f;
	
	//these functions will be called by the window/windowAPI
	Bool isCapturing(Window& window);
	void captureCursor(Window& window);
	void uncaptureCursor(Window& window);

	//callbacks
	void windowInFocus(Window& window);
	void windowOutOfFocus(Window& window);

	//void mouseIsScrolled(Window& window, Float position);
	void mouseIsMoving(Window& window, IVec2 position);
	void windowChangedFocus(Window& window, Bool isInFocus);
	void mouseWheelIsScrolled(Window& window, Double xAxis, Double yAxis);
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers);
};

extern InputManager inputManager;