
#pragma once

#include "Basics.hpp"

struct Renderer;

struct Transform {
	Vec3 location = Vec3(0);
	Vec3 rotation = Vec3(0);
	Float scale = 1;

	Matrix matrix();
	void render(Renderer& renderer);
};