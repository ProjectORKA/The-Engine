
#pragma once

#include "Renderer.hpp"
#include "InputManager.hpp"
#include "UIElement.hpp"

struct GameSimulation{
	Bool loaded = false;
	virtual void create() {};
	virtual void destroy() {};
	virtual void update(Float timestep) {};
};

struct GameRenderer : public UIElement{
	//GameRenderer* selfReplace = nullptr;
	virtual void create() {};
	virtual void destroy() {};
};