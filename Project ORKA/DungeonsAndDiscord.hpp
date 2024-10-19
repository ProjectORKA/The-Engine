#pragma once

#include "Transform.hpp"
#include "Game.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"
#include "SceneSystem.hpp"

// todo list
// import objects
// move objects around
// also scale and rotate them in real time
// be able to change the sky

struct Renderer;
struct Window;

struct DNDEntity
{
	// DO NOT REORDER !!!
	Name      meshName = "empty";
	Transform transform;
	// DO NOT REORDER !!!

	void render(Renderer& renderer) const;
};

struct DNDSimulation final : GameSimulation
{
	// savedata for camera
	Scene2            scene;
	Vector<DNDEntity> entities;
	Int               speedExponent = 1;
	Float             nearClipValue = 0.1f;
	Float             fieldOfView   = 80.0f;
	Float             farClipValue  = 100000.0f;
	Vec3              position      = Vec3(0.0f);
	DVec3             rotation      = Vec3(0.0f);

	void destroy() override;
	void update(Float delta) override;
	void create() override;
};

struct DNDRenderer final : GameRenderer
{
	DebugPlayer    player;
	Framebuffer    framebuffer;
	Vector<Index>  selectedObjects;
	Index          lastSelectedObject  = -1;
	Float          mouseSensitivity    = 0.0015f;
	DNDSimulation* world               = nullptr;
	InputId        selectMultiple      = InputId(InputType::KeyBoard, Shift);
	InputEvent     select              = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent     enter               = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent     exit                = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent     toggleWireframeMode = InputEvent(InputType::KeyBoard, F, true);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct DungeonsAndDiscord
{
	DNDSimulation sim;
	DNDRenderer   renderer;

	void run()
	{
		sim.start();
		renderer.connect(sim);
		ui.window("Dungeons and Discord", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
		ui.run();
		sim.stop();
	}
};

Int diceRoll(Int diceCount);
