#pragma once

#include "Basics.hpp"
#include "CameraSystem.hpp"
#include "KeyMap.hpp"

struct CameraController {
	void mouseIsMoving(Int deltaX, Int deltaY);
	void buttonIsPressed(KeyID keyID);
};