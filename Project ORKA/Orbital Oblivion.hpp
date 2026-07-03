#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Random.hpp"

constexpr auto orbitalOblivionPlayfieldSize = 500;

struct OOPlanet
{
	Vec2 position;

	void render(Renderer& r) const
	{
		r.uniforms().setMMatrix(matrixFromPositionAndSize(position, 10));
		r.meshSystem.currentMesh().render(r);
	}
};

struct OrbitalOblivionPlayer final : DebugPlayer
{
	Int        speedExponent = 0;
	Float      baseNumber    = 1.2f;
	Bool       debugMode     = true;
	InputEvent faster        = InputEvent(InputType::Scroll, 1, true);
	InputEvent slower        = InputEvent(InputType::Scroll, 1, false);

	void update(Window& window)
	{
		//get frequently used info
		const Float delta = window.renderer.deltaTime();

		//set up temporary data
		Vec3 movementVector = Vec3(0);

		//process input
		if (ui.capturing) camera.rotate(window.mouseDelta * DVec2(mouseSensitivity));
		if (window.pressed(forward)) movementVector += camera.getForwardVector();
		if (window.pressed(backward)) movementVector -= camera.getForwardVector();
		if (window.pressed(right)) movementVector += camera.getRightVector();
		if (window.pressed(left)) movementVector -= camera.getRightVector();
		if (window.pressed(up)) movementVector += camera.getUpVector();
		if (window.pressed(down)) movementVector -= camera.getUpVector();

		//calculate movement
		if (length(movementVector) > 0)
		{
			//if there is movement input
			const Float desiredSpeed = powf(baseNumber, static_cast<Float>(speedExponent)); //calculate speed
			movementVector           = normalize(movementVector);                           //get direction of movement (just direction)
			movementVector *= desiredSpeed * delta;                                         //add speed to direction
			camera.setPosition(camera.getPosition() + movementVector);                      //add it to cameras position
		}
	}

	void inputEvent(Window& window, InputEvent& input)
	{
		if (input == faster) speedExponent++;
		if (input == slower) speedExponent--;
	}
};

struct OOTeam
{
	Index id;
	Color color{};

	OOTeam(Index id, Color color)
	{
		this->id    = id;
		this->color = color;
	}
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

	OOUnit(const Index team)
	{
		this->team = team;
	}

	void updatePosition()
	{
		position = position + direction * speed;
	};

	OOUnit(Index team, Vec2 position, Vec2 direction)
	{
		this->team      = team;
		this->position  = position;
		this->direction = direction;
	}

	OOPlanet& getClosestPlanet(Vector<OOPlanet>& planets) const
	{
		Float     shortestDistance = 10000000000000000000.0f;
		OOPlanet* current          = &planets.front();

		for (OOPlanet& p : planets)
		{
			if (const Float dist = distance(position, p.position); dist < shortestDistance)
			{
				current          = &p;
				shortestDistance = dist;
			}
		}

		return *current;
	}

	void render(Renderer& renderer) const
	{
		renderer.uniforms().setMMatrix(matrixFromPositionAndDirection(position, direction));
		renderer.renderMesh("unit");
	}

	void updateDirection(Vector<OOUnit>& neighbors, Vector<OOPlanet>& planets)
	{
		target = getClosestPlanet(planets).position;

		const Vec2 targetDelta = vectorFromAToB(position, target);

		Vec2 influence = direction + targetDelta;

		for (const OOPlanet& p : planets)
		{
			Vec2        delta    = vectorFromAToB(p.position, position);
			const Float distance = length(delta);
			const Float factor   = viewDistance * 800 / (distance * distance);

			influence += Vec2(factor) * normalize(delta);
		}

		for (OOUnit& n : neighbors)
		{
			Vec2        delta    = vectorFromAToB(n.position, position);
			const Float distance = length(delta);
			//if (distance > viewDistance)
			if (distance == 0.0f) continue;

			const Float factor = viewDistance / (distance * distance);

			influence += Vec2(factor / 2) * n.direction;
			influence += Vec2(factor * 2) * normalize(delta);
		}

		direction = normalize(direction + Vec2(turnRate) * normalize(influence));
	}
};

struct OrbitalOblivionSimulation final : GameSimulation
{
	Vector<OOTeam>   teams;
	Vector<OOUnit>   units;
	Vector<OOPlanet> planets;
	UInt             planetCount    = 5;
	Float            planetDistance = 200;
	UInt             shipCount      = 200;

	void destroy() override
	{
		loaded = false;
	};

	void update(Float delta) override
	{
		if (delta != 0.0f)
		{
			for (OOUnit& u : units) u.updatePosition();
			for (OOUnit& u : units) u.updateDirection(units, planets);
		}
	}

	void create() override
	{
		//create teams
		teams.emplace_back(0, Color(0, 0.33333333333, 1, 1));
		teams.emplace_back(1, Color(0.65098039215, 0, 0, 1));

		//create planets
		while (planets.size() < planetCount)
		{
			Vec2 testPosition = randomVec2Fast(orbitalOblivionPlayfieldSize);

			for (const OOPlanet& p : planets) if (distance(p.position, testPosition) < planetDistance) goto skip;

			planets.emplace_back(testPosition);

		skip:;
		}

		//create units
		for (UInt i = 0; i < shipCount; i++)
		{
			units.emplace_back(0);
			units.emplace_back(1);
		}

		loaded = true;
	}
};

struct OrbitalOblivionRenderer final : GameRenderer
{
	OrbitalOblivionPlayer      player;
	GPUMesh*                   unitMesh;
	Bool                       bloom           = true;
	OrbitalOblivionSimulation* sim             = nullptr;
	InputEvent                 enter           = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent                 exit            = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent                 toggleWireframe = InputEvent(InputType::KeyBoard, F, true);
	InputEvent                 toggleBloom     = InputEvent(InputType::KeyBoard, T, true);
	InputEvent                 pause           = InputEvent(InputType::KeyBoard, P, true);

	void update(Window& window) override {}
	void destroy(Window& window) override;

	void connect(GameSimulation& simulation) override
	{
		this->sim = dynamic_cast<OrbitalOblivionSimulation*>(&simulation);
	}

	void inputEvent(Window& window, InputEvent& input) override
	{
		if (input == enter) ui.capture(*this);
		if (input == exit) ui.releaseCapture();
		if (input == toggleWireframe) window.renderer.wireframeMode = !window.renderer.wireframeMode;
		if (input == toggleBloom) bloom = !bloom;
		if (input == pause)
		{
			if (window.renderer.time.isPaused()) window.renderer.time.unpause();
			else window.renderer.time.pause();
		}
		player.inputEvent(window, input);
	}

	void create(Window& window) override;

	void renderScene(Window& window) override;
	void renderInteractive(Window& window) override {};
};

struct OrbitalOblivion
{
	OrbitalOblivionRenderer   renderer;
	OrbitalOblivionSimulation simulation;

	void run()
	{
		simulation.start();
		renderer.connect(simulation);
		ui.window("Orbital Oblivion", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
		simulation.stop();
	}
};
