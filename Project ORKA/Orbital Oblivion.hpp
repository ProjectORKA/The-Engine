
#pragma once

#include "Game.hpp"
#include "Engine.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Random.hpp"

#define ORBITAL_OBLIVION_PLAYFIELD_SIZE 500

struct OOPlanet {
	Vec2 location;

	void render(Renderer& r) {
		r.uniforms().mMatrix(matrixFromLocationAndSize(location, 10));
		r.rerenderMesh();
	};
};
struct OOWhale {};
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
	void updatePosition() {
		location = location + direction * speed;
	}
	OOUnit(Index team, Vec2 location, Vec2 direction);

	OOPlanet& getClosestPlanet(Vector<OOPlanet>& planets) {

		Float shortestDistance = 10000000000000000000;
		OOPlanet * current = &planets[0];

		for (OOPlanet& p : planets) {
			Float dist = distance(location, p.location);
			if (dist < shortestDistance) {
				current = &p;
				shortestDistance = dist;
			}
		}

		return *current;
	}

	void render(Engine& engine, Renderer& r) {
		r.uniforms().mMatrix(matrixFromPositionAndDirection(location, -direction));
		r.renderMesh(engine, "unit");
		//r.arrow(engine, location, location + direction);
	}
	void updateDirection(Vector<OOUnit>& neighbours, Vector<OOPlanet>& planets) {

		Int count = 1;

		target = getClosestPlanet(planets).location;

		Vec2 targetDelta = vectorFromAToB(location, target);

		Vec2 influence = direction + targetDelta;

		for (OOPlanet& p : planets) {
			Vec2 delta = vectorFromAToB(p.location, location);
			Float distance = length(delta);
			Float factor = (viewDistance * 800) / (distance * distance);

			influence += Vec2(factor) * normalize(delta);
		}



		for (OOUnit& n : neighbours) {

			Vec2 delta = vectorFromAToB(n.location, location);
			Float distance = length(delta);
			//if (distance > viewDistance)
			if(distance == 0) continue;

			Float factor = viewDistance / (distance * distance);

			influence += Vec2(factor / 2) * n.direction;
			influence += Vec2(factor * 2) * normalize(delta);
		}

		direction = normalize(direction + Vec2(turnrate) * normalize(influence));
	}
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
		teams.emplace_back(0, Color(0, 0.33333333333, 1, 1));
		teams.emplace_back(1, Color(0.65098039215, 0, 0, 1));

		//create planets
		while (planets.size() < planetCount) {
			Vec2 testLocation = randomVec2(ORBITAL_OBLIVION_PLAYFIELD_SIZE);

			for (OOPlanet& p : planets) {
				if (distance(p.location, testLocation) < planetDistance) goto skip;
			}

			planets.emplace_back(testLocation);
		skip:;
		}

		//create units
		for (UInt i = 0; i < 10000; i++) {
			units.emplace_back(0);
			units.emplace_back(1);
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

	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent toogleWireframe = InputEvent(InputType::KeyBoard, F, 1);
	InputEvent toggleBloom = InputEvent(InputType::KeyBoard, T, 1);
	InputEvent pause = InputEvent(InputType::KeyBoard, P, 1);

	void create(Window& window)override;
	void renderBloom(Engine& e, Renderer& r);
	void inputEvent(Window& window, InputEvent input) override;
	void update(Window& window) override {
		sim.update(window.renderer);
	}
	void render(Engine& e, Window& window, TiledRectangle area) override {

		Renderer& r = window.renderer;

		r.draw("main");

		r.setWireframeMode(r.wireframeMode);

		r.clearColor(Color(0, 0, 0, 1));
		r.clearDepth();
		r.setDepthTest(true);
		r.setCulling(true);

		player.update(window);
		player.render(window);

		r.useTexture(e, "orbitalOblivionReflection");
		r.useShader(e, "orbitalOblivion");
		for (OOUnit& u : sim.units) {
			u.render(e, r);
		}

		r.useMesh(e, "sphere");
		for (OOPlanet& p : sim.planets) {
			p.render(r);
		}

		if (bloom)renderBloom(e, r);

	}
};

struct OrbitalOblivion {
	void run(Engine& engine);
};
