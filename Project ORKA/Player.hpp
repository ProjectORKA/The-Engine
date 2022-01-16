
#pragma once

#include "Basics.hpp"
#include "Camera.hpp"

struct Renderer;

struct Player {
	Camera camera;

	Vec3 accelerationVector = Vec3(0);
	Float speed = 1;
	Int speedExponent = 0;

	virtual void update() {};
	virtual void render(Renderer& renderer);
};