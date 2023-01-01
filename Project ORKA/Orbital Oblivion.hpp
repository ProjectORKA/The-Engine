
#pragma once

#include "Game.hpp"
#include "Engine.hpp"
#include "Player.hpp"
#include "Input.hpp"

struct OOPlanet{
	Vec2 location;

	void render() {
		
	};
};
struct OOWhale{};
struct OrbitalOblivionPlayer : public Player {
	Vec3 location;
};

struct OOTeam {
	Index id;
	Color color;

	OOTeam(Index id, Color color);
};

struct OOUnit{
	Index team;
	Vec2 location;
	Vec2 direction;
	
	OOUnit(Index team, Vec2 location, Vec2 direction);
};

struct OrbitalOblivionSimulation : GameSimulation{
	Vector<OOTeam> teams;
	Vector<OOUnit> units;
	Vector<OOPlanet> planets;

	OrbitalOblivionSimulation();
};

struct OrbitalOblivionRenderer : GameRenderer{
	Framebuffer bloomHalf;

	OrbitalOblivionPlayer player;
	OrbitalOblivionSimulation* sim = nullptr;
	
	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent toogleWireframe = InputEvent(InputType::KeyBoard, F, 1);

	void create(Window& window)override;
	void renderBloom(Engine& e, Renderer& r);
	OrbitalOblivionRenderer(OrbitalOblivionSimulation& sim); 
	void inputEvent(Window& window, InputEvent input) override;
	void render(Engine& engine, Window& window, TiledRectangle area) override;
};

struct OrbitalOblivion{
	void run(Engine & engine);
};
