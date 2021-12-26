
#pragma once

#include "Basics.hpp"
#include "Camera.hpp"

struct Renderer;

struct Player {
	Camera camera;

	Vec3 accelerationVector = Vec3(0);
	Float speed = 1;


	virtual void update(Float delta);
	virtual void render(Renderer& renderer);
};