#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Random.hpp"

constexpr auto orbitalOblivionPlayfieldSize = 500;

struct OOPlanet
{
	Vec2 position;

	void render(Renderer& r) const;
};

struct OrbitalOblivionPlayer final : DebugPlayer
{
	Int        speedExponent = 0;
	Float      baseNumber    = 1.2f;
	Bool       debugMode     = true;
	InputEvent faster        = InputEvent(InputType::Scroll, 1, true);
	InputEvent slower        = InputEvent(InputType::Scroll, 1, false);

	void update(Window& window);
	void inputEvent(Window& window, InputEvent input);
};

struct OOTeam
{
	Index id;
	Color color{};

	OOTeam(Index id, Color color);
};

struct OOUnit
{
	Index team;
	Float speed        = 1;
	Float viewDistance = 10;
	Float turnRate     = 0.1f;
	Vec2  direction    = randomUnitVec2Fast();
	Vec2  position     = randomVec2Fast(orbitalOblivionPlayfieldSize);
	Vec2  target       = randomVec2Fast(orbitalOblivionPlayfieldSize);

	OOUnit(Index team);
	void updatePosition();
	OOUnit(Index team, Vec2 position, Vec2 direction);
	OOPlanet& getClosestPlanet(Vector<OOPlanet>& planets) const;
	void      render(Renderer& renderer) const;
	void      updateDirection(Vector<OOUnit>& neighbors, Vector<OOPlanet>& planets);
};

struct OrbitalOblivionSimulation final : GameSimulation
{
	Vector<OOTeam>   teams;
	Vector<OOUnit>   units;
	Vector<OOPlanet> planets;
	UInt             planetCount    = 5;
	Float            planetDistance = 200;
	UInt             shipCount      = 200;

	void destroy() override;
	void update(Float delta) override;
	void create() override;
};

struct OrbitalOblivionRenderer final : GameRenderer
{
	OrbitalOblivionPlayer      player;
	GpuMesh*                   unitMesh;
	Bool                       bloom           = true;
	OrbitalOblivionSimulation* sim             = nullptr;
	InputEvent                 enter           = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent                 exit            = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent                 toggleWireframe = InputEvent(InputType::KeyBoard, F, true);
	InputEvent                 toggleBloom     = InputEvent(InputType::KeyBoard, T, true);
	InputEvent                 pause           = InputEvent(InputType::KeyBoard, P, true);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	//void renderBloom(Renderer& r);
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct OrbitalOblivion
{
	UserInterface             ui;
	Window                    window;
	OrbitalOblivionRenderer   renderer;
	OrbitalOblivionSimulation simulation;

	void run()
	{
		ui.create();
		simulation.start();
		renderer.connect(simulation);
		ui.window("Orbital Oblivion", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
		simulation.stop();
	}
};
