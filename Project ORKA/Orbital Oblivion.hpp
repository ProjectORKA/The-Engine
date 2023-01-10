
#pragma once

#include "Game.hpp"
#include "Engine.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Random.hpp"
#include "Velox_Vector.hpp"

#define ORBITAL_OBLIVION_PLAYFIELD_SIZE 500

struct OOPlanet {
	Vec2 location;

	void render(Renderer& r);
};

struct OrbitalOblivionPlayer : public DebugPlayer {
	bool debugMode = true;

	Int speedExponent = 0;
	Float baseNumber = 1.2;

	InputEvent faster = InputEvent(InputType::Scroll, 1, 1);
	InputEvent slower = InputEvent(InputType::Scroll, 1, 0);

	void update(Window& window);
	void inputEvent(Window& window, InputEvent input);
};

struct OOTeam {
	Index id;
	Color color;

	OOTeam(Index id, Color color);
};

struct OOUnit {
	Index team;
	Float viewDistance = 10;
	Float speed = 1;
	Float turnrate = 0.1;
	Vec2 location = randomVec2(ORBITAL_OBLIVION_PLAYFIELD_SIZE);
	Vec2 direction = randomUnitVec2();
	Vec2 target = randomVec2(ORBITAL_OBLIVION_PLAYFIELD_SIZE);

	OOUnit(Index team);
	void updatePosition();
	void render(Engine& engine, Renderer& r);
	OOUnit(Index team, Vec2 location, Vec2 direction);
	OOPlanet& getClosestPlanet(Vector<OOPlanet>& planets);
	void updateDirection(Vector<OOUnit>& neighbours, Vector<OOPlanet>& planets);
};

struct OrbitalOblivionSimulation : GameSimulation {

	//teams
	Vector<OOTeam> teams;
	//planets
	UInt planetCount = 5;
	Float planetDistance = 200;
	Vector<OOPlanet> planets;
	//units
	Vector<OOUnit> units;

	OrbitalOblivionSimulation() {
		//create teams
		teams.emplaceBack(0, Color(0, 0.33333333333, 1, 1));
		teams.emplaceBack(1, Color(0.65098039215, 0, 0, 1));

		//create planets
		while (planets.size() < planetCount) {
			Vec2 testLocation = randomVec2(ORBITAL_OBLIVION_PLAYFIELD_SIZE);

			for (OOPlanet& p : planets) {
				if (distance(p.location, testLocation) < planetDistance) goto skip;
			}

			planets.emplaceBack(testLocation);
		skip:;
		}

		//create units
		for (UInt i = 0; i < 1000; i++) {
			units.emplaceBack(0);
			units.emplaceBack(1);
		}
	}
	void update(Renderer& renderer) {
		if (!renderer.time.paused) {

			for (OOUnit& u : units) {
				u.updatePosition();
			}

			for (OOUnit& u : units) {
				u.updateDirection(units, planets);
			}
		}
	}
};

struct OrbitalOblivionRenderer : GameRenderer {
	Bool bloom = true;

	OrbitalOblivionPlayer player;
	OrbitalOblivionSimulation sim;

	GPUMesh * unitMesh;

	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent toogleWireframe = InputEvent(InputType::KeyBoard, F, 1);
	InputEvent toggleBloom = InputEvent(InputType::KeyBoard, T, 1);
	InputEvent pause = InputEvent(InputType::KeyBoard, P, 1);

	void update(Window& window) override;
	void renderBloom(Engine& e, Renderer& r);
	void create(Engine& engine, Window& window)override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Engine& e, Window& window, TiledRectangle area) override;
};

struct OrbitalOblivion {
	void run(Engine& engine);
};
