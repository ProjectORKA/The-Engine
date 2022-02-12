
#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"

struct Renderer;
struct Window;

struct UISandbox : public GameRenderer {
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void buttonIsPressed(Window& window, Key key, Int action, Int modifiers) override;
	void mouseIsPressed(Window& window, MouseButton button, Int action, Int modifiers) override;
};