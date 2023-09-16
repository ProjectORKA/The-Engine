
#pragma once
#include "Renderer.hpp"
#include "ResourceManager.hpp"

struct PongPlayer;

struct PongBall
{
	List<Vec2> trail;
	Float desiredSpeed = 2;
	Bool  stuckToPaddle = true;
	Bool  stuckToPaddle1 = true;
	Vec2  position = Vec2(0);
	Vec2  velocity = Vec2(1, 0);

	void update(Float deltaTime, PongPlayer players[2]);
	void render(ResourceManager& resourceManager, Renderer& renderer) const;
};