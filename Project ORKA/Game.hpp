
#pragma once

#include "Renderer.hpp"
#include "InputManager.hpp"
#include "UIElement.hpp"

struct GameSimulation{
	virtual void create() {};
	virtual void update() {};
	virtual void destroy() {};
};

struct GameRenderer : public UIElement{
	//GameRenderer* selfReplace = nullptr;
	virtual void create() {};
	virtual void destroy() {};
};