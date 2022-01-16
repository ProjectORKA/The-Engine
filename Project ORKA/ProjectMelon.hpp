
#pragma once

#include "Game.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"
#include "ParticleSystem.hpp"
#include "Window.hpp"


//bullethell
//	autofire
//parcour
//skillbased
//


#define PROJECT_MELON_PARTICLE_SIZE 1
#define PROJECT_MELON_PARTICLE_LIFETIME 2

struct ProjectMelonPlayer : public Player {
	Vec3 location = Vec3(0);
	Int zoomFactor = 10;
	ParticleSystem smoke;

	void update() override;
	void render(Renderer& renderer) override;;
};

struct ProjectMelonGame : public Game {
	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Float mouseSensitivity = 0.02;
	ProjectMelonPlayer player;

	void update() override;
	void render(Renderer& renderer) override;;
	void mouseIsMoving(Window& window, IVec2 position);
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis);
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers);
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers);
};