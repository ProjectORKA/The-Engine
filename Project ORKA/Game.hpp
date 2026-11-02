#pragma once

#include "Framebuffer.hpp"
#include "Time.hpp"
#include "UserInterface.hpp"

struct GameSimulation
{
	void stop();
	void start();

	virtual void create() = 0;
	virtual void destroy() = 0;
	virtual void update(Float delta) = 0;

	[[nodiscard]] Bool  isLoaded() const;
	[[nodiscard]] Float getTickRate() const;
	[[nodiscard]] Bool  getKeepRunning() const;

protected:
	Thread thread;
	Float  tickRate    = 60;
	Bool   loaded      = false;
	Bool   keepRunning = false;
};

void gameSimulationThread(GameSimulation& sim);

// Legacy game root: sizes from UIElement layout + optional FBO composite.
// New apps should use AppHost + <interactive-surface tags> (layout rect from UiLayoutEngine).
struct GameRenderer : UIElement
{
	Framebuffer framebuffer;
	Bool        framebufferInitialized = false;
	Bool        renderToFramebuffer    = true;

	void create(Window& window) override;
	void render(Window& window) override;
	void destroy(Window& window) override;

	virtual void renderScene(Window& window) = 0;
	virtual void connect(GameSimulation& simulation) = 0;
};
